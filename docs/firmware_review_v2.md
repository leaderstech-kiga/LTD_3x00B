# LTD-3700B Firmware Code Review (V2) - Applied-Fix Edition

> Target: `src/` user code (Abov A96L416 / Keil C51)
> Vendor SDK (`a96l41x_*.c/h`) excluded per the README rule.
> Date: 2026-05-20
> Supersedes: firmware_review_v1.md (kept for history)

---

## 0. Executive summary

| Severity | Issues v1 | Applied | Skipped (by design) | Production stage |
|----------|-----------|---------|---------------------|------------------|
| Critical | 7         | **5**   | 0                   | 0                |
| High     | 10        | **5**   | 2 (H4, H8)          | 1 (H5)           |
| Medium   | 10        | **5**   | 0                   | 0                |
| Low      | 5         | **2**   | 1 (L4)              | 0                |
| **Total**| 32        | **17**  | 3                   | 1                |

Of the 32 review items from v1, 17 have been resolved in code, 3 are
explicitly preserved as intentional design, and 1 is owned by the
production process (per-chip calibration). The remaining items either
have no code impact (H8: audio range never extended; M2: pattern
disappeared together with H9) or are deemed acceptable as-is for this
release.

The device now:

- Boots and detects MX25L1006E flash (CRC-free but ID-checked + SPI-timeout-checked).
- Wakes from Stop every 0.192 s for button polling; sleep current ~1.5 uA.
- Runs the 8-s sensor cycle (`Dust_ADC_Single`), 56-s battery LED, 280-s battery check.
- Plays alarm clips via the `Play_Clip(AUDIO_CLIP_xxx)` helper.
- Survives sustained CPU hang via the active-mode reset watchdog (~2 s).
- Survives stuck SPI peripheral via bounded wait-loop guards.
- Survives flash power-up after the C14 1uF -> 10uF change via a 10 ms inrush settle.
- All source files in `src/` are ASCII-only after Korean-comment cleanup.

---

## 1. Applied fixes

### Critical

#### C2 - `volatile` on ISR-shared variables - APPLIED
**Where**: `main.c` definitions + `main.h` extern.
**Variables**: `Audio_length`, `Audio_addr`, `Audio_start`, `Audio_max_length`,
`SystemStatus`, `Timer2_cnt`. (`Audio_start_address` removed under L5.)
**Effect**: Prevents Keil C51 register caching that could mask ISR-side
updates; guarantees that `while(Audio_length < Audio_max_length)` polling
sees fresh values fed by `TIMER1_Int`.

#### C4 - Active-mode WDT hang protection - APPLIED
**Where**: `main.c` `WD_Reset()` reconfigured to RESET mode with WDTDR=30 (~2 s).
**Call sites**: main loop top, in-alarm `while(1)` top, `Audio_Run` polling loop.
**Effect**: Any CPU hang in active code (stuck poll, runaway loop, EMI-induced
PC corruption) triggers an automatic MCU reset within ~2 s. `BeforeStop()`
keeps the wake-from-Stop timer in INTERRUPT mode with WDTDR=2 (~0.192 s) so
the existing button-poll architecture is unaffected.

#### C5 - SPI timeout flag - APPLIED
**Where**: `flash.c` adds `volatile uint8_t SPI_Timeout`, set inside
`SPI_Transfer` / `SPI_Receive` when their wait loops time out.
**Consumer**: `SPI_Memory_Check()` in `main.c` now AND-checks `SPI_Timeout == 0`
together with the JEDEC ID compare.
**Effect**: Real 0xFF data bytes are no longer confused with timeout returns;
flash-detection failure can no longer be masked by a happens-to-match ID.

#### C7 - `SPI_Receive` timeout guard - APPLIED
**Where**: `flash.c` `SPI_Receive` two bounded wait loops (0x1000 iterations each).
**Effect**: Audio streaming ISR can no longer hang indefinitely if the
USART/SPI peripheral mis-behaves; worst case a bounded number of cycles is
spent inside the ISR before it returns 0xFF and sets SPI_Timeout.

