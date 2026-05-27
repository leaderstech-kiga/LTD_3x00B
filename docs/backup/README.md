# Backup files

Archive of historical source files moved out of the active `src/` tree.
These files are NOT compiled and not referenced by the Keil .uvproj.
Kept for reference / quick rollback only — for full history use git.

## main.c.bak (77 KB, 2025-01-22)

Pre-cleanup snapshot of `main.c` from an earlier development state.
Roughly the same era as `자료/Stand_along_240628/src/main.c.bak`,
captured before the 2026 firmware review and the C2/C4/C5/C7/H1-H10/M1-M5
fixes were applied.

Useful for:
- Quickly viewing the original Korean comments before mojibake corruption
- Comparing function signatures or variable layouts with the current code
- Rolling back a specific function if a regression is found

Do NOT add this file back to the Keil project. If you need to restore
a specific function, copy only the relevant portion into the active
`src/main.c` and re-apply the cleanup patterns (volatile, FIX markers,
ASCII-only comments).

## Other backup locations (not in this folder, FYI)

- `src_b/main.c.bak`, `src_b/main.h.bak` - parallel directory snapshot
  (project-level structure, kept by the developer separately).
- `자료/Stand_along_240628/` - full earlier project tree, working audio.
- `자료/Stand_along_Abov/` - vendor / reference variant.

These are managed independently and were not touched by the M9 fix.

## (FIX M9)

This folder was created as part of fix M9 of the firmware review:
move stale backup files out of the source directory so `src/` only
contains files that the build actually uses.
