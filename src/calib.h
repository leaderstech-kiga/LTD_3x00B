/**
*   @file       calib.h
*   @brief      Per-unit Si_Da calibration (offset + gain) stored in
*               A96L416 internal DataFlash. Compensates for sample-to-sample
*               variance of LED brightness / PD sensitivity / OPAMP offset
*               so a single fixed alarm threshold can be used in firmware.
*
*   Measured variance (7 samples, OPAMP GAIN1_X30, chp_125KHz):
*       Smoke 0%  : 6 ~ 75   (12x range  -> offset compensation needed)
*       Smoke 12% : 100 ~ 250 (2.5x range -> gain compensation needed)
*       Delta     : 90 ~ 196  (2.2x range)
*
*   Three-step initialisation (aligned with the existing product family)
*   --------------------------------------------------------------------
*     STEP 1 - FLASH ERASE (SW2 held at power-up)
*               Alternate green / red LED 3 times, then erase the
*               DataFlash record. After the erase, control falls through
*               to step 2 - so a single "SW2 held at boot" gesture
*               resets everything and re-arms the unit for a fresh
*               calibration.
*
*     STEP 2 - BASELINE CAPTURE (every boot)
*               If DataFlash holds a valid record, load it and continue.
*               If the record is empty / corrupt, blink LED_G at 0.5 s
*               on/off and wait for the operator to press SW2 once. The
*               first SW2 press triggers a single Dust_ADC_2AMP() reading
*               which is stored as the clean-air baseline (with the
*               default unity gain). The unit then continues normally.
*
*     STEP 3 - SMOKE GAIN CALIBRATION (short SW2 press in normal mode)
*               If the loaded record still uses the default gain (no
*               smoke calibration has ever been performed), the unit
*               arms a short-press listener in the main loop. A SW2
*               press shorter than 500 ms while running in Normal mode
*               enters the smoke calibration: 10 s wait with LED_R solid,
*               then 5 s of sampling with LED_R blinking at 0.5 s, then
*               gain_Q8 is computed and written so the average smoke
*               reading normalizes to CALIB_REF_DELTA (=200). The
*               alarm trip point is 80% of REF (=160).
*
*   Runtime correction
*   ------------------
*       Si_Da_norm = ((Si_Da_raw - baseline) * gain_Q8) >> 8
*       Fire alarm trigger: Si_Da_norm > CALIB_ALARM_THRESHOLD for
*           FIRE_TRIGGER_COUNT consecutive readings.
*/
#ifndef __CALIB_H_
#define __CALIB_H_

#include "typedef.h"

/*----------------------------------------------------------------------------*
 * Public constants                                                           *
 *----------------------------------------------------------------------------*/

/* Magic value written to DataFlash to recognize a valid calibration record.
 * Reads as 0xFFFF on an erased / never-written DataFlash, so any other value
 * means "stale or corrupted - reject". */
#define CALIB_MAGIC              0xCA1B

/* DataFlash region: 256 bytes at 0x3000. We use the very first page (32 B). */
#define CALIB_ADDR               0x3000

/* Reference value: every calibrated unit reports this number at the
 * fire-detection smoke concentration used during step 3 calibration. */
#define CALIB_REF_DELTA          200u

/* Alarm threshold on the normalized Si_Da value. 80% of CALIB_REF_DELTA -
 * Si_Da_norm crossing this triggers the fire counter. */
#define CALIB_ALARM_THRESHOLD    160u

/* Default gain (1.0 in Q8 = 256) used until a smoke calibration has been
 * performed. baseline=0 + gain=256 leaves readings uncompensated. */
#define CALIB_DEFAULT_GAIN_Q8    256u
#define CALIB_DEFAULT_BASELINE   0u

/* Minimum acceptable delta between smoke reading and baseline during
 * step 3 ("delta_min"). Below this the calibration is rejected (operator
 * did not insert into smoke, smoke generator off, LED/PD/chamber problem,
 * or simply not enough smoke). On reject g_calib_needs_smoke stays 1 so
 * the operator can retry. Default 50 (7-sample set has deltas 90..196). */
#define CALIB_MIN_DELTA          50u

/* STEP 1 / STEP 2-1 LED timing (ms). */
#define CALIB_BOOT_LED_MS        200u     /* STEP 1: both LEDs on time      */
#define CALIB_ANIM_MS            200u     /* STEP 2-1: G/R alternate half   */
#define CALIB_BLINK_FAST_MS      100u     /* 0.2 s blink half-period        */
#define CALIB_RESULT_BLINKS      5u       /* STEP 2-1 result: 5 blinks      */