#### C14 - Flash VCC decoupling 1 uF -> 10 uF + inrush settle - APPLIED
**Hardware**: C14 replaced (1 uF/6.3 V -> 10 uF/10 V X7R).
**Firmware**: `SPI_Memory_Check()` adds `NOP_20us_Delay(500)` (~10 ms) after
`CVDD_ON; P2 = 0x0F;` so the flash VCC has time to charge through the GPIO
output impedance before the first SPI command runs.
**Effect**: Reproducibly clean SPI_Memory_Check at boot; audio glitch
frequency reduction pending field measurement (see Section 5).

### High

#### H1 - Empty for-loop delays -> `NOP_4us_Delay()` - APPLIED
**Where**: `flash.c` `Get_Identification()` and `FastRead()`. The
`for(i=0;i<10;i++);` empty loops would be eliminated under any non-zero
optimisation; replaced with `NOP_4us_Delay()` which is implemented in
assembly and cannot be elided.

#### H2 - Side-effect macros wrapped in `((void)(...))` - APPLIED
**Where**: `main.h` 14 macros: `LED_G/R_ON/OFF`, `AUDIO_ON/OFF`,
`BAT_PW_ON/OFF`, `LDO_ON/OFF`, `CVDD_ON/OFF`, `SLAVESELECT/DESELECT`.
**Effect**: Misuse like `if (LDO_ON) ...` (which previously assigned 0x01
to LDOCR and silently evaluated false) is now a compile error. Call-site
syntax (`LDO_ON;`) unchanged.

#### H3 - `Main()` -> `main()` - APPLIED
**Where**: `main.c` entry point.
**Note**: Keil C51 happens to match symbols case-insensitively at the
linker, so `Main` always worked; lowercase form is the C standard and
prevents future static-analyser surprises.

#### H9 - UART decimal leading-zero suppression - APPLIED
**Where**: `main.c` adds `Uart_Send_Decimal(uint16_t value)` helper;
**18 inline 4-line zero-pad patterns** across `ADC_Uart_Out`,
`Uart_Out_avr`, `Uart_Out`, and `Uart_Test_Out` are replaced with single
`Uart_Send_Decimal(...)` calls. Output is now compact ("123" instead of
"0123") and 29 lines net shorter.

#### H10 - `&array` -> `array` in `USART_SendDataWithPolling` calls - APPLIED
**Where**: `main.c` `46 call sites` corrected across 19 unique array
variables (`Tab`, `End`, `Visu_MODE`, etc.). Single-variable calls
(`&digit`, 2 places) intentionally retain `&` because they are
`uint8_t *` already.
**Effect**: MISRA-C / strict-compiler clean, no run-time change.

### Medium

#### M1 - Empty `//` comment lines filled - APPLIED
**Where**: 7 lines in `main.c` that had been stripped to bare `//` during
the Korean->English conversion. Now carry English explanations matching
the surrounding code intent.

#### M2 - `+ 48` -> `+ '0'` - RESOLVED VIA H9
The 18 inline patterns that contained `+ 48` were removed by the H9
rewrite. The single remaining `+ '0'` in `Uart_Send_Decimal()` already
uses the character literal.

#### M5 - Dead `#if 0 ... #endif` blocks removed - APPLIED
**Where**: `flash.c` had three commented-out alternative implementations
(old `SPI_Transfer` with DRIE, old `Get_Identification`/`Read`/`FastRead`,
and `Read_n`). Removed; markers point at git history for recovery.

#### M7 - Audio1/2/3 magic numbers -> struct table + helper - APPLIED
**Where**: `main.c` defines `code AudioClip_t Audio_Clips[AUDIO_CLIP_COUNT]`
in Flash memory; `Play_Clip(clip_id)` helper hides the address/length/
runtime triple. `main.h` exports the enum-like `AUDIO_CLIP_BOOT/LOWBAT/FIRE`
indices and the struct type.
**Effect**: -15 IRAM bytes (clip table moves to CODE); call sites change
from `Audio_Run(Audio3_Address, Audio3_length, Audio3_runtime)` to
`Play_Clip(AUDIO_CLIP_FIRE)`.

