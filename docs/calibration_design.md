# LTD-3x00B Per-Unit Calibration Design

> 작성일: 2026-05-27
> 대상: A96L416 펌웨어 (Keil C51), Dust_ADC_2AMP 광전식 연기 감지
> 적용 OPAMP 설정: `OPAMP_Initial(GAIN1_X30, GAIN0_DIS, chp_125KHz)`

## 1. 배경 - 시료별 편차

7개 시료를 동일 챔버에서 측정한 Si_Da (= ADC2_On_Dust_Val - ADC2_Off_Dust_Val) 값:

| 시료 | 0% 연기 (offset) | 12% 연기 | Δ |
|------|-----------------:|---------:|---:|
| 1 | 19  | 190 | 171 |
| 2 | 75  | 250 | 175 |
| 3 | 10  | 100 |  90 |
| 4 | 44  | 240 | 196 |
| 5 |  8  | 100 |  92 |
| 6 |  6  | 100 |  94 |
| 7 | 15  | 120 | 105 |

관찰:

- **Offset 편차 12배** (6 ~ 75) — 챔버 stray light, dark current, OPAMP DC offset 시료별 차이
- **응답(Δ) 편차 2.2배** (90 ~ 196) — LED 광량, 포토다이오드 감도, 광학 정렬 시료별 차이
- **offset과 gain은 약하게 상관**되어 있어 둘 모두 독립 보정이 필요

단일 고정 threshold로는 시료 3/5/6은 둔감하고 시료 2는 false alarm 위험.

## 2. 보정 모델

```
baseline   = Si_Da @ 0%             (시료별 측정)
delta_meas = Si_Da @ 12% - baseline (시료별 측정)
gain_Q8    = (CALIB_REF_DELTA << 8) / delta_meas
```

런타임:

```
if (Si_Da_raw <= baseline) Si_Da_norm = 0;
else                       Si_Da_norm = ((Si_Da_raw - baseline) * gain_Q8) >> 8;
```

상수:

| 상수 | 값 | 의미 |
|------|---:|------|
| `CALIB_REF_DELTA` | 150 | 12% 연기에서 모든 시료가 보고할 표준화 값 |
| `CALIB_ALARM_THRESHOLD` | 100 | Si_Da_norm > 이 값이면 화재 카운터 증가 |
| `FIRE_TRIGGER_COUNT` | 3 | 3회 연속 임계 초과 시 In_Fire_al_mode 진입 (~23s) |
| `CALIB_MIN_DELTA` | 30 | 12% 측정 - 0% 측정 < 30이면 calibration 거부 |
| `CALIB_SAMPLES` | 4 | 각 단계에서 4회 측정 평균 |

## 3. 보정 결과 시뮬레이션 (정수 산술)

`((raw - baseline) * gain_Q8) >> 8` 를 7개 시료에 적용:

| 시료 | gain_Q8 | 0% Norm | 12% Norm | 12% Trigger | 8% Norm (추정) | 8% Trigger |
|------|--------:|--------:|---------:|:-----------:|---------------:|:----------:|
| 1 | 224 | 0 | 149 | ✓ | 99 | ✗ |
| 2 | 219 | 0 | 149 | ✓ | 99 | ✗ |
| 3 | 426 | 0 | 149 | ✓ | 99 | ✗ |
| 4 | 195 | 0 | 149 | ✓ | 99 | ✗ |
| 5 | 417 | 0 | 149 | ✓ | 99 | ✗ |
| 6 | 408 | 0 | 149 | ✓ | 98 | ✗ |
| 7 | 365 | 0 | 149 | ✓ | 99 | ✗ |

핵심:

- 모든 시료가 12% 연기에서 **149**로 수렴 (목표 150 대비 -1 LSB는 정수 나눗셈 오차)
- 모든 시료가 0% 연기에서 정확히 **0**으로 출력 (clamp)
- **12%에서 7/7 정상 알람**
- 8% 연기 (linear interpolation 가정) Norm = 98~99 → 100 임계에 1 LSB 부족, 트리거 안 됨

### Threshold 튜닝 권고

