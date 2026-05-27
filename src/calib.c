/**
*   @file       calib.c
*   @brief      Per-unit Si_Da calibration backed by A96L416 internal
*               DataFlash. See calib.h for the design overview, the on-flash
*               layout and the runtime correction formula.
*
*   Operational summary
*   -------------------
*   - calib_load()           : called once from main(). Validates magic +
*                              checksum, populates g_calib_*. On any failure
*                              the working values fall back to defaults so
*                              detection still works (uncalibrated).
*   - calib_apply(raw)       : called on every Si_Da reading in the main
*                              loop. Applies (raw - baseline) * gain_Q8 >> 8.
*   - calib_run_procedure()  : entered when SW2 is held at boot. Drives a
*                              2-step measurement (clean air -> SW2 press,
*                              then 12% smoke -> SW2 press), writes the
*                              result to DataFlash and busy-loops so the
*                              user can power-cycle into the new state.
*/

#include "A96L41x.h"
#include "intrins.h"
#include "delay.h"
#include "typedef.h"
#include "a96l41x_gpio.h"

#include "main.h"
#include "calib.h"
#include "a96l41x_dataflash.h"

/*----------------------------------------------------------------------------*
 * Working copy of calibration. See calib.h for the public extern decl.       *
 *----------------------------------------------------------------------------*/
uint16_t g_calib_baseline = CALIB_DEFAULT_BASELINE;
uint16_t g_calib_gain_Q8  = CALIB_DEFAULT_GAIN_Q8;
uint8_t  g_calib_valid    = 0;

/* DataFlash user IDs - see vendor a96l41x_dataflash.c. */
#define DF_USER_ID_ERASE     0xA901358F
#define DF_USER_ID_PAGEWR    0x4F17DC86

/* DataFlash region 0x3000..0x30FF is mapped into the 8051 'code' space, so
 * we can read it with a plain code pointer. */
static uint16_t calib_read_word(uint16_t offset)
{
	code unsigned char *p = (code unsigned char *)(CALIB_ADDR + offset);
	uint16_t lo = p[0];
	uint16_t hi = p[1];
	return (uint16_t)((hi << 8) | lo);
}

/*----------------------------------------------------------------------------*
 * calib_load                                                                 *
 *----------------------------------------------------------------------------*/
uint8_t calib_load(void)
{
	uint16_t magic, baseline, gain, checksum, expected;

	magic    = calib_read_word(0);
	baseline = calib_read_word(2);
	gain     = calib_read_word(4);
	checksum = calib_read_word(6);

	expected = magic ^ baseline ^ gain;

	if (magic != CALIB_MAGIC) {
		goto bad;
	}
	if (checksum != expected) {
		goto bad;
	}
	if (gain == 0) {
		goto bad;                  /* zero gain would zero every reading */
	}

	g_calib_baseline = baseline;
	g_calib_gain_Q8  = gain;
	g_calib_valid    = 1;
	return 1;

bad:
	g_calib_baseline = CALIB_DEFAULT_BASELINE;
	g_calib_gain_Q8  = CALIB_DEFAULT_GAIN_Q8;
	g_calib_valid    = 0;
	return 0;
}

/*----------------------------------------------------------------------------*
 * calib_save - erase + page-write the 32-byte calibration page, then verify *
 * with a fresh read-back. The vendor DataFlash sequence is self-timed; we    *
 * add a small delay after each op as a margin.                               *
 *----------------------------------------------------------------------------*/
