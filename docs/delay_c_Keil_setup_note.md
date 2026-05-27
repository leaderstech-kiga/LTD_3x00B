# delay.c Keil compile-setup note

> Original filename: `delay.c error.txt`
> This is a **fix procedure record**, not an active error. The required setting is already applied in `LTD_3x00B.uvproj`.

## Background

`src/delay.c` contains the `NOP_20us_Delay()` routine which uses `#pragma asm` / `#pragma endasm` inline assembly to count instruction cycles for precise polling delays at 4 MHz MCLK. The Keil C51 compiler needs a per-file option to handle inline assembly.

## Keil uVision GUI steps

1. Right-click `delay.c` in the Project window.
2. Choose **Options for File...**
3. Open the **Properties** tab.
4. Tick **Generate Assembler SRC File**.
5. Tick **Assemble SRC File** (clear it once then re-tick if it appears greyed out).
6. Click OK.

## Equivalent XML in .uvproj (for reference)

```xml
<FileName>delay.c</FileName>
<FilePath>.\src\delay.c</FilePath>
<FileOption>
  <CommonProperty>
    ...
    <GenerateAssemblyFile>1</GenerateAssemblyFile>   <!-- 1 = ticked -->
    <AssembleAssemblyFile>1</AssembleAssemblyFile>   <!-- 1 = ticked -->
    ...
```

| XML value | Meaning |
|-----------|---------|
| `1` | Option ticked (enabled) |
| `2` | Option unticked (disabled / default) |

## Symptoms if missing

- `#pragma asm` blocks are silently ignored - `NOP_20us_Delay()` returns immediately without delay.
- Or `ERROR L104: MULTIPLE PUBLIC DEFINITIONS` at link time.
- The A51 assembler library (C51S.LIB etc.) is not auto-linked.

## Notes

- Setting lives in the .uvproj file -> tracked by git -> persists across machines.
- Any future file that contains `#pragma asm` needs the same option enabled.
