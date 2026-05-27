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
*   Calibration model
*   -----------------
*   At production (clean air + 12% standard smoke generator):
*       baseline   = Si_Da @ 0% smoke           (uint16_t)
*       delta_meas = Si_Da @ 12% smoke - baseline
*       gain_Q8    = (CALIB_REF_DELTA << 8) / delta_meas
*
*   At runtime, every Si_Da reading is normalized:
*       Si_Da_norm = ((Si_Da_raw - baseline) * gain_Q8) >> 8
*   so every calibrated unit reports CALIB_REF_DELTA at 12% smoke.
*
*   Fire alarm trigger: Si_Da_norm > CALIB_ALARM_THRESHOLD for N consecutive
*   readings -> SYS_mode = In_Fire_al_mode.
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

/* Reference value: every calibrated unit reports this number at 12% smoke. */
#define CALIB_REF_DELTA          150u

/* Alarm threshold on the normalized Si_Da value. Selected so that ~8% smoke
 * (==~ 100 / 150 of the calibration smoke level) trips the alarm. */
#define CALIB_ALARM_THRESHOLD    100u

/* Default gain (1.0 in Q8 = 256) used when no calibration is present. With
 * baseline=0 this yields Si_Da_norm == Si_Da_raw - i.e. no compensation. */
#define CALIB_DEFAULT_GAIN_Q8    256u
#define CALIB_DEFAULT_BASELINE   0u

/* Minimum acceptable delta between 12% smoke and clean air during the
 * production calibration step. Below this the unit is treated as failed
 * (LED/PD/chamber problem) and the calibration is rejected. */
#define CALIB_MIN_DELTA          30u

/* Number of consecutive samples averaged for each calibration step. */
#define CALIB_SAMPLES            4u

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
extern uint8_t  g_calib_valid;     /* 1 = loaded from DataFlash, 0 = defaults */

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

/* Blocking 2-step calibration procedure driven by SW2 + LED feedback.
 * Call when SW2 is detected as held at boot. The function never returns;
 * after saving (or failing) the unit waits for power cycle to come up
 * with the new calibration. */
void calib_run_procedure(void);

#endif /* __CALIB_H_ */

/* --------------------------------- End Of File ------------------------------ */