uint8_t calib_save(uint16_t baseline, uint16_t gain_Q8)
{
	uint8_t buf[SECTOR_SIZE_BYTE];
	uint16_t magic = CALIB_MAGIC;
	uint16_t checksum;
	uint8_t i;

	checksum = magic ^ baseline ^ gain_Q8;

	for (i = 0; i < SECTOR_SIZE_BYTE; i++) {
		buf[i] = 0xFF;             /* erased flash convention */
	}
	buf[0] = (uint8_t)(magic       & 0xFF);
	buf[1] = (uint8_t)(magic       >> 8);
	buf[2] = (uint8_t)(baseline    & 0xFF);
	buf[3] = (uint8_t)(baseline    >> 8);
	buf[4] = (uint8_t)(gain_Q8     & 0xFF);
	buf[5] = (uint8_t)(gain_Q8     >> 8);
	buf[6] = (uint8_t)(checksum    & 0xFF);
	buf[7] = (uint8_t)(checksum    >> 8);

	DATAFLASH_PageEr(DF_USER_ID_ERASE, CALIB_ADDR);
	Delay_ms(5);
	DATAFLASH_PageWt(DF_USER_ID_PAGEWR, CALIB_ADDR, buf);
	Delay_ms(5);

	/* Verify by re-reading the four words. */
	if (calib_read_word(0) != magic)    return 0;
	if (calib_read_word(2) != baseline) return 0;
	if (calib_read_word(4) != gain_Q8)  return 0;
	if (calib_read_word(6) != checksum) return 0;

	/* Update RAM copy so the caller does not need a reset to use the new
	 * values for any subsequent calls. */
	g_calib_baseline = baseline;
	g_calib_gain_Q8  = gain_Q8;
	g_calib_valid    = 1;
	return 1;
}

/*----------------------------------------------------------------------------*
 * calib_apply                                                                *
 *----------------------------------------------------------------------------*/
uint16_t calib_apply(uint16_t si_da_raw)
{
	uint32_t scaled;

	if (si_da_raw <= g_calib_baseline) {
		return 0;                  /* below baseline: clean-air noise */
	}
	scaled  = (uint32_t)(si_da_raw - g_calib_baseline);
	scaled *= g_calib_gain_Q8;
	scaled >>= 8;
	if (scaled > 0xFFFFu) {
		return 0xFFFFu;
	}
	return (uint16_t)scaled;
}

/*----------------------------------------------------------------------------*
 * SW2 polling helpers                                                        *
 *   - sw2_is_pressed : 1 when the button is held down (P12 sees 0 with the   *
 *                      internal pull-up).                                    *
 *   - wait_for_press / wait_for_release : blocking with WDT refresh.         *
 *----------------------------------------------------------------------------*/
static uint8_t sw2_is_pressed(void)
{
	return (Port_GetInputpinValue(PORT1, PIN2) == 0);
}

static void wait_for_press(void)
{
	while (!sw2_is_pressed()) {
		WD_Reset();
		Delay_ms(20);
	}
}

static void wait_for_release(void)
{
	while (sw2_is_pressed()) {
		WD_Reset();
		Delay_ms(20);
	}
}

/* Indicator LED helpers used during calibration. */
static void blink_green_slow(uint8_t cycles)
{
	uint8_t i;
	for (i = 0; i < cycles; i++) {
		LED_G_ON;
		Delay_ms(150);
		LED_G_OFF;
		Delay_ms(850);
		WD_Reset();
	}
}

static void blink_red_slow(uint8_t cycles)
{
	uint8_t i;
	for (i = 0; i < cycles; i++) {
		LED_R_ON;
		Delay_ms(150);
		LED_R_OFF;
		Delay_ms(850);
		WD_Reset();
	}
}

static void blink_both_fast(uint8_t cycles)
{
	uint8_t i;
	for (i = 0; i < cycles; i++) {
		LED_G_ON; LED_R_ON;
		Delay_ms(80);
		LED_G_OFF; LED_R_OFF;
		Delay_ms(80);
		WD_Reset();
	}
}

/*----------------------------------------------------------------------------*
 * Si_Da averaging helper - takes CALIB_SAMPLES measurements and returns the  *
 * mean. The OPAMP setup is owned by Dust_ADC_2AMP() so we call it directly.  *
 *----------------------------------------------------------------------------*/
static uint16_t measure_si_da_average(void)
{
	uint32_t sum = 0;
	uint8_t  i;

	for (i = 0; i < CALIB_SAMPLES; i++) {
		WD_Reset();
		sum += Dust_ADC_2AMP();
		Delay_ms(50);
	}
	return (uint16_t)(sum / CALIB_SAMPLES);
}

