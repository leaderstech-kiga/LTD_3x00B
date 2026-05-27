# LTD-3700B Firmware Code Review (V1)

> Target: `src/` directory user code (Abov A96L416 / Keil C51)
> Excluded: `a96l41x_*.c/h` (vendor SDK - per README rule)
> Date: 2026-05-18

---

## 0. Executive summary

| Severity | Count | Status |
|----------|-------|--------|
| **Critical** | **7** | Immediate fix required. 2 of them block actual fire-alarm operation today. |
| **High** | 10 | Safety / robustness; fix soon. |
| **Medium** | 10 | Maintainability / clarity. |
| **Low** | 5 | Optional. |

**Most urgent conclusions:**
1. **C1** - ADC buffer overflow (stack/global memory corruption on every ADC path).
2. **C5** - `Read_Audio_Length()` is defined but never called, so `Audio_max_length = 0` and **the alarm clip never plays a single sample**.
3. **C6** - WDT only operates in interrupt mode in `BeforeStop()`; the active code path had **no hang protection** (a fire detector that can silently lock up is unsafe).

---

## 1. Critical - fix immediately

### C1. ADC buffer overflow - `sizeof()` vs element count

**Where**: `main.c` lines 800, 808, 849, 891, 934, 945, 958 (and the matching `Data_Avr` / `Data_TrimmedMean` / `Data_Sorting` calls).

**Problem**:
```c
#define ADC_BUFFER_SIZE 8
uint16_t ADC_temp_data[ADC_BUFFER_SIZE];   // 8 elements x 2 = 16 bytes

ADC_GetDataWithPolling(ADC_temp_data, sizeof(ADC_temp_data));
//                                    ^^^^^^^^^^^^^^^^^^^^^^^
// Vendor signature: void ADC_GetDataWithPolling(uint16_t *adc_data, uint8_t count)
// count is the ELEMENT count: for (i=0; i<count; i++) adc_data[i] = ADCDR;
```

`sizeof(ADC_temp_data)` is **bytes (16)**, not elements (8). The vendor function therefore writes 16 uint16_t elements into an 8-element array - **16 extra bytes overrun adjacent memory** (stack frame or neighbouring globals on the small 8051 IRAM/XRAM).

The downstream `Data_TrimmedMean(ADC_temp_data, sizeof(ADC_temp_data))` then sorts and averages over those same 16 elements, computing a meaningless mean of half-real, half-corrupted data.

**Symptom**: hard-to-reproduce intermittent misbehaviour (random globals get clobbered every ADC cycle).

**Fix applied**: introduced an `ADC_BUFFER_COUNT` macro and replaced all 14 call sites:
```c
#define ADC_BUFFER_COUNT (sizeof(ADC_temp_data) / sizeof(ADC_temp_data[0]))

ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
V25_On_adc_data = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);
```

---

### C2. ISR-shared variables missing `volatile`

**Where**: `main.c`, `main.h`, `audio.c`.

**Problem**: variables read/written by the TIMER1 ISR (`audio.c`) and the main thread (`Audio_Initial`, `Read_Audio_Length`, `hw_initial`) had no `volatile`. Keil C51's optimiser is free to cache them in a register; ISR-side updates then become invisible to the main loop (and vice-versa).

**Fix applied**: `volatile` added to definitions in `main.c` and matching `extern` declarations in `main.h` for: `Audio_length`, `Audio_addr`, `Audio_start`, `Audio_start_address`, `Audio_max_length`, `SystemStatus`.

---

### C3. `Audio_max_length` is never initialised - alarm never plays

**Where**: `audio.c` line 166 defines `Read_Audio_Length()`; **nothing in the project ever calls it.**

**Effect**: BSS-initialised to `0`. In TIMER1_Int:
- first tick: `Audio_start == 0`, kick off SPI READ, set `Audio_start = 1`.
- second tick: `if (Audio_length < Audio_max_length)` -> `if (0 < 0)` -> false -> tears playback down on the very next sample.

**Result**: zero samples are ever fed to the PWM. The alarm clip is silent.

**Recommended fix** (not yet applied - flagged as next step):
```c
// New trigger function the alarm path should call
void Audio_Play_Start(uint16_t st_addr) {
    Audio_start  = 0;
    Audio_length = 0;
    Read_Audio_Length(st_addr);   // <-- the missing call
    Audio_addr   = st_addr;       // Read_Audio_Length leaves it at st_addr+41
    AUDIO_ON;
    Timer1_Start();
}
```

