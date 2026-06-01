# LTD-3x00B Per-Unit Calibration Design (rev 4)

> 작성일: 2026-05-30 (rev 4 — 부팅 절차 재정의)
> 대상: A96L416 펌웨어 (Keil C51), Dust_ADC_2AMP 광전식 연기 감지
> 적용 OPAMP 설정: `OPAMP_Initial(GAIN1_X30, GAIN0_DIS, chp_125KHz)`
> 변경 요지: STEP 1(부트 LED) 추가, erase 경로에서 baseline 자동 캡처(STEP 2-1),
> SW2 미홀드 시 단순 load(STEP 2-2), STEP 3 트리거를 short-press → **SW2 hold** 로 변경,
> `delta_min` 기본값 30 → **50**.
> 메인 루프 세부 동작(화재 감시·화재 음성 등)은 별도 정의 예정 (본 문서 STEP 3의 TODO 참조).

## 1. 배경 - 시료별 편차

7개 시료를 동일 챔버에서 측정한 Si_Da 편차 — 자세한 데이터는 v2 문서 참조. 요약하면:

- **Offset 편차 12배** (clean air 6 ~ 75) → baseline 보정 필요
- **응답 편차 2.2배** (delta 90 ~ 196) → gain 보정 필요

→ baseline + gain 둘 다 시료별 독립 저장.

## 2. 부팅 / 캘리브레이션 절차

### STEP 1 — 부팅 (전원 인가)

```
전원 인가
   ↓
LED_G, LED_R 동시 0.2초 점등 → 소등
   ↓
SW2 상태 판별로 분기 (Delay_ms(300) 후 P12 읽기)
```

함수: `calib_step1_boot_led()`

### STEP 2-1 — Flash Erase + Baseline 캡처 (SW2 누른 채 부팅)

```
부팅 후 SW2 hold 감지
   ↓
SW2 hold 유지 중: LED_G ↔ LED_R 0.2초 단위 교차 점등
   ↓
SW2 release 감지
   ↓
DataFlash 0x3000 page erase (DFMBUSY 완료 대기)
   ↓
RAM globals 기본값 리셋 (baseline=0, gain=256, valid=0)
   ↓
Dust_ADC_2AMP() 1회 측정 (내부 8회 평균) → baseline
   * release 순간이 clean air 여야 함 (사용자 책임)
   ↓
g_calib_needs_smoke = 1
calib_save(baseline, gain_Q8=256) → DataFlash 저장
   ├─ 성공 → LED_G 0.2초 단위 5회 점멸
   └─ 실패 → LED_R 0.2초 단위 5회 점멸
```

함수: `calib_erase_with_animation()`

> rev 3 대비: baseline 캡처가 "별도의 버튼 1회 누름 대기" 에서 **erase 직후 자동 1회 측정**으로 바뀜.
> SW2를 떼는 순간의 clean-air 값이 baseline 으로 확정된다.

### STEP 2-2 — Flash Read (SW2 안 누른 채 부팅)

```
calib_load() → DataFlash record 읽기
   ├─ 유효 (magic=0xCA1B + checksum OK) → baseline, gain_Q8 갱신, UART "Flash_Read_OK"
   └─ erase/corrupt → 기본값 유지(baseline=0, gain=256), UART "Flash_Read_Fail"
   ↓
g_calib_needs_smoke = (g_calib_gain_Q8 == 256) ? 1 : 0
```

함수: `calib_boot_init()`

### STEP 3 — Smoke Gain 보정 (메인 루프, SW2 hold)

