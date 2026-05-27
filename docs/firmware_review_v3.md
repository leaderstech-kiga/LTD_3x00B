# LTD-3x00B 펌웨어 검토 보고서 v3

## 개요

v2 보고서 이후 적용된 추가 검토·수정 사항을 누적 정리한 최종 판본. v3 작업의 핵심은 **활성-모드 ADC/OPAMP 측정 안정화** + **Sleep 전류 88 µA → 2 µA 회복** + **dead-code 추가 정리** 입니다.

핵심 결과
- Sleep 전류: **~2 µA** (90 µA 회귀 해결, 최종 1.6 µA 측정)
- Dust 측정 정상 동작 (1AMP / 2AMP, ratiometric 배터리 보정 유지)
- Battery 전압 측정 (2.99 V 정확) — VBGR_CV = 92 데이터시트 일치
- LDO_ON / TEMP_ADC / Check_System 모두 정상 동작
- 음성 재생 + Al_Stop_mode ack 절차 모두 정상
- 활성 모드 WDT 2 s hang 보호 동작

---

## 1. Sleep 90 µA 회귀 — 근본 원인과 해결

### 1.1 증상
v2 cleanup 이후 sleep 전류가 2 µA → 90 µA 로 증가.

### 1.2 진단 절차
다단계 가설 검증:

| 단계 | 가설 | 결과 |
| --- | --- | --- |
| 1차 | WDTRC clock (WD_Reset 의 RESET 모드 클럭) | 효과 없음 |
| 2차 | LDO23 standby (P15 alternate function) | 효과 없음 |
| 3차 | 진단 BeforeStop — 9 줄 강제 OFF | **1.6 µA 회복** (방향 맞음) |
| 4차 | 9 줄 중 어느 줄이 결정타? — 한 줄씩 단독 ON | **AMPCR1 = 0x00 만이 핵심** |
| 5차 | 왜 OPAMP 가 sleep 중 켜져있나? | `#define DELAY_TIME` 누락 → `#else` 분기 → AMPCR1 = 0x88 (OPAMP ENABLED) |

### 1.3 근본 원인
v2 cleanup 시 main.h 의 `#define DELAY_TIME` 매크로를 "의미 불명" 으로 잘못 판단해 삭제. 그 결과 hw_initial 의

```c
#ifdef DELAY_TIME
    AMPCR1 = 0x00;       /* OPAMP OFF (의도된 분기) */
#else
    AMPCR1 = 0x88;       /* OPAMP ENABLED (잘못 컴파일됨) */
#endif
```

가 `#else` 분기를 선택 → OPAMP 가 항상 ON 상태로 boot → sleep 중에도 OPAMP analog block 동작 → 약 88 µA leakage.

### 1.4 영구 fix (3 곳)

**main.h** — `AMP_AUTO_DIS` 매크로 의미 명확화:
```c
#define AMP_AUTO_DIS  0   /* "Always" mode - keep AMPCR1 enable bits across
                           * ADC conversions. AMP_AUTO_DIS=1 ("Auto disable
                           * after ADC") breaks multi-shot OPAMP+ADC reads. */
```

**main.c hw_initial** — `#ifdef DELAY_TIME` 분기 제거하고 단일화:
```c
AMPCR1 = 0x00
    |(0<<7)   /* OP1 disable */
    |(0<<3)   /* OP0 disable */
    |(0<<4) | (0<<0);
```

**main.c BeforeStop** — defensive 보호망:
```c
AMPCR1 = 0x00;       /* OPAMP defensive disable before Stop entry */
LDOCR  = 0x00;       /* LDO defensive disable */
```

### 1.5 검증
- 1.6 ~ 2 µA sleep 전류 회복 ✓
- 음성 재생 / SW2 / EMR_IO 모두 정상 ✓

---

## 2. ADC 함수 추가 + 동작 검증

### 2.1 추가된 함수