선형 보간 기준으로 현재 설정은 **약 8.1% 연기에서 트리거**됩니다. UL/KFI 표준이 일반적으로 더 낮은 농도(~4%)에서 트리거를 요구한다면:

- `CALIB_ALARM_THRESHOLD`를 50~80으로 낮추거나
- `CALIB_REF_DELTA`를 200~250으로 높여 분해능 확장

다만 실제 연기 산란 응답은 비선형이므로 위 값은 추정치입니다. 실측 후 최종 결정 권장.

## 4. 저장 (A96L416 DataFlash)

- 위치: 0x3000 (256B 영역의 첫 32바이트 페이지)
- 구조 (8바이트, 나머지 24바이트는 0xFF):

```
offset 0..1 : magic   (0xCA1B)
offset 2..3 : baseline (uint16_t LE)
offset 4..5 : gain_Q8  (uint16_t LE)
offset 6..7 : checksum (magic XOR baseline XOR gain_Q8)
```

- 무효 / 미보정 시: 자동으로 `baseline=0, gain_Q8=256` 기본값 사용 → 보정 없는 raw 값과 동일

## 5. Calibration 모드 진입 / 절차

```
부팅 (POR) + SW2 누른 채   →   진입 감지: 양 LED 빠르게 8회 점멸
                            →   SW2 떼기 대기 (LED_G 점등)
Step 1: 0% 연기 측정
   LED_G 슬로우 블링크 → 시료를 깨끗한 공기에 두고 SW2 짧게 누름
   양 LED 빠르게 4회 점멸 → 4회 측정 평균 = baseline
Step 2: 12% 연기 측정
   LED_R 슬로우 블링크 → 시료에 12% 표준 연기 주입 후 SW2 짧게 누름
   양 LED 빠르게 4회 점멸 → 4회 측정 평균 = smoke
저장:
   delta = smoke - baseline
   delta < 30 → LED_R 빠른 블링크 5초 = 측정 거부 (광학/주입 문제)
   delta ≥ 30 → gain_Q8 계산 + DataFlash erase + page write + verify
                성공 → LED_G 점등 3초
                실패 → LED_R 느린 블링크 5회
대기 루프:
   전원 재공급 (SW2 안 누르고) → Normal mode + 새 보정값 적용
```

## 6. 코드 변경 요약

| 파일 | 변경 |
|------|------|
| `src/a96l41x_dataflash.h/c` | 신규 (vendor sample → 프로젝트 내) |
| `src/calib.h/c` | 신규 — load/save/apply/run_procedure |
| `src/main.c` | (a) `#include "calib.h"` 추가 <br>(b) 부팅 시 `calib_load()` + SW2 검사 + `calib_run_procedure()` <br>(c) `ADC2_Dust_Val_Norm` 전역 + `fire_count` 추가 <br>(d) 메인 루프 ADC 블록에 dust 측정 + 정규화 + 화재 카운터 <br>(e) UART 디버그 출력에 Norm/Base/Gain/Cv 추가 |

## 7. Keil 프로젝트 등록

다음 두 파일을 `A96L41x_Sample.uvproj`의 소스 그룹에 추가해야 합니다:

- `src/a96l41x_dataflash.c`
- `src/calib.c`

uVision 에서 *Project → Manage Project Items → Source Group* 에 추가하거나, .uvproj XML을 직접 편집할 수 있습니다.

## 8. 검증 체크리스트

- [ ] DataFlash가 erase 상태일 때 부팅 → `calib_load()` 반환 0, `g_calib_valid==0` 확인 (UART Cv:0)
- [ ] SW2 누른 채 부팅 → 양 LED 빠른 점멸 진입 표시 확인
- [ ] 2단계 측정 후 LED_G 3초 점등 확인 (저장 성공)
- [ ] 전원 재인가 → `g_calib_valid==1`, UART Norm: 값이 ~0 (clean air)
- [ ] 12% 연기 주입 → Norm: 값이 ~150 부근, fire_count 증가, 3회 연속 시 알람
- [ ] DataFlash 검증: 7개 시료 모두에 대해 보정 후 12% Norm 145~155 범위