```
메인 루프에서 매 wake(0.192초)마다 검사:
   if (SYS_mode == Normal_mode && SW2 눌림) {
      ├─ g_calib_needs_smoke == 0 → SW2 release 대기 없이 화재발생 음성 1회
      │                              (※ TODO - 메인 루프 정의 시 구현)
      └─ g_calib_needs_smoke == 1 → calib_run_smoke() 진입
   }

calib_run_smoke():
   LED_R 점등 → SW2 release 대기
      ↓
   LED_R 0.2초 간격 깜박 5초 (사용자가 시료를 연기 발생기 안에 둘 시간)
      ↓
   LED_R 0.5초 단위 깜박 + 5회 측정 (총 ~5초)
      ↓
   smoke_avg = 5회 평균
      ↓
   delta = smoke_avg - baseline
      ├─ delta < delta_min(50) → LED_R 0.2초 빠른 깜박 5초
      │                          → g_calib_needs_smoke = 1 유지 (보정 실패, 재시도 가능)
      └─ delta ≥ delta_min(50) → gain_Q8 = (200 << 8) / delta
                                → calib_save(baseline, gain_Q8)
                                   ├─ 성공 → LED_G 0.2초 빠른 깜박 1초 → g_calib_needs_smoke = 0
                                   └─ 실패 → LED_G 0.2초 빠른 깜박 1초 → g_calib_needs_smoke = 1
      ↓
   (※ TODO) 화재 감시 실시 후 메인 루프로 복귀
```

함수: `calib_run_smoke()`

> 메인 루프의 세부 동작(ADC 사이클, 화재 감시, 화재 음성 등)은 추후 별도 정의.
> 현재 STEP 3 함수는 보정 시퀀스까지 구현, 화재 음성/감시는 TODO 주석으로 표시.

### 상수

| 매크로 | 값 | 의미 |
|------|---:|------|
| `CALIB_REF_DELTA`           | 200 | Step 3에서 normalize되는 표준 값 |
| `CALIB_ALARM_THRESHOLD`     | **160** | Si_Da_norm > 이 값 → 화재 카운터 증가 (REF의 80%) |
| `FIRE_TRIGGER_COUNT`        | 3 | 3회 연속 임계 초과 시 In_Fire_al_mode |
| `CALIB_MIN_DELTA`           | **50** | Step 3 거부 임계 (delta_min). rev3는 30 |
| `CALIB_BOOT_LED_MS`         | 200 | STEP 1 양 LED 점등 시간 |
| `CALIB_ANIM_MS`             | 200 | STEP 2-1 G/R 교차 half-period |
| `CALIB_BLINK_FAST_MS`       | 100 | 0.2초 깜박 half-period (성공/실패/거부 표시) |
| `CALIB_RESULT_BLINKS`       | 5 | STEP 2-1 결과 점멸 횟수 |
| `CALIB_SMOKE_PLACE_MS`      | 5000 | STEP 3 "시료 투입" 대기 (LED_R 0.2초 깜박) |
| `CALIB_SMOKE_PLACE_BLINK_MS`| 100 | place 단계 깜박 half-period |
| `CALIB_SMOKE_SAMPLES`       | 5 | STEP 3 평균 샘플 수 |
| `CALIB_SMOKE_BLINK_MS`      | 500 | STEP 3 측정 중 LED_R 깜박 half-period |
| `CALIB_FAIL_BLINK_MS`       | 5000 | 거부 표시 총 시간 |
| `CALIB_OK_BLINK_MS`         | 1000 | 성공 표시 총 시간 |

## 3. 보정 결과 시뮬레이션 (REF_DELTA=200, threshold=160, delta_min=50)

7개 시료의 12% 연기 raw 값을 적용 (raw = baseline + delta):

| 시료 | baseline | 12% raw | delta | 판정 | gain_Q8 | 12% norm | 9.6% norm | 알람 @ 9.6% |
|------|---:|---:|---:|:-:|---:|---:|---:|:-:|
| 1 | 19 | 190 | 171 | 통과 | 299 | 199 | 160 | ✓ |
| 2 | 75 | 250 | 175 | 통과 | 292 | 199 | 159 | ✗ |
| 3 | 10 | 100 |  90 | 통과 | 568 | 199 | 159 | ✗ |
| 4 | 44 | 240 | 196 | 통과 | 261 | 199 | 160 | ✓ |
| 5 |  8 | 100 |  92 | 통과 | 556 | 199 | 160 | ✓ |
| 6 |  6 | 100 |  94 | 통과 | 544 | 199 | 159 | ✗ |
| 7 | 15 | 120 | 105 | 통과 | 487 | 199 | 159 | ✗ |