/* STEP 3 timing (ms / counts) - see calib_run_smoke(). */
#define CALIB_SMOKE_PLACE_MS       5000u  /* "place sample" LED_R 0.2 s blink */
#define CALIB_SMOKE_PLACE_BLINK_MS 100u   /* place-phase blink half-period    */
#define CALIB_SMOKE_SAMPLES        5u     /* averaged smoke readings (~5 s)   */
#define CALIB_SMOKE_BLINK_MS       500u   /* sampling LED_R blink half-period */
#define CALIB_FAIL_BLINK_MS        5000u  /* reject indication total          */
#define CALIB_OK_BLINK_MS          1000u  /* success indication total         */

/*----------------------------------------------------------------------------*
 * On-flash layout (8 bytes, fits in first 8 bytes of the 32-byte page).      *
 *----------------------------------------------------------------------------*/
typedef struct {
    uint16_t magic;        /* CALIB_MAGIC if valid                   */
    uint16_t baseline;     /* clean-air Si_Da (offset)               */
    uint16_t gain_Q8;      /* gain x 256 (e.g. 256 == 1.0)           */
    uint16_t checksum;     /* magic ^ baseline ^ gain_Q8             */
} CalibData_t;

/*----------------------------------------------------------------------------*
 * Working copy in RAM. Populated by calib_load() at boot. Read by            *
 * calib_apply() on every Dust_ADC_2AMP() result.                             *
 *----------------------------------------------------------------------------*/
extern uint16_t g_calib_baseline;
extern uint16_t g_calib_gain_Q8;
extern uint8_t  g_calib_valid;        /* 1 = loaded from DataFlash, 0 = defaults */

/* 1 = smoke calibration has NOT yet been performed (gain still at default).
 * The main loop watches this flag together with SW2: a SW2 hold starts
 * calib_run_smoke(). Cleared after a successful smoke calibration save. */
extern uint8_t  g_calib_needs_smoke;

/*----------------------------------------------------------------------------*
 * Public API                                                                 *
 *----------------------------------------------------------------------------*/

/* Read DataFlash record, validate magic + checksum, populate g_calib_*.
 * Returns 1 on success (valid record loaded), 0 on missing/corrupt
 * (defaults installed). */
uint8_t calib_load(void);

/* Write a new {baseline, gain_Q8} record to DataFlash, then verify by
 * reading back. Returns 1 on success, 0 on verify failure. */
uint8_t calib_save(uint16_t baseline, uint16_t gain_Q8);

/* Apply current calibration to a raw Si_Da reading:
 *     out = ((raw - baseline) * gain_Q8) >> 8
 * Underflow (raw < baseline) saturates to 0, overflow saturates to UINT16_MAX. */
uint16_t calib_apply(uint16_t si_da_raw);

/* STEP 1 - power-on indication. Turns LED_G and LED_R on together for
 * CALIB_BOOT_LED_MS (0.2 s), then off. Called first at boot. */
void calib_step1_boot_led(void);

/* STEP 2-1 - flash erase + baseline capture (SW2 held at power-up).
 * Alternates LED_G / LED_R at CALIB_ANIM_MS while SW2 is held. On release:
 * erases the DataFlash page, resets the RAM globals to defaults, takes one
 * Dust_ADC_2AMP() reading as the clean-air baseline, saves {baseline,
 * gain=256} and sets g_calib_needs_smoke = 1.
 *   success -> LED_G blink 5x at 0.2 s ; failure -> LED_R blink 5x at 0.2 s */
void calib_erase_with_animation(void);

/* STEP 2-2 - boot-time load (SW2 NOT held at power-up). Calls calib_load()
 * to update baseline / gain_Q8 from DataFlash; defaults stay if no valid
 * record. Sets g_calib_needs_smoke when the gain is still default. */
void calib_boot_init(void);

/* STEP 3 - smoke gain calibration. Entered from the main loop when SW2 is
 * held while g_calib_needs_smoke is set.
 *   - LED_R solid, wait for SW2 release
 *   - "place sample" phase: LED_R blinks 0.2 s for CALIB_SMOKE_PLACE_MS
 *   - sampling: LED_R blinks 0.5 s, CALIB_SMOKE_SAMPLES readings averaged
 *   - delta = smoke_avg - baseline
 *       delta <  CALIB_MIN_DELTA -> reject, LED_R fast blink 5 s, flag kept
 *       delta >= CALIB_MIN_DELTA -> gain_Q8 = (CALIB_REF_DELTA<<8)/delta,
 *                                   save; LED_G fast blink 1 s; flag cleared
 *                                   on success (kept on save failure). */
void calib_run_smoke(void);

#endif /* __CALIB_H_ */

/* --------------------------------- End Of File ------------------------------ */