---

### C4. WDT was only used as a wake timer - no real watchdog

**Where**: `main.c` `BeforeStop()` (line 657, `WDTCR = 0xA2`), `WD_Reset()` (defined but never called).

**Problem**: `WDTCR = 0xA2` = `10100010` -> bit 6 (`WDTRSON`) = 0, i.e. **interrupt mode**. A timeout wakes the MCU; it does NOT reset it. A real watchdog reset never fires - a hang during active code is permanent. Unacceptable for a safety device.

`WD_Reset()` was already written with `WDTCR = 0xE5` (reset mode) but was never invoked.

**Fix applied**:
- `WD_Reset()`: extended timeout from `WDTDR=2` (~0.192 s) to `WDTDR=30` (~2 s) - long enough for the longest blocking call (`Delay_ms(300)` and ADC polling) but short enough to catch genuine hangs.
- Main loop top: call `WD_Reset()` on every iteration to arm/refresh the active-mode reset watchdog.
- After the LED + `hw_initial_Wait` sequence: call `WD_Reset()` again for a fresh 2 s budget.
- UART debug while(1): added `WD_Reset()` so debug mode is also protected.
- `Delay_s()` inner loop: added `WDTCR |= 0x20` to clear the counter every second.
- `BeforeStop()` is left alone: it intentionally reconfigures WDT into interrupt mode (`0xA2`) for the next sleep cycle wakeup. This is correct.

---

### C5. `SPI_Transfer` timeout sentinel collides with valid data

**Where**: `flash.c` lines 134-151.

```c
uint8_t SPI_Transfer(uint8_t value) {
    /* ... wait loop ... */
    if ((USTST & RXC) != RXC)
        return 0xFF;     // timeout sentinel - indistinguishable from a real 0xFF byte
    return USTDR;
}
```

A legitimate 0xFF data byte (e.g. the second byte of a JEDEC ID or arbitrary ADPCM byte) is reported as a timeout. The caller cannot tell.

**Recommendation** (not yet applied): change the signature to `int16_t` (negative = error) or set a separate `SPI_Timeout` flag global.

---

### C6. `hw_initial_Wait()` reinitialises every peripheral on every wake

**Where**: `main.c` main loop 327-355, `hw_initial()` 420-646.

**Problem**: `hw_initial_Wait(1)` is called inside the main loop every `adc_time` ticks. That single call reprograms the system clock, every GPIO, every timer, USART, ADC, WDT, LVR, opamps... If an audio playback is in flight, `hw_initial()` does have a guard (`if (Audio_start == 1) { tear down }`), but the race window with the ISR is still wide and the design is fragile.

**Recommendation** (not yet applied): split `hw_initial()` into per-peripheral helpers and call only what is actually needed at each point.

---

### C7. `Read()` leaves CS asserted; `Read_c()` has no SPI timeout guard

**Where**: `flash.c` lines 284-292, 295-299.

`Read(addr)` asserts CS, sends READ+address, and exits with CS still low so that the audio ISR can stream subsequent bytes via `Read_c()` (exploiting the flash auto-increment). Two issues:

1. If anything else accesses the SPI bus between `Read()` and the stream of `Read_c()` calls, the transaction silently corrupts.
2. `SPI_Receive()` has no timeout - a stuck SPI peripheral will hang the audio ISR forever.

**Recommendations** (not yet applied): add timeout guards to `SPI_Receive()`, and rename `Read()` to `Read_Stream_Start()` so the asymmetry is documented in the name.

---

## 2. High - fix soon

### H1. Empty for-loop "delays" that the compiler discards
`flash.c` 235, 308: `for (i=0; i<10; i++);` - the loop body is empty and an optimising compiler removes it entirely. Replace with `NOP_4us_Delay()` or explicit `_nop_()` expansions.

### H2. Side-effect macros without parentheses
`main.h` 8-21:
```c
#define LDO_ON LDOCR = 0x01    // expands assignment, not a function call
```
`if (LDO_ON) {...}` will assign 1 and always test true. Wrap each one:
```c
#define LDO_ON()  do { LDOCR = 0x01; } while(0)
```
(call-site change required.)

### H3. `Main()` capitalised - non-standard entry point
`void Main(void)` should be `void main(void)`. Works only because `startup.a51` happens to jump to this exact symbol; portable tooling and static analysers will not accept it.