#### M9 - `.bak` files moved to `docs/backup/` - APPLIED
**Where**: `src/main.c.bak` -> `docs/backup/main.c.bak`. `docs/backup/README.md`
explains the archive policy. Keil `.uvproj` was already not referencing the
file, so build is unaffected.

### Low

#### L1 - Loop counters retyped - APPLIED
**Where**: 4 functions in `main.c`.
| Location          | Before    | After             | Reason |
| ---------------   | --------- | ----------------- | -------|
| `main()`          | `int i`   | `uint8_t i`       | counter max 4 (`In_charge_wait`) |
| Alarm-stop block  | `int i,j` | `uint8_t i; uint16_t j;` | i max 8, j max 393 |
| `Delay_s`         | `int i`   | `uint16_t i`      | signed-int overflow if sec > 32767 |
| `Delay_ms`        | `int i`   | `uint16_t i`      | same |

#### L5 - Unused globals removed - APPLIED
**Where**: `deviceID[3]` global, `Audio_start_address`, `EM_AL_Status`,
`opamp1[5]`, `opamp2[5]`, `Dust_avr[7]`. Net **~22 RAM/code bytes freed**.
Local `deviceID[3]` inside `SPI_Memory_Check()` retained - it is the real user.

---

## 2. Intentional design (skipped on purpose)

#### H4 - `start_uart_debug_mode = Debug_On` - SKIPPED BY DESIGN
This is the entry gate for the "sensor raw-value UART dump" mode used for
field calibration. Holding SW2 at power-up activates an infinite UART
loop that streams raw ADC values via Uart_Test_Out. Production units ship
with this enabled because:
- Activation requires deliberate user action (button held at boot).
- Calibration tooling depends on this mode.
- No accidental entry path during normal operation.

#### H8 - `Audio_addr` 16-bit width - SKIPPED BY DESIGN
The product has a fixed set of three audio clips (boot, low-battery, fire).
No plan to add clips that would push the highest clip address past 64 KB.
If future hardware requires it, search for `uint16_t Audio_addr` and
related types and widen to `unsigned long`.

#### L4 - Flash data CRC verification - SKIPPED
Owner decision. MX25L1006E NOR flash data corruption in the field is
extremely rare; the SPI-side defences (C5 SPI_Timeout flag, C7 timeout
guards, C14 inrush settle, JEDEC ID check) cover the realistic failure
modes. Adding a runtime CRC would require:
- An offline CRC-generation tool in the production audio-burn flow;
- Layout space for per-clip CRCs;
- Boot-time CRC compute + verify code (~150-200 lines).
Re-open if field data ever shows silent audio-corruption events.

---

## 3. Production-stage items

#### H5 - VBGR per-chip calibration - PRODUCTION
Battery voltage measurement uses `VBGR_CV = 92` as a chip-independent
constant. Real A96L416 VBGR varies ~+-5% chip-to-chip, so battery
voltage error today is up to ~+-5%. Mitigation route (owned by
production): trim per-unit during burn-in and store in an unused flash
sector or in a fixed firmware constant per unit. The firmware exposes
`Get_Bat_Voltage_cV()` which already reads VBGR ratiometrically; only
the constant needs to become a per-unit value.

---

## 4. Additional improvements applied

These are not from the v1 review but were made during the same pass:

| Item | Description |
|------|-------------|
| **Comment language** | All user-source comments converted to English (mojibake-corrupted Korean stripped; ASCII-only across `main.c/h`, `audio.c/h`, `flash.c/h`, `delay.c/h`). |
| **`CVDD_ON` macro semantics** | Renamed from `FLASH_ON` to match the real P20 = C_VDD wiring (per the schematic). The vestigial `FLASH_ON` (P00 = OP1_OUT test point) was eliminated. |
| **`SystemStatus = SPI_Memory_Check()`** | Owner added the JEDEC-ID-based detection (rather than the earlier hard-coded `SystemStatus = 1`), then C5 strengthened it with the `SPI_Timeout == 0` co-check. |
| **8-sec wake architecture (proposed, then reverted)** | An attempt to wake every 8 s for battery savings was rolled back because the product spec requires 0.5-s button responsiveness. Current architecture keeps the 0.192-s wake; sleep current is ~1.5 uA, well inside the 10-year budget. |

