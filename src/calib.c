/**
*   @file       calib.c
*   @brief      Per-unit Si_Da calibration backed by A96L416 internal
*               DataFlash. See calib.h for the design overview, the on-flash
*               layout and the runtime correction formula.
*
*   Operational summary
*   -------------------
*   - calib_load()                : low-level read + validate. Populates
*                                   g_calib_* or installs defaults.
*   - calib_save(b, g)            : low-level erase + page write + verify.
*   - calib_apply(raw)            : called on every Si_Da reading in the
*                                   main loop. Applies
*                                   ((raw - baseline) * gain_Q8) >> 8.
*   - calib_step1_boot_led()      : STEP 1. Power-on indication, both
*                                   LEDs on 0.2 s.
*   - calib_erase_with_animation(): STEP 2-1. SW2 held at boot. LED G/R
*                                   alternate while held; on release erase
*                                   DataFlash, capture clean-air baseline,
*                                   save {baseline, gain=256}.
*   - calib_boot_init()           : STEP 2-2. SW2 not held - load the saved
*                                   record into the RAM globals.
*   - calib_run_smoke()           : STEP 3. Triggered from the main loop
*                                   when SW2 is held in normal mode while a
*                                   smoke calibration is still pending.
*                                   place phase + 5-sample mean + gain save.
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
uint16_t g_calib_baseline    = CALIB_DEFAULT_BASELINE;
uint16_t g_calib_gain_Q8     = CALIB_DEFAULT_GAIN_Q8;
uint8_t  g_calib_valid       = 0;
uint8_t  g_calib_needs_smoke = 0;

/* DataFlash user IDs - see vendor a96l41x_dataflash.c. */
#define DF_USER_ID_ERASE     0xA901358F
#define DF_USER_ID_PAGEWR    0x4F17DC86

/* DataFlash region 0x3000..0x30FF is a separate area (UM 4.4): it has no
 * relation with RAM nor program FLASH and "can be read by using DPTR",
 * i.e. with an XDATA (MOVX @DPTR) access - NOT MOVC. Reading it with a
 * 'code' pointer returns program-code space at 0x3000 (empty -> 0x00),
 * which is why earlier read-backs always came back 0. Use an 'xdata'
 * pointer so the DataFlash array is addressed correctly. */