### H4. `start_uart_debug_mode = Debug_On` in production
With this flag on and SW2 held at boot, the device enters an endless UART debug loop and never runs the fire-detection main loop. Set to `Debug_Off` for production builds (or wrap in `#ifdef DEBUG_BUILD`).

### H5. `VBGR_CV = 92` is a per-chip calibration constant
Comment already admits "chip-specific, calibrate!" - currently the same value is used for every chip. Move to a Flash/OTP calibration slot loaded at boot.

### H6. `volatile void` return types
`delay.c` declares functions `volatile void NOP_*_Delay(...)`. `volatile` on a void return is meaningless and silently ignored by the compiler. Drop it; if inlining must be prevented, do that via the assembly or a compiler pragma.

### H7. `int` (16-bit signed) used for ADC results
`Check_System` and `TEMP_ADC` store ADC results in `int` and call `abs()` on the difference. ADC range is 0..1023 so it works, but the type discipline is fragile - use `int16_t` / `uint16_t` explicitly.

### H8. `Audio_addr` is only `uint16_t` - 64 KB wrap risk
MX25L1006E is 128 KB. Current Audio3 ends at 0xE00C so we are safe, but any future clip past 64 KB will wrap. Promote `Audio_addr` and `AudioN_Address` to `unsigned long`.

### H9. `Uart_Out_Int` always prints 4 digits with leading zeros
`123` is sent as `"0123"`. Suppress leading zeros (or use a real format helper) if the output is meant to be human-readable.

### H10. `USART_SendDataWithPolling(&Visu_MODE, sizeof(Visu_MODE))`
Passing the address of an array as `uint8_t *`. Works by implicit cast but the type is `uint8_t (*)[10]`. Drop the `&` - array decays to pointer automatically.

---

## 3. Medium

| # | Where | Item |
|---|-------|------|
| M1 | all files | Korean comments were mojibake-corrupted - all source comments **converted to English** as part of this revision |
| M2 | main.c 1125 | `+ 48` -> `+ '0'` for ASCII conversion readability |
| M3 | main.h | no `#ifndef` include guard - **added during this revision** |
| M4 | main.c 94 | `[BUG-06 FIX]` comment block but the duplicate definition is now correctly resolved |
| M5 | main.c 1075+ | large block-commented `Set_Temp_Table` dead code - candidate for removal |
| M6 | main.c 169-171 | `adc_time`, `ck_led_time`, `Bat_al_time` are runtime variables but should be `const`/`code` |
| M7 | main.c 101-109 | Audio1/2/3 addresses and lengths are magic numbers - prefer a struct or symbolic constants |
| M8 | flash.c | `USTCR2 \|= DRIE` inside polled helpers enables DRE interrupt without a handler |
| M9 | src/ | `main.c.bak`, `main.h.bak` - move to `docs/backup/` or delete |
| M10 | main.c 169 | timing math (`8 * 5`, `56 * 5 + 12`) needs a `MS_PER_TICK` macro for clarity |

---

## 4. Low

| # | Item |
|---|------|
| L1 | use `uint8_t` instead of `int` for short loop counters on the 8-bit core |
| L2 | `Audio_start_address` declared but never used |
| L3 | `deviceID[3]` declared but never used |
| L4 | no CRC/checksum on the flash audio data (no corruption detection) |
| L5 | `EM_AL_Status`, `Bat_al_time` defined but their use is sparse |

---

## 5. What is currently applied vs pending

**Applied in this revision**:
- C1 (all 14 ADC call sites)
- C2 (volatile on all 6 ISR-shared variables, definitions and externs)
- C4 (active-mode WDT + WD_Reset() calls + Delay_s loop kick)
- main.h include guard
- All Korean comments (original mojibake + the C2/C4 fix notes) converted to English

**Pending**:
- C3, C5, C6, C7 (Critical) - still open
- All High and Medium items - open

---

## 6. Suggested next-step order

1. **C3** - add the missing `Read_Audio_Length()` call - **without this the device is silent during a fire**.
2. **C7** - add SPI timeout guards.
3. **C5** - SPI_Transfer error reporting.
4. **C6** - refactor `hw_initial()` into per-peripheral helpers.
5. Then the High items, then Medium/Low housekeeping.

---

*End of review v1.*