| 함수 | 역할 |
| --- | --- |
| `Check_System()` | NTC vs 광 detector 변종 자동 감지 (boot 시 1회) |
| `TEMP_ADC()` | NTC 온도 측정 (LDO_REF + Temp_Table lookup) |
| `Get_Bat_Voltage_cV()` | 배터리 전압 측정 (VBGR ratiometric, 0.01 V 단위) |
| `Dust_ADC_1AMP()` | 1단 OP-amp 광량 측정 |
| `Dust_ADC_2AMP()` | 2단 OP-amp 광량 측정 (X15 / X30 gain) |
| `Data_TrimmedMean()` | outlier 1 개 제거 후 평균 (ADC 잡음 평활) |

### 2.2 적용된 fix 목록 (7+α 가지)

| # | 항목 | 영향 |
| --- | --- | --- |
| 1 | 로컬 `ADC_temp_data[8]` 제거 → 글로벌 사용 | 스택 16 byte 절감 |
| 2 | Underflow 가드 (`if (On > Off)`) | **가짜 화재 알람 차단** |
| 3 | `goto cleanup` 패턴 — 함수 종료 시 ADCCRL=0, AMPCR1=0, OP*_Enable(FALSE), P0FSRL 복귀 | 활성 모드 전류 ↓ + 다음 함수 의존성 ↓ |
| 4 | OP1 stabilization 200 µs (2AMP) | 데이터시트 권장 준수, 첫 측정 정확도 ↑ |
| 5 | VBGR 측정에만 `adc_avg = 0` 가드 유지 | LED ON/OFF 정상 흐름 보장 |
| 6 | `P0FSRL` 함수 진입 시 명시 설정 (1AMP/2AMP 양쪽) | 호출 순서 의존성 제거 |
| 7 | 한글 mojibake → 영문 주석 | ASCII-only 정책 회복 |
| 8 | `ADC_BUFFER_COUNT` 매크로 직접 정의 (sizeof 의존성 제거) | Keil C51 매크로 컨텍스트 안전성 |
| 9 | `ADC_CLK_4M` 인자 6 곳 → `ADC_CLK_2M` 통일 | 코드 의도와 실제 동작 일치 |
| 10 | `Set_Temp_Table` 주석 dead code (~100 줄) 제거 | 코드 청결 |

### 2.3 LED ON/OFF 가드 버그 수정

`adc_avg=0` 가드를 LED ON/OFF 측정에도 적용했더니 chamber 어두울 때 정상 흐름 차단되는 버그 발견 (`Si_Da=0` 출력). 해결:
- LED ON/OFF 단계에선 나눗셈이 1024 로만 일어남 (divide-by-zero 무관)
- adc_avg=0 도 정상 측정값으로 받아들여 처리
- VBGR 측정 (`op /= adc_avg`) 에서만 가드 유지

---

## 3. LDO_ON 동작 불가 회귀 해결

### 3.1 원인
Sleep 90 µA 디버깅 중 `P1FSRH` 의 P15 비트를 LDO23(1) → I/O(0) 으로 바꿔놓았던 코드가 잔존. 결과:
- `LDO_ON` 매크로는 LDOCR 모듈 자체는 enable 하지만
- P15 핀이 I/O 모드라 LDO 출력이 외부로 안 나옴
- `ADC_LDO_REF` 모드에서 AVREF floating → 측정 부정확

### 3.2 해결
hw_initial 의 P15 비트를 LDO23 alternate function (1) 으로 복원:
```c
P1FSRH = (1 << 2);   /* P15 = LDO23 출력 */
LDOCR  = 0x00;       /* boot 직후 LDO 는 OFF, LDO_ON 호출 시에만 켬 */
```

이전 진단으로 P15 = LDO23 + LDOCR = 0 이어도 sleep 누설 없음 확인됐으므로 안전.

---

## 4. Battery 전압 측정 검증

### 4.1 데이터시트 매칭
사용자 측정: VBAT 실제 2.99 V, `Get_Bat_Voltage_cV()` 결과 정확히 299.