static uint16_t calib_read_word(uint16_t offset)
{
	unsigned char xdata *p = (unsigned char xdata *)CALIB_ADDR;
	uint16_t lo = p[offset];
	uint16_t hi = p[offset + 1];
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
 * calib_wait_df_ready - poll the DataFlash busy bit until the self-timed     *
 * erase/write operation completes. UM 20: do not start a new DataFlash op    *
 * (or read back) while DFMBUSY (DFMCR.7) is still 1. A guard count keeps the *
 * loop bounded so a stuck controller cannot hang the firmware.               *
 *----------------------------------------------------------------------------*/
static void calib_wait_df_ready(void)
{
	uint16_t guard = 0;
	while ((DFMCR & (1 << DFMBUSY)) != 0) {
		if (++guard >= 50000) {
			break;                 /* safety timeout */
		}
	}
}

/*----------------------------------------------------------------------------*
 * calib_save - erase + page-write the 32-byte calibration page, then verify *
 * with a fresh read-back. Each DataFlash op is self-timed, so we wait on     *
 * DFMBUSY before issuing the next op or reading the result back.             *
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
	calib_wait_df_ready();
	Delay_ms(1);
	DATAFLASH_PageWt(DF_USER_ID_PAGEWR, CALIB_ADDR, buf);
	calib_wait_df_ready();
	Delay_ms(1);

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
 * SW2 polling helper                                                         *
 *   sw2_is_pressed : 1 when the button is held down (P12 reads 0 with the    *
 *                   internal pull-up enabled).                               *
 *----------------------------------------------------------------------------*/
static uint8_t sw2_is_pressed(void)
{
	return (Port_GetInputpinValue(PORT1, PIN2) == 0);
}

/* Long delay broken into 100 ms chunks so WDT (RESET mode, ~2 s timeout)
 * stays happy. */
static void wait_ms_kicked(uint16_t total_ms)
{
	uint16_t left = total_ms;
	while (left >= 100) {
		Delay_ms(100);
		WD_Reset();
		left -= 100;
	}
	if (left) {
		Delay_ms(left);
		WD_Reset();
	}
}

/* Generic blink helpers: half_ms on, half_ms off, repeated `cycles` times.
 * WDT-safe via wait_ms_kicked. */
static void blink_g(uint8_t cycles, uint16_t half_ms)
{
	uint8_t i;
	for (i = 0; i < cycles; i++) {
		LED_G_ON;
		wait_ms_kicked(half_ms);
		LED_G_OFF;
		wait_ms_kicked(half_ms);
	}
}

static void blink_r(uint8_t cycles, uint16_t half_ms)
{
	uint8_t i;
	for (i = 0; i < cycles; i++) {
		LED_R_ON;
		wait_ms_kicked(half_ms);
		LED_R_OFF;
		wait_ms_kicked(half_ms);
	}
}

/*----------------------------------------------------------------------------*
 * calib_step1_boot_led - STEP 1.                                             *
 *   Power-on indication: LED_G and LED_R on together for 0.2 s, then off.    *
 *----------------------------------------------------------------------------*/
void calib_step1_boot_led(void)
{
	LED_G_ON;
	LED_R_ON;
	Delay_ms(CALIB_BOOT_LED_MS);
	LED_G_OFF;
	LED_R_OFF;
	WD_Reset();
}

/*----------------------------------------------------------------------------*
 * calib_erase_with_animation - STEP 2-1 (SW2 held at power-up).              *
 *                                                                            *
 *   While SW2 stays held: alternate LED_G / LED_R every CALIB_ANIM_MS.       *
 *   When SW2 is released:                                                    *
 *     - erase the DataFlash calibration page                                *
 *     - reset the RAM globals to defaults (baseline=0, gain=256, valid=0)    *
 *     - take ONE Dust_ADC_2AMP() reading as the clean-air baseline           *
 *       (the unit must be in clean air at the moment of release)             *
 *     - save {baseline, gain=256}, set g_calib_needs_smoke = 1               *
 *         success -> LED_G blink 5x at 0.2 s                                 *
 *         failure -> LED_R blink 5x at 0.2 s                                 *
 *----------------------------------------------------------------------------*/
void calib_erase_with_animation(void)
{
	uint16_t baseline;

	/* Alternate G/R while the operator keeps SW2 held. */
	while (sw2_is_pressed()) {
		LED_G_ON;
		LED_R_OFF;
		wait_ms_kicked(CALIB_ANIM_MS);
		if (!sw2_is_pressed()) {
			break;
		}
		LED_G_OFF;
		LED_R_ON;
		wait_ms_kicked(CALIB_ANIM_MS);
	}
	LED_G_OFF;
	LED_R_OFF;

	/* SW2 released -> erase the calibration page. */
	DATAFLASH_PageEr(DF_USER_ID_ERASE, CALIB_ADDR);
	calib_wait_df_ready();
	Delay_ms(1);

	/* Reset RAM globals to the uncalibrated defaults. */
	g_calib_baseline = CALIB_DEFAULT_BASELINE;   /* 0   */
	g_calib_gain_Q8  = CALIB_DEFAULT_GAIN_Q8;    /* 256 */
	g_calib_valid    = 0;

	/* Capture the clean-air baseline (Dust_ADC_2AMP averages 8 internally). */
	baseline = Dust_ADC_2AMP();

	/* Persist baseline with unity gain; smoke gain still to be performed. */
	g_calib_needs_smoke = 1;
	if (calib_save(baseline, CALIB_DEFAULT_GAIN_Q8)) {
		blink_g(CALIB_RESULT_BLINKS, CALIB_BLINK_FAST_MS);   /* save OK   */
	} else {
		blink_r(CALIB_RESULT_BLINKS, CALIB_BLINK_FAST_MS);   /* save fail */
	}
}

/*----------------------------------------------------------------------------*
 * calib_boot_init - STEP 2-2 (SW2 NOT held at power-up).                     *
 *                                                                            *
 *   Load the saved record from DataFlash and update the RAM globals          *
 *   (baseline, gain_Q8). If no valid record exists the defaults stay in      *
 *   place (baseline=0, gain=256), which leaves readings uncompensated -      *
 *   the safest fallback. g_calib_needs_smoke is set when the gain is still   *
 *   the default, i.e. smoke calibration has not been performed yet.          *
 *----------------------------------------------------------------------------*/
void calib_boot_init(void)
{
	if (calib_load()) {
		if(uart_debug_mode == uart_debug_On){
			Uart_Out_Text(Flash_Read_OK, 17);
			Uart_Out();
		}
	} else {
		if(uart_debug_mode == uart_debug_On){
			Uart_Out_Text(Flash_Read_Fail, 19);
			Uart_Out();
		}
	}

	/* g_calib_gain_Q8 == default means no smoke cal has been done. */
	g_calib_needs_smoke = (g_calib_gain_Q8 == CALIB_DEFAULT_GAIN_Q8) ? 1 : 0;
}

/*----------------------------------------------------------------------------*
 * calib_run_smoke - STEP 3 (smoke gain calibration).                         *
 *                                                                            *
 *   Entered from the main loop when SW2 is held while g_calib_needs_smoke    *
 *   is set. Sequence:                                                        *
 *      a. LED_R solid, wait for SW2 release.                                 *
 *      b. "Place sample" phase: LED_R blinks at 0.2 s for                    *
 *         CALIB_SMOKE_PLACE_MS (5 s) - time for the operator to put the      *
 *         sensor into the smoke generator.                                   *
 *      c. Sampling: LED_R blinks at 0.5 s, take CALIB_SMOKE_SAMPLES          *
 *         Dust_ADC_2AMP() readings (~5 s total); smoke_avg = mean.           *
 *      d. delta = smoke_avg - baseline.                                      *
 *           delta <  CALIB_MIN_DELTA -> reject: LED_R fast blink 5 s,        *
 *                                       g_calib_needs_smoke kept = 1 (retry).*
 *           delta >= CALIB_MIN_DELTA -> gain_Q8 = (CALIB_REF_DELTA<<8)/delta,*
 *                                       calib_save(baseline, gain_Q8):       *
 *                                         success -> LED_G fast blink 1 s,   *
 *                                                    g_calib_needs_smoke = 0 *
 *                                         failure -> LED_G fast blink 1 s,   *
 *                                                    g_calib_needs_smoke = 1 *
 *                                                                            *
 *   Returns to main() in either case (does NOT halt the loop). On success    *
 *   the main loop (STEP5) runs fire_monitor_step() immediately so the same   *
 *   12% smoke injection also validates the alarm on the production line.     *
 *----------------------------------------------------------------------------*/
void calib_run_smoke(void)
{
	uint32_t sum;
	uint16_t smoke_avg;
	uint16_t delta;
	uint32_t gain32;
	uint16_t gain_Q8;
	uint16_t elapsed;
	uint8_t  i;

	/* Step a: LED_R on, wait for SW2 release. */
	LED_R_ON;
	while (sw2_is_pressed()) {
		WD_Reset();
		Delay_ms(20);
	}

	/* Step b: "place the sample" - LED_R blinks at 0.2 s for 5 s. */
	elapsed = 0;
	while (elapsed < CALIB_SMOKE_PLACE_MS) {
		LED_R_ON;
		wait_ms_kicked(CALIB_SMOKE_PLACE_BLINK_MS);
		LED_R_OFF;
		wait_ms_kicked(CALIB_SMOKE_PLACE_BLINK_MS);
		elapsed += (uint16_t)(2u * CALIB_SMOKE_PLACE_BLINK_MS);
	}

	/* Step c: CALIB_SMOKE_SAMPLES readings, LED_R toggling at 0.5 s. */
	sum = 0;
	for (i = 0; i < CALIB_SMOKE_SAMPLES; i++) {
		LED_R_ON;
		sum += Dust_ADC_2AMP();
		wait_ms_kicked(CALIB_SMOKE_BLINK_MS);
		LED_R_OFF;
		wait_ms_kicked(CALIB_SMOKE_BLINK_MS);
	}
	LED_R_OFF;
	smoke_avg = (uint16_t)(sum / CALIB_SMOKE_SAMPLES);

	/* Step d: validate delta against the baseline captured in step 2-1. */
	if (smoke_avg <= g_calib_baseline) {
		delta = 0;
	} else {
		delta = smoke_avg - g_calib_baseline;
	}

	if (delta < CALIB_MIN_DELTA) {
		/* Not enough smoke - reject. Keep the flag so the operator can
		 * retry by holding SW2 again. */
		blink_r((uint8_t)(CALIB_FAIL_BLINK_MS / (2u * CALIB_BLINK_FAST_MS)),
		        CALIB_BLINK_FAST_MS);
		g_calib_needs_smoke = 1;
		return;
	}

	/* gain_Q8 = (REF << 8) / delta. */
	gain32  = ((uint32_t)CALIB_REF_DELTA) << 8;
	gain32 /= delta;
	if (gain32 > 0xFFFFu) {
		gain32 = 0xFFFFu;
	}
	gain_Q8 = (uint16_t)gain32;

	if (calib_save(g_calib_baseline, gain_Q8)) {
		blink_g((uint8_t)(CALIB_OK_BLINK_MS / (2u * CALIB_BLINK_FAST_MS)),
		        CALIB_BLINK_FAST_MS);
		g_calib_needs_smoke = 0;
	} else {
		/* Save / verify failed - keep the flag so the operator can retry. */
		blink_g((uint8_t)(CALIB_OK_BLINK_MS / (2u * CALIB_BLINK_FAST_MS)),
		        CALIB_BLINK_FAST_MS);
		g_calib_needs_smoke = 1;
	}

	/* Fire monitoring after a successful calibration is done by the main
	 * loop (STEP5 calls fire_monitor_step() right after this returns). */
}

/* --------------------------------- End Of File ------------------------------ */