- **보정 성공 7/7** (최소 delta = 90 ≥ delta_min 50)
- 12% (보정 기준) → Norm ≈ 199, 알람 ✓
- 9.6% (= 80% × 12%) → Norm ≈ 159, 알람 직전 (160 미만)
- ~9.65% 이상에서 알람 트리거

> **delta_min 선택 근거:** 본 시료군의 delta 최소값은 90. delta_min=100 으로 하면 시료 3·5·6 (delta 90/92/94)이 거부되어 4/7만 통과한다. delta_min=50 은 7/7 통과를 보장하면서, 실제 연기 미주입(delta가 한 자릿수~수십)과 정상 보정(delta ≥ 90)을 충분히 구분한다.

## 4. DataFlash 저장 구조

위치: 0x3000 (256B DataFlash 영역의 첫 32B 페이지)

```
offset 0..1 : magic    (0xCA1B)
offset 2..3 : baseline (uint16_t LE)
offset 4..5 : gain_Q8  (uint16_t LE)
offset 6..7 : checksum (magic XOR baseline XOR gain_Q8)
offset 8..1F: 0xFF × 24 (여유)
```

- erase 상태: magic = 0xFFFF → calib_load 무효 → 기본값 사용
- baseline만 캡처된 상태: gain_Q8 = 256 → calib_load 유효이지만 g_calib_needs_smoke = 1
- 완전 보정된 상태: gain_Q8 ≠ 256 → g_calib_needs_smoke = 0

> **읽기 주의 (rev4 버그 fix):** DataFlash(0x3000~0x30FF)는 프로그램 플래시와 별개 영역으로,
> UM 4.4에 따라 **DPTR(XDATA, MOVX) 접근**으로 읽어야 한다. `calib_read_word()`는 `xdata` 포인터 사용.
> `code` 포인터(MOVC)로 읽으면 빈 코드공간(0x00)을 읽어 항상 0이 나온다.
> 또한 erase/write 후 **DFMBUSY 완료 대기**(`calib_wait_df_ready()`) 후 read-back 한다.

## 5. LED 상태표

| 단계 | 트리거 | LED_G | LED_R | 사용자 의미 |
|------|--------|:-:|:-:|------|
| **Step 1** 부팅 | 전원 인가 | 0.2초 ON | 0.2초 ON | "전원 인가 / 부팅" |
| **Step 2-1** erase 중 | SW2 부팅 시 hold | 0.2초 교차 | 0.2초 교차 | "Flash erase 대기 (떼면 baseline 캡처)" |
| Step 2-1 성공 | baseline 저장 OK | 0.2초 5회 점멸 | OFF | "baseline 저장 완료" |
| Step 2-1 실패 | baseline 저장 fail | OFF | 0.2초 5회 점멸 | "DataFlash 저장 오류" |
| **Step 3** 시료 투입 | SW2 hold + needs_smoke | OFF | 0.2초 깜박 5초 | "시료를 연기 발생기에 두세요" |
| Step 3 측정 | 자동 | OFF | 0.5초 깜박 | "측정 중 (5회)" |
| Step 3 성공 | 자동 | 0.2초 깜박 1초 | OFF | "gain 저장 완료" |
| Step 3 거부 | delta < 50 | OFF | 0.2초 깜박 5초 | "연기 미주입/광학 이상 (재시도)" |

## 6. UART 출력 (uart_debug_On일 때만)

메인 루프 ADC 측정 직후 출력. 양산 모드(`uart_debug_mode = Debug_Off`)에서는 아예 호출 안 됨 → 전류 소모 없음.

출력 예 (개행 포함):
```
Visu_MODE: Bat:0299 Si_Off:0000 Si_On:0000 Si_Da:0000 Du_Off:00xx Du_On:00xx Si_Da:0190 Norm:0199 Base:0019 Gain:0299 Cv:0001
```

- `Cv:0001` = DataFlash 유효 record load 성공
- `Norm > 160` → 화재 카운터 증가, 3회 연속 시 알람

## 7. 코드 변경 요약 (rev 4)