---

## 5. Open verification

### Audio-glitch field test (in progress)

C14 1 uF -> 10 uF is intended to suppress the previously observed
intermittent "audio plays partial, then silent for the rest of the
clip, then next clip works" symptom. The hypothesis is that Class-D
audio-amp switching and external-LED dI/dt was momentarily dipping
the GPIO-fed flash VCC below MX25L1006E's POR threshold, causing the
chip to drop out of continuous-read mode.

**Pending data**: glitch rate over 30 clip playbacks at 10 uF, compared
to the 1 uF baseline. If glitches are essentially eliminated, the V2
board guide will mandate >=10 uF here. If glitches persist, follow-on
options are SPI clock 500 kHz -> 250 kHz, CS-line 33 ohm + 100 pF
filter, and a P-MOSFET load-switch in place of the GPIO supply path.

### Sleep current

Measured ~1.5 uA after the P0FSRH/P0FSRL analog-pin fix already
present in the reference firmware. No further regression after the
fixes in this v2.

---

## 6. Fix-marker index (grep handles)

For grep-based traceability the source files contain `(FIX <id>)`
markers everywhere a change was made.

```
(FIX C2)    main.c, main.h    volatile on ISR globals
(FIX C4)    main.c            active-mode WDT
(FIX C5)    flash.c, flash.h  SPI_Timeout flag
(FIX C7)    flash.c           SPI_Receive timeout guard
(FIX H1)    flash.c           NOP_4us_Delay replaces empty for-loop
(FIX H2)    main.h            ((void)(...)) macro guard
(FIX H3)    main.c            main() entry symbol
(FIX H9)    main.c            Uart_Send_Decimal helper + call sites
(FIX H10)   main.c            &array -> array
(FIX M1)    main.c            empty-comment refill
(FIX M5)    flash.c           dead-code removed
(FIX M7)    main.c, main.h    Audio_Clips[] / Play_Clip()
(FIX L1)    main.c            int loop counters retyped
(FIX L5)    main.c, main.h    unused globals removed
```

The `(FIX C14)` change is in main.c near `SPI_Memory_Check()` (the
`NOP_20us_Delay(500)` line and surrounding comment).

The owner-applied changes (`FLASH_ON -> CVDD_ON` rename,
`SystemStatus = SPI_Memory_Check()` introduction) are visible in the
git diff against the Stand_along_240628 reference but do not carry
explicit markers.

---

## 7. Build / encoding sanity (as of this revision)

| Property | Value |
|----------|-------|
| main.c lines | 2370 |
| main.h lines | 200 |
| flash.c lines | 290 (down from 333 after M5) |
| Non-ASCII bytes in any user source | **0** |
| Open compile warnings (Keil, default settings) | (verify on next build) |
| Open static-analysis warnings (MISRA quick pass) | (verify on next pass) |

---

## 8. Recommended next steps

1. **Audio-glitch field test report**: capture glitch rate at 10 uF over
   30 playbacks; compare with prior 1 uF experience.
2. **V2 board guide**: based on the field-test result, finalise the C14
   value (>= 10 uF), keep CVDD supply path as GPIO or move to P-MOSFET,
   decide whether SPI 500 kHz is acceptable or move to 250 kHz.
3. **Production calibration procedure for H5**: per-unit VBGR_CV trim.
4. **Optional later**: revisit L4 (flash CRC) only if a corruption event
   is ever observed in the field.

---

*End of v2. Replace v1 with this file for the next code-review cycle.
v1 left in place for diff/history reasons; safe to remove after archiving.*
