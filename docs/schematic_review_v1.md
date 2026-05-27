# LTD-3700B Schematic Review (V1)

> Target: `circuit/LTD-3700B_V1.pdf` (rev 1.1, 2026-03-19, 4 pages A4)
> Scope: component-value-only review per README rule (no topology changes)
> Date: 2026-05-18

---

## 1. Summary

| Severity | Count | Notes |
|----------|-------|-------|
| Recommend change (High) | 3 | Reliability / safety margin |
| Investigate (Medium) | 4 | Confirm design intent before changing |
| Informational (Low) | 4 | Currently OK; optional optimisations |

---

## 2. High - recommended changes

### H1. C1, C2 (MCU decoupling) voltage rating margin (Sheet 1)

| Item | Current | Recommended | Reason |
|------|---------|-------------|--------|
| C1 | 0.1 uF / **6.3 V** | 0.1 uF / **10 V** or 16 V | 3V0_BAT rail but battery insertion / brownout transients can briefly exceed 5 V. MLCC 6.3 V derates to ~4 V effective. |
| C2 | 0.1 uF / **6.3 V** | 0.1 uF / **10 V** or 16 V | Same reason. |

Long-term reliability. Cost delta is essentially zero at X7R 0603.

### H2. C12 (IR LED pulse smoothing) voltage rating (Sheet 3)

| Item | Current | Recommended | Reason |
|------|---------|-------------|--------|
| C12 | 100 uF / **6.3 V** | 100 uF / **10 V** | Large ripple under 100-150 mA IR-LED pulses. 6.3 V on an electrolytic is thin margin. |

Alternative: replace with 2 x 47 uF / 10 V X5R MLCCs in parallel.

### H3. V1 (EMR_IO surge / ESD protection) is unpopulated (Sheet 4)

| Item | Current | Recommended |
|------|---------|-------------|
| V1 | **NC** | Populate TVS or varistor |

If EMR_IO leaves the PCB via CON3, ESD strikes go directly into the MCU. Suggested parts: SMAJ5.0CA, ESD9B5.0ST5G, or equivalent 5 V clamp TVS.

---

## 3. Medium - confirm design intent

### M1. R3 (TACT_SW / DSCL shared path) = 2 kohm (Sheet 1)

R3 sits in series with DSCL between the MCU pin and the tact switch SW2. The arrangement reuses the DSCL line for button detection.
- If DSCL is an I2C clock during normal operation, 2 kohm series will distort the signal. Recommend 4.7 kohm to 10 kohm.
- If DSCL is just a GPIO during button check, 2 kohm is fine.

Check firmware: what is MCU P12 actually used for?

### M2. R34 / R30 (PAM8302A input) = 100 ohm (Sheet 2)

PAM8302A internal gain formula: Av = 2 * (142 kohm / Rin). With Rin = 100 ohm this gives gain ~ 2840 (~ 69 dB) - clearly not the intended use. The 100 ohm resistors are almost certainly **series input resistors**, not gain-setting resistors. Combined with the 1 uF coupling cap they form an HPF with fc = 1 / (2 pi * 100 * 1u) = ~1.6 kHz - which would attenuate the low end of voice audio.

Either:
- The intended gain network is implemented elsewhere and 100 ohm is just a damping/protection R - in which case the coupling caps may need to be 4.7 uF or larger to keep low-end response.
- The gain network is missing and Rin should be ~10-24 kohm.

### M3. R26 (EMR_IO pullup or load) = 220 kohm (Sheet 4)

RC time constant with C11 (270 pF) = 60 us (fc ~ 2.7 kHz). Suitable as an input pullup for a slow external signal. If EMR_IO is meant to drive a load externally, 220 kohm is too high. Confirm direction.

### M4. R23 (IR LED pulse current set) = 10 ohm (Sheet 3)

I_LED ~ (3.0 V - 1.5 V_Vf - 0.1 V_Vcesat) / 10 ohm = ~140 mA peak pulse. Typical chamber LEDs run 50-200 mA so this is in range. Optionally raise to 15 ohm or 18 ohm (~100 mA) to extend battery life if the chamber SNR allows.

---

## 4. Low - informational

### L1. R1, R2 (status LED current limit) = 180 ohm (Sheet 1)
Gives ~5.5 mA per LED. Fine for blinky indicators. If running continuously, raising to 1 kohm (~1 mA) cuts standby drain by 5x while still being visible.

### L2. R12 (photodiode TIA feedback) = 10 Mohm + C10 = 10 pF (Sheet 3)
Bandwidth = 1 / (2 pi * 10M * 10p) = ~1.6 kHz - well matched to chamber pulse detection. Value itself is correct but the 10 Mohm node is sensitive to PCB cleanliness and humidity - require IPA cleaning + conformal coating at assembly.

### L3. R7 = 180 ohm, R8 = 20 kohm, C5 = 0.1 uF, C9 = 1 nF (ADPCM LPF) (Sheet 2)
Two-pole LPF, both stages at ~8 kHz. Correct for the 8 kHz ADPCM audio bandwidth.

### L4. NTC divider: R10 = 15 kohm + RT2 50 kohm @ 25 C, B = 3950 K (Sheet 3)
Predicted V_TEMP_IN at LDO23 = 2.3 V supply:

| Temp | NTC R | V_TEMP_IN |
|------|-------|-----------|
| 25 C | 50.0 kohm | 0.53 V |
| 60 C | 12.4 kohm | 1.26 V |
| 80 C | 6.4 kohm  | 1.60 V |
| 90 C | 4.5 kohm  | 1.77 V |
| 100 C | 3.0 kohm | 1.92 V |

Resolution in the 57-93 C fire-detection window is good. No change needed.

---

## 5. BOM change summary

| Designator | From | To | Severity |
|------------|------|----|----------|
| C1, C2 | 0.1 uF / 6.3 V | 0.1 uF / **10 V or 16 V** | High |
| C12 | 100 uF / 6.3 V | 100 uF / **10 V** | High |
| V1 | NC | TVS (e.g. SMAJ5.0CA) - **populate** | High |
| R3 | 2 kohm 1% | (review) 4.7-10 kohm | Medium |
| R34, R30 | 100 ohm | (review) confirm gain topology | Medium |
| R26 | 220 kohm | (review) depends on direction | Medium |
| R23 | 10 ohm 1% | (review) 15-18 ohm option | Medium |
| R1, R2 | 180 ohm | 1 kohm (if low-power mode) | Low |

---

## 6. Out-of-scope notes (per README)

Topology / routing items below are recorded for future reference only; not part of this value-only review:

1. R12 (10 Mohm) node guard ring on PCB - reduce surface leakage.
2. PAM8302A BTL output ferrite-bead + cap EMI filter on OUT_P/N if RF emissions are a concern.
3. Battery polarity protection is already handled by U7 (DMP3099LQ) P-MOSFET - OK.

---

*End of review v1.*