/*----------------------------------------------------------------------------*
 * calib_run_procedure                                                        *
 *                                                                            *
 *   State machine, LED legend                                                *
 *   ----------------------------                                             *
 *   Boot SW2 held detected            -> blink_both_fast (8)                 *
 *   Waiting for SW2 release           -> LED_G solid                         *
 *   Step 1 (place in clean air,                                              *
 *           press SW2 to capture 0%)  -> blink_green_slow until press        *
 *   Capturing 0%                       -> blink_both_fast (4)                *
 *   Step 2 (apply 12% smoke,                                                 *
 *           press SW2 to capture 12%) -> blink_red_slow until press          *
 *   Capturing 12%                      -> blink_both_fast (4)                *
 *   Save OK                            -> LED_G solid for 3 s                *
 *   Save fail / delta too small        -> LED_R rapid for 5 s                *
 *   Then: busy loop with WDT refresh (user power-cycles to leave cal mode).  *
 *----------------------------------------------------------------------------*/
void calib_run_procedure(void)
{
	uint16_t baseline;
	uint16_t smoke;
	uint16_t delta;
	uint32_t gain32;
	uint16_t gain_Q8;
	uint8_t  ok;
	uint8_t  i;

	/* Entry acknowledge - tell the operator that calibration was detected
	 * (so a stray button bump can be distinguished from real entry). */
	blink_both_fast(8);

	/* The operator is still holding SW2 from the boot trigger - wait for
	 * the release before starting step 1 so the next press is unambiguous. */
	LED_G_ON;
	wait_for_release();
	LED_G_OFF;
	Delay_ms(200);

	/* ------ Step 1 : capture clean-air baseline ------ */
	while (!sw2_is_pressed()) {
		WD_Reset();
		LED_G_ON;  Delay_ms(150);
		LED_G_OFF; Delay_ms(850);
	}
	blink_both_fast(4);
	baseline = measure_si_da_average();
	wait_for_release();
	Delay_ms(200);

	/* ------ Step 2 : capture 12% smoke ------ */
	while (!sw2_is_pressed()) {
		WD_Reset();
		LED_R_ON;  Delay_ms(150);
		LED_R_OFF; Delay_ms(850);
	}
	blink_both_fast(4);
	smoke = measure_si_da_average();
	wait_for_release();
	Delay_ms(200);

	/* ------ Validate + compute gain ------ */
	if (smoke <= baseline) {
		delta = 0;
	} else {
		delta = smoke - baseline;
	}

	if (delta < CALIB_MIN_DELTA) {
		/* Bad measurement: rapid red 5 s then halt. Operator must check
		 * smoke generator / sample alignment and power cycle. */
		for (i = 0; i < 25; i++) {
			LED_R_ON;  Delay_ms(100);
			LED_R_OFF; Delay_ms(100);
			WD_Reset();
		}
		goto halt;
	}

	gain32 = ((uint32_t)CALIB_REF_DELTA << 8);
	gain32 /= delta;
	if (gain32 > 0xFFFFu) {
		gain32 = 0xFFFFu;
	}
	gain_Q8 = (uint16_t)gain32;

	ok = calib_save(baseline, gain_Q8);
	if (ok) {
		LED_G_ON;
		for (i = 0; i < 30; i++) {
			Delay_ms(100);
			WD_Reset();
		}
		LED_G_OFF;
	} else {
		/* Save / verify failed - DataFlash issue. Slow red blink. */
		blink_red_slow(5);
	}

halt:
	/* Park here so the operator can power-cycle into the new calibration.
	 * WDT is still in RESET mode, so refresh it from this loop. */
	while (1) {
		WD_Reset();
		LED_G_ON;  Delay_ms(50);
		LED_G_OFF; Delay_ms(950);
	}
}

/* --------------------------------- End Of File ------------------------------ */