| 파일 | 변경 |
|------|------|
| `src/a96l41x_dataflash.h/c` | vendor sample 복사 (변경 없음) |
| `src/calib.c` | (a) `calib_read_word` `code`→`xdata` 수정 <br>(b) `calib_wait_df_ready()` (DFMBUSY 폴링) 추가 <br>(c) `calib_step1_boot_led()` 신규 (STEP 1) <br>(d) `calib_erase_with_animation()` = STEP 2-1 (교차 점등 + erase + baseline 캡처 + save) <br>(e) `calib_boot_init()` = STEP 2-2 (단순 load) <br>(f) `calib_run_smoke_short()` → `calib_run_smoke()` 재작성 (hold 기반, place 5초, delta_min=50, 실패 시 flag 유지) |
| `src/calib.h` | 상수 재정의 (delta_min=50, LED 타이밍), 프로토타입 갱신 |
| `src/main.c` | (a) 부팅: STEP 1 호출 → SW2 hold 시 2-1, 미홀드 시 2-2 분기 <br>(b) STEP 3 트리거를 short-press → SW2 hold 로 변경, needs_smoke==0 시 화재음성(TODO) <br>(c) DataFlash 디버그 while 루프 주석 처리 |
| `docs/calibration_design.md` | 본 문서 (rev 4) |

## 8. Keil 프로젝트 등록

다음 파일이 `.uvproj` 소스 그룹에 등록되어 있어야 함:

- `src/a96l41x_dataflash.c`
- `src/calib.c`

## 9. 검증 체크리스트

### Step 1 — 부팅
- [ ] 전원 인가 → LED_G/R 동시 0.2초 점등 후 소등 확인

### Step 2-1 — Flash Erase + Baseline 캡처
- [ ] SW2 누른 채 부팅 → LED_G/R 0.2초 교차 점등 확인
- [ ] (clean air 상태에서) SW2 release → erase + baseline 측정
- [ ] LED_G 5회 점멸(성공) 확인 (실패 시 LED_R 5회)
- [ ] 재부팅(SW2 안 누름) → UART `Cv:0001 Base:<측정값> Gain:0256 Norm:0000`

### Step 2-2 — Flash Read
- [ ] SW2 안 누른 채 부팅 → 저장된 baseline/gain load, UART `Flash_Read_OK`

### Step 3 — Smoke Gain 보정
- [ ] needs_smoke=1 상태, Normal 모드에서 SW2 hold → LED_R 점등
- [ ] SW2 release → LED_R 0.2초 깜박 5초 (시료 투입 시간)
- [ ] 시료를 12% 연기 챔버에 넣음 → LED_R 0.5초 깜박 5회 측정
- [ ] delta ≥ 50 → LED_G 0.2초 깜박 1초(성공), needs_smoke=0
- [ ] delta < 50 → LED_R 0.2초 깜박 5초(거부), needs_smoke=1 유지(재시도 가능)
- [ ] 전원 재인가 → `Gain:` 값이 256이 아닌 시료별 값 확인
- [ ] 12% 연기 다시 주입 → `Norm:` 약 199, 3회 연속 시 알람

### 종합
- [ ] 7개 시료 모두 12% 보정 후 → 12% 연기에서 `Norm: 195~205`
- [ ] 9.6% 이상 연기에서 fire 트리거

## 10. 알려진 제약사항

1. **STEP 2-1 baseline = release 순간의 clean air** — SW2를 떼는 순간 ADC가 baseline. 연기 챔버 안에서 떼면 baseline 오염. (rev3의 "버튼 1회 누름 캡처"와 동일한 책임, 타이밍만 release 시점으로 변경)
2. **Smoke cal 재실행 = flash erase 후 처음부터** — step 3 성공 시 `g_calib_needs_smoke = 0`이 되어 SW2 hold가 더 이상 보정을 트리거 안 함(대신 화재음성 경로). 재보정은 STEP 2-1(erase)부터.
3. **STEP 3 블로킹** — 보정 중(투입 5초 + 측정 5초) 메인 루프 정지. 이 동안 화재 감지 안 됨.
4. **needs_smoke==0 + SW2 hold = 화재 음성(TODO)** — 메인 루프 정의 시 구현 예정.
5. **센서 노화 미대응** — 장기 drift 자동 보상 없음. 필요 시 사용자가 erase + 재보정.