| 항목 | 코드 | 데이터시트 | 일치 |
| --- | --- | --- | --- |
| VBGR_CV | 92u (0.92 V) | 0.92 V ±3 % @ Ta=25°C | ✓ |
| ADC FS | 1024 | 10-bit | ✓ |
| INTERNAL_REF | VDD | VDD or AVREF (REFSEL select) | ✓ |
| Ratiometric 수식 | Vbat = VBGR × 1024 / adc | block diagram 과 일치 | ✓ |

VBGR_CV 별도 캘리브레이션 불필요.

---

## 5. Al_Stop_mode WDT-RESET 위험 차단

### 5.1 문제
사용자 ack 단계 (8 × 40 s wait) 가 활성 모드 WDT (2 s) 보다 훨씬 김. 중간에 WD_Reset 호출 없으면 reset 됨.

### 5.2 해결
5 곳에 `WD_Reset()` 추가:
- 첫 번째 SW2 release polling `while(1)`
- 400 ms 내부 `for(j=0;j<4;j++)`
- **39.4 s 내부 `for(j=0;j<394;j++)`** (가장 중요)
- 1 s LED 펄스 사이
- 두 번째 SW2 release polling `while(1)`

매 100 ms 마다 dog kick → 활성 WDT 2 s 한도의 5 % 사용 → 충분한 마진.

---

## 6. SPI / Audio / 기타 hardening

### 6.1 SPI_Memory_Check 3회 retry
일시적 flash POR / EMI 글리치 시 음성 알람 영구 disable 방지. 3회 모두 실패 시 LED_R 3회 깜박임으로 시각 알림.

### 6.2 Audio_Run stall detector
ADPCM 디코더가 멈춰 `Audio_length` 가 증가하지 않으면 200 ms 후 break. WD_Reset 만 부르며 무한 spin 하는 livelock 차단.

### 6.3 bitwise `&` → logical `&&`
main loop 의 두 if 문에서 비트 AND 를 논리 AND 로 교체. short-circuit 평가 회복.

### 6.4 ADC bounded wait
`ADC_GetDataWithPolling` 에 `ADC_Timeout` 플래그 + bounded wait 추가. AVREF floating 등 ADC fault 시 hang 차단.

---

## 7. Dead-code 추가 정리

### 7.1 미사용 함수 제거 (v4 cleanup 시)

| 파일 | 제거 함수 |
| --- | --- |
| flash.c | `Read_v`, `Read_`, `FastRead`, `SPI_Send` |
| flash.h 매크로 | `COMMAND_WREN/WRDI/RDSR/WRSR/FREAD/WRITE/SE/BE/CE`, `STATUS_*`, `MAX_BUFF_SIZE` |
| a96l41x_adc.c | `ADC_Enable`, `ADC_GetDataWithInterrupt`, `ADC_ConfigureInterrupt`, `ADC_ClearInterruptStatus`, `ADC_Int_Handler`, 3개 ISR globals |
| a96l41x_opamp.c | `OPAMP_Initial`, `OP0_Enable`, `OP1_Enable` (나중에 dust ADC 재도입으로 복원) |
| a96l41x_clock.c | `Clock_Initial`, `Clock_ConfigureOSC` |
| a96l41x_usart.c | `USART_Clock_Initial`, `USART_Enable`, interrupt/queue 변형 9 개 |

### 7.2 모든 `(FIX X)` 마커 주석 제거
14 파일 89 곳의 fix 마커와 ORIG 라인 모두 제거. 코드는 그대로 유지하면서 자동화 도구가 보기 깨끗한 형태로.

---

## 8. 최종 상태 검증

### 8.1 동작 확인 항목

| 항목 | 상태 |
| --- | --- |
| Sleep 전류 | ~ 2 µA ✓ |
| Active mode WDT 2 s hang 보호 | 동작 ✓ |
| SW2 fire alarm 수동 트리거 | 정상 ✓ |
| EMR_IO 외부 트리거 | 정상 ✓ |
| Al_Stop_mode ack 절차 (40 s × 8) | reset 없이 완주 ✓ |
| Play_Clip (BOOT / FIRE) 음성 재생 | 정상 ✓ |
| `Get_Bat_Voltage_cV` 정확도 | 2.99 V @ VBAT=3.0V ✓ |
| `Check_System` heat / smoke 판별 | 정상 ✓ |
| `TEMP_ADC` NTC 측정 + lookup | 정상 ✓ |
| `Dust_ADC_1AMP/2AMP` 광 측정 | 정상 (chamber 비었을 때 작은 값, 의도된 동작) ✓ |
| 한글 mojibake | 0 byte ✓ |
| 빌드 경고 (L16 등) | 정리 ✓ |

### 8.2 10 년 동작 마진 (최종)

```
sleep 전류 : 2 µA
active 평균 : ~22 µA (사이클당 ~2 ms)
sensor / Bat_Ck : 영향 미미 (56 s, 7.7 s 주기)
음성 알람 : 일주일 1회 가정 시 무시

평균 ≈ 3 µA
CR17450 × 2 = 4800 mAh
지속 시간 = 4800 / 3 ≈ 1.6 M 시간 ≈ 180 년 (이론치)
실제 10 년 spec 마진 약 15 배
```

---

## 9. 권장 후속 작업

### Calibration 단계

- **VBGR_CV calibration** — chip 별 ±3 % VBGR 편차 보정 (필요 시)
- **dust 임계값 결정** — chamber 에 연기/먼지 주입 시 ADC1_Dust_Val / ADC2_Dust_Val 측정해 alarm 트리거 임계값 정함 (max ~299 범위)
- **NTC Temp_Table 검증** — 보정된 thermistor 와 비교

### 회로 검증

- LDO23 의 standby current 실측 (P15 = LDO23 alternate function 상태에서)
- chamber 광량 ↔ ADC 응답 곡선

### 향후 코드 개선

- main loop 의 빈 `if (Dust_mode)` / `if (Temp_mode)` 분기에 측정 코드 통합
- Dust_ADC_1AMP / 2AMP 결과를 임계값과 비교해 SYS_mode 전환 트리거 작성
- 첫 번째 시험용 `while(1)` 은 이미 제거됐고, 두 번째 (실 동작) `while(1)` 만 active

---

## 10. 변경 파일 요약

| 파일 | 변경 분량 |
| --- | --- |
| src/main.c | 새 ADC 함수 5 개 + 기존 함수 cleanup + sleep fix |
| src/main.h | AMP_AUTO_DIS = 0, 미사용 매크로/프로토타입 정리 |
| src/flash.c, flash.h | dead code 제거, SPI timeout 가드 |
| src/audio.c, audio.h | Read_Audio_Length 제거 |
| src/a96l41x_adc.c, .h | interrupt 경로 제거, ADC_Timeout 추가 |
| src/a96l41x_opamp.c, .h | unused 함수 제거 → 후에 일부 복원 |
| src/a96l41x_clock.c, .h | unused 함수 제거 |
| src/a96l41x_usart.c, .h | unused 함수 제거 |
| docs/ | adc_review.md, main_flow_v3.md, wdt_periodic_v3.md, firmware_review_v3.md (이 파일) |

---

## 11. 변경 이력

| 단계 | 핵심 작업 | 결과 |
| --- | --- | --- |
| v1 | 초기 코드 검토 + Critical 5건 / High 6건 / Medium 5건 식별 | v1 보고서 |
| v2 | C1~M9 / L1~L5 fix 적용 + 8-sec wake 아키텍처 + 음성 글리치 (C14 10µF) | v2 보고서 |
| v3 (이 보고서) | ADC 함수 추가 + Sleep 90µA 회귀 해결 + LDO_ON 회복 + dead-code 추가 정리 | 안정화 완료 |

작업 기간 마무리. 빌드 + 동작 검증 모두 정상.
