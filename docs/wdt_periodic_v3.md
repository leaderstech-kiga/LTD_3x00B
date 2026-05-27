# WDT 기반 주기 동작 분석 — LTD-3x00B (v3 코드 기준)

## 0. 한 줄 요약

메인 함수는 **두 가지 모드의 WDT 를 번갈아 가며** 동작한다.
저전력을 위해 평소에는 Stop 상태로 잠들어 있다가 **INTERRUPT 모드 WDT 가 0.192 s 마다 깨워주고**, 깨어 있는 동안에는 **RESET 모드 WDT 가 2 s 안에 hang 을 감지해서 리셋**한다.

```
        잠     깸  잠      깸   잠
       |---192ms---|--A--|--192ms--|--A--|--192ms--|...
        INT-WDT     ↑     INT-WDT   ↑     INT-WDT
                    RESET-WDT       RESET-WDT
                    무장             무장
                    (2 s 한도)       (2 s 한도)

A = active 구간 (수백 µs ~ 10 ms 정도, 평소엔 짧다)
```

이렇게 0.192 s 단위로 **system_count 가 1씩 증가**하며, 이 값에 대한 **모듈로 연산** 으로

- 매 40 카운트 (≈ 7.7 s) — 센서 ADC 체크 슬롯
- 매 292 카운트 (≈ 56 s) — 배터리 체크 + 전원 LED 깜박임
- 매 카운트 — SW2 버튼 / EMR_IO / SYS_mode 분기 (반응 latency 최대 0.192 s)

가 트리거된다.

---

## 1. 관련 하드웨어 레지스터

A96L416 데이터시트 + `A96L41x.h` 기반.

| 레지스터 | 주소 | 핵심 비트 | 의미 |
| --- | --- | --- | --- |
| `WDTCR` | 0x8D | b7 WDTEN | WDT enable (0 = off) |
|  |  | b6 WDTRSON | 1 = RESET on overflow / 0 = INTERRUPT (free-running 8-bit) |
|  |  | b5 WDTCL | 1 쓰면 카운터 클리어 (자동 클리어 1 cycle 후) |
|  |  | b2:1 WDTCK | 클럭 소스 — 00 = BIT-overflow / 01 = LFIRC / 10 = WDTRC |
|  |  | b0 WDTIFR | WDT 인터럽트 플래그 (0 쓰면 클리어) |
| `WDTDR` | 0x8E | 8-bit | 오버플로우 비교값 (0~255) |
| `OSCCR` | 0xC8 | b7 LFIRCE | LFIRC 발진기 enable |
|  |  | b4:3 HFIRCS | HFIRC 분주 — 00=DIV8(0.5MHz) / 11=DIV1(4MHz) |
|  |  | b2 HIRCE | HIRC 발진기 enable (0=enable, 1=disable) |
| `IE3` | — | b3 | WDT 인터럽트 mask (1 = 활성) |
| `PCON` | — | b1:0 | 00=Run / 01=Idle / 11=Stop |

### WDT 타이밍 공식 (실측 + 코드 주석 역산)

```
T_WDT = (WDTDR + 1) × 256 / (LFIRC_or_WDTRC / 4)
      = (WDTDR + 1) × 1024 / LFIRC_or_WDTRC
```

대략 LFIRC ≈ WDTRC ≈ 16 384 Hz 가정 시:

| WDTDR | 계산값 | 실측 주석 |
| --- | --- | --- |
| 2  | 0.1875 s | 0.192 s |
| 5  | 0.375 s  | 0.384 s |
| 7  | 0.500 s  | 0.512 s |
| 30 | 1.9375 s | ~ 2 s   |
| 124| 7.81 s   | ~ 8 s   |

값은 RC 공정 편차로 ± 10 % 정도 변동 가능.

---

## 2. 두 가지 WDT 모드 — 같은 하드웨어, 다른 역할

### 모드 A — `BeforeStop()` 의 INTERRUPT 모드 (sleep wake)

```c
void BeforeStop(void)
{
    OSCCR = 0x80;        // LFIRC enable (WDT 클럭 유지 위해)
    WDTCR = 0xA2;        // = 1010 0010
                         //   WDTEN=1, WDTRSON=0 (INTERRUPT), WDTCL=1, WDTCK=01 (LFIRC)
    WDTDR = 2;           // 0.192 s 타임아웃
    IE3 |= 0x08;         // WDT 인터럽트 unmask
}
```

목적은 **저전력 sleep 에서의 정기적 wake-up**.

- WDT 가 오버플로우하면 RESET 이 아닌 **인터럽트만 발생** → MCU 가 Stop 상태에서 깨어남
- LFIRC 는 Stop 모드에서도 살아있는 저주파 RC 발진기
- HFIRC 는 Stop 으로 꺼져있다가 wake 시 자동 재기동

### 모드 B — `WD_Reset()` 의 RESET 모드 (active hang 보호)

```c
void WD_Reset(void)
{
    WDTDR = 30;          // 약 2 s 타임아웃
    WDTCR = 0xE5;        // = 1110 0101
                         //   WDTEN=1, WDTRSON=1 (RESET), WDTCL=1, WDTCK=10 (WDTRC)
    IE3 |= 0x08;         // RESET 모드에서는 인터럽트 enable 영향 없지만 무해
}
```

목적은 **활성 코드 hang 시 자동 reset**.

- WDT 가 오버플로우하면 **MCU reset**
- 메인 루프가 정상 진행되면 매 사이클 진입 시 `WD_Reset()` 이 카운터를 클리어 ("kick the dog")
- SPI 무한 대기, ADC stall, ISR 데드락, EMI 로 인한 PC 깨짐 등에서 2 s 안에 자동 복구
- 화재 감지기로는 핵심 안전망

> WDTCL (bit 5) 비트가 매 WDTCR write 시 자동으로 카운터를 클리어한다. 즉 WDTCR 에 같은 값을 다시 쓰기만 해도 "리프레시" 가 된다.

### 두 모드 전환 시점

```
   loop top                            loop end
      ↓                                   ↓
   WD_Reset()  ─→  [active code] ─→  BeforeStop()  ─→  Stop()  ─→  WDT_Int  ─→  loop top
   ────────────  ─→  ─────────── ─→  ─────────────  ─→  ──────  ─→  ────────  ─→  ──────
   RESET 모드      RESET 모드 유지     INTERRUPT 모드     sleep      ISR 후
   (2 s)          ('kick' 매 cycle)   (0.192 s)         (Stop)     active 복귀
```

---

## 3. WDT 인터럽트 핸들러 — 거의 비어있는 ISR

```c
void WDT_Int(void) interrupt WDT_VECT
{
    WDTCR &= ~0x01;   // WDTIFR 클리어 (다음 인터럽트 재준비)
}
```

이 ISR 은 **"깨우기 신호" 외에 아무 일도 하지 않는다**. 깨우는 행위 자체는 Stop 모드의 wake 메커니즘이 처리하므로, ISR 은 단지 플래그를 클리어해서 다음 wake 가 준비되도록만 한다.

ISR 종료 후 CPU 는 `Stop()` 매크로 다음 라인 — 즉 `AfterStop()` 으로 복귀해 정상 코드 흐름으로 돌아온다.

```c
void AfterStop(void)
{
    IE3 &= ~(0x08);     // WDT 인터럽트 mask off (이제 RESET 모드로 바뀔 예정)
    OSCCR = 0x18;       // HFIRC/1 = 4 MHz 다시 활성 (sleep 중 0.5 MHz 모드에서 회복)
}
```

---

## 4. 메인 루프 한 사이클 — 시간 축 트레이스

`adc_time = 40`, `ck_led_time = 292` 가정. system_count 가 0 일 때:

```
0     1     2     3                    40                     292               1     2 …
|--A--|-s-|--A-|-s-|--…-|-s-|---A---|-s-|---A---|-s-|---…---|---A+Bat---|-s-|--A--|-s-|...
 ↑                          ↑       ↑   ↑                       ↑          ↑
 main 진입 직후              ADC      |   ADC                    Bat_Ck    Bat_Ck 후
 SystemStatus = SPI         체크 슬롯  |   체크 슬롯               (LED + UART)
                            (현재     |   (현재
                            body 비음) |   body 비음)
                            
A = active 구간 (수백 µs 평소, ADC 슬롯에서 LED+hw_initial 호출로 ~10 ms)
s = sleep 구간 (0.192 s)
```

매 wake 시점에 main 이 하는 일:

```
while(1) {                                           ─ system_count = N 시점
    WD_Reset();                                       ─ RESET 모드 2 s 무장

    if (N % ck_led_time == 0) {                       ─ 약 56 s 마다
        hw_initial_Wait(10);                          
        LED_G 5 ms 펄스                                ─ 살아있음 표시
        Bat_Ck();                                     ─ ADC + UART 로그 + Bat_mode 갱신
        hw_initial_Wait(10);
        // Bat_Ck 내부에서 system_count = 1 로 재설정
    }

    if (N % adc_time == 0) {                          ─ 약 7.7 s 마다
        hw_initial_Wait(10);
        LED_G 5 ms 펄스
        if (ADC_mode == Dust_mode) { /* 비어있음 */ }  ─ 사용자가 제거한 본문
        else if (ADC_mode == Temp_mode) { /* 비어있음 */ }
        hw_initial_Wait(10);
    }

    if (SYS_mode == In_Fire_al_mode) {                ─ 외부 진입 경로 없음 (잠재 dead)
        EMR_IO 토글
    }

    if (SYS_mode == Normal_mode && SW2 눌림) {        ─ 매 wake 마다 polling
        Play_Clip(AUDIO_CLIP_FIRE);                   ─ 수동 화재 테스트 (3 s 재생)
        hw_audio_initial();
    }

    if (Bat_mode == Bat_Low_mode) { /* 비어있음 */ }   ─ 사용자가 제거한 본문

    if (SYS_mode == Al_Stop_mode) {                   ─ Timer2_Int 가 SW2 long-press
        Play_Clip(AUDIO_CLIP_BOOT);                   ─  → Al_Stop_mode 설정 시 진입
        ...  8 × 40 s acknowledge 루프  ...
    }

    if (SYS_mode == Normal_mode && P10 입력 == 1) {   ─ EMR_IO 외부 트리거
        Play_Clip(AUDIO_CLIP_FIRE);
    }

    system_count++;                                   ─ 다음 wake 를 위해 +1

    BeforeStop();                                     ─ WDT 를 INTERRUPT 모드로 바꿈
    GLOBAL_INTERRUPT_EN();
    Stop();                                           ─ PCON=0x03 → Stop 진입
    AfterStop();                                      ─ wake 후 즉시 호출
}                                                     ─ 다음 사이클 시작
```

---

## 5. system_count 와 모듈로 산수

```
sleep cycle      = 0.192 s
adc_time         = 8 × 5         = 40      → 40 × 0.192 s ≈ 7.68 s
ck_led_time      = 56 × 5 + 12   = 292     → 292 × 0.192 s ≈ 56.06 s
```

**왜 이런 숫자인가?**

원래 설계는 8 s 마다 한 번 wake 하는 것이었지만, 사용자가 SW2 버튼 입력을 최대 0.5 s 안에 받아야 하기 때문에 wake 주기를 0.192 s 로 단축. ADC / Bat_Ck 의 효과적 주기는 그대로 유지하기 위해 모듈로 카운터로 분주:

- 8 s ÷ 0.192 s ≈ 41.67 → 40 으로 절삭 (7.68 s 마다)
- 56 s ÷ 0.192 s ≈ 291.67 → 292 카운트 (56.06 s 마다)

`Bat_Ck()` 가 끝나면 `system_count = 1` 로 강제 리셋. 즉 Bat_Ck 가 발생한 사이클 다음에 ADC 슬롯이 곧바로 겹치지 않도록 위상 분리.

**경계 케이스**

- system_count == 0 (부팅 직후): ck_led_time / adc_time 모두 % 0 → **양쪽 분기 동시 실행** (Bat_Ck → system_count=1, 이후 ADC 슬롯은 39 사이클 후)
- system_count overflow (uint16_t): 65536 사이클 = 약 3.5 시간 후 0 으로 wrap → 다시 양쪽 분기 동시 실행. 이는 의도된 동작 (배터리 체크가 더 잦아지는 부작용 없음).

---

## 6. SW2 버튼 반응 latency

- 매 wake (0.192 s) 마다 `Port_GetInputpinValue(PORT1, PIN2)` polling
- 최악 latency: SW2 누름 직후 곧바로 sleep 진입 → 0.192 s 까지 대기
- 평균 latency: 약 96 ms
- 사용자 체감 한계 (~200~300 ms) 보다 충분히 빨라서 "버튼이 빨랐다" 는 느낌

오디오 재생 (Audio_Run while 루프) 중에는 polling 안 함. 하지만 그 사이에 다른 SW2 누름이 들어와도 어차피 알람이 끝까지 재생되는 것이 정상 동작.

추가로 `TIMER2_Int` 도 100 ms 마다 SW2 상태를 보고 long-press 카운트 (`Al_Stop_key_Count`) 를 누적. 5 회 이상 (≈ 0.5 s) 이면 `SYS_mode = Al_Stop_mode` 로 전환.

---

## 7. Hang 보호 시나리오

### 시나리오 7-1: SPI 무한 polling (이미 fix 됨)

- 과거: `SPI_Receive` 가 RXC 안 올라오면 영원히 hang
- 현재: bounded wait + `SPI_Timeout` 플래그 (FIX C5/C7)
- 그래도 catch-all: WDT-RESET 이 2 s 안에 강제 reset

### 시나리오 7-2: ADC 무한 polling (방금 fix 됨)

- 과거: `while(!(ADCCRL & AFLAG));` 무한 대기
- 현재: bounded wait + `ADC_Timeout` 플래그 (FIX ADC3)
- catch-all: WDT-RESET 2 s

### 시나리오 7-3: ISR 데드락

- 만약 ISR 안에서 다른 ISR 이 firing 되어 stack 충돌 또는 깊은 nested
- main 으로 복귀하지 못함 → 다음 `WD_Reset()` 호출이 불가능 → 2 s 후 WDT-RESET

### 시나리오 7-4: EMI 로 PC 깨짐

- PC 가 의외의 주소로 점프 → 무한 루프 또는 잘못된 코드 실행
- 어차피 `WD_Reset()` 가 호출되지 않을 가능성 큼 → 2 s 후 WDT-RESET

### 시나리오 7-5: 오디오 재생 (정상 케이스이지만 길이가 길다)

- AUDIO_CLIP_FIRE = 3.04 s, AUDIO_CLIP_LOWBAT = 2.68 s
- 둘 다 활성 WDT 2 s 보다 김
- Audio_Run while 루프 안에 `WD_Reset()` 호출 → 정상 재생 중 reset 없음
- 만약 ADPCM 디코더가 비정상으로 멈추면 (`Audio_length < Audio_max_length` 가 갱신 안 됨), while 안 WD_Reset 도 못 도달 → 2 s 후 WDT-RESET

---

## 8. 전력 계산

### 정상 사이클 (0.192 s 주기)

```
구간           시간       전류         에너지 (소비량)
─────────────────────────────────────────────────────────
sleep         ~190 ms    ~1.5 µA      0.285 nA·h
active        ~2 ms      ~2 mA        4.0 µA·s = 1.11 µA·h × 0.01 
LED 펄스      ~5 ms     ~10 mA        50 µA·s = 13.9 µA·h × 0.01
─────────────────────────────────────────────────────────
사이클 평균    192 ms     ~50 µA 등가
```

**평균 전류 추산**: `(1.5 µA × 190 ms + 2000 µA × 2 ms) / 192 ms ≈ 22 µA`

ADC 슬롯 / Bat_Ck 슬롯에서 active 가 잠시 늘어나지만 7.68 s / 56 s 마다이므로 평균에 미치는 영향은 작음.

### 10 년 동작 가능 여부

CR17450 × 2 병렬 = 2400 mAh × 2 = 4800 mAh

10 년 = 87 600 시간 → 평균 허용 = 54.8 µA

추산 22 µA < 54.8 µA → **마진 약 2.5 배**. 음성 재생 1 회당 약 1 mA·s ≈ 0.3 µA·h 소비, 일주일에 한 번 화재 테스트 가정 시 무시 가능.

---

## 9. 디버깅 팁

### 9-1: wake 주기가 안 맞는다고 느낄 때

`OSCCR` 값을 의심하라. `BeforeStop` 의 `OSCCR = 0x80` 은 LFIRC 만 켜고 HFIRC 분주를 묻지 않는다 (Stop 진입 후 HFIRC 자동 off). 만약 `AfterStop` 의 `OSCCR = 0x18` 이 실행 안 되면 wake 후 CPU 가 0.5 MHz 로 8 배 느리게 돌아간다.

### 9-2: 가끔 reset 이 발생한다고 의심될 때

`RSTFR` 레지스터 (0x???) 를 부팅 직후 읽어보면 reset 원인이 비트로 표시된다:
- POR / LVR / EXT / WDT 중 어느 것이었는지 알 수 있음
- `RSTFR = 0x00` 으로 클리어해야 다음 reset 추적이 정확

### 9-3: WDT 가 너무 자주 reset 한다면

- WD_Reset 호출 간격이 2 s 를 넘는 코드 경로 추적
- 후보: Bat_Ck 안 Uart_Out 의 9600 baud 출력이 너무 길 수 있음. 100 bytes × ~1 ms/byte = 100 ms → 안전 마진 안에는 있음

### 9-4: WDT 가 잠에서 못 깨운다면

- `IE3` 의 b3 (WDT IE) 가 BeforeStop 직전 1 이어야 함
- `EA` (GLOBAL_INTERRUPT_EN) 가 1 이어야 함 — Stop() 직전 명시 호출 있음
- LFIRC 가 살아있어야 함 — `OSCCR & 0x80 == 0x80`

---

## 10. 종합 도식

```
   +------------------+
   |  POR / WDT-Reset |
   +--------+---------+
            |
            v
   +------------------+
   | main 진입         |
   |  hw_initial_Wait  |
   |  SPI_Memory_Check |
   |  SYS_mode=Normal  |
   +--------+---------+
            |
            v
   +--------------------+   <----------------------------+
   |  while(1) 진입      |                                |
   |    WD_Reset()      |  (RESET 모드 2 s)               |
   +--------+-----------+                                |
            |                                            |
            v                                            |
   +--------------------+                                |
   | active 구간:        |                                |
   |  Bat_Ck (56 s)     |                                |
   |  ADC slot (7.7 s)  |                                |
   |  SW2 polling       |                                |
   |  Fire test / Stop  |                                |
   |  system_count++    |                                |
   +--------+-----------+                                |
            |                                            |
            v                                            |
   +--------------------+                                |
   |  BeforeStop()      |                                |
   |   WDTCR=0xA2 (INT) |  (INTERRUPT 모드 0.192 s 무장)   |
   |   WDTDR=2          |                                |
   +--------+-----------+                                |
            |                                            |
            v                                            |
   +--------------------+      WDT 인터럽트                |
   |  Stop()  (PCON=03) | =====================+         |
   |  sleep 약 0.192 s   |                      ↓         |
   +--------+-----------+              +----------------+ |
            |                          |  WDT_Int ISR   | |
            |                          |  WDTCR &= ~01  | |
            |  CPU 깨어남               +-------+--------+ |
            |                                  |          |
            v                                  v          |
   +--------------------+   ←  ISR 종료 후 복귀  ─        |
   |  AfterStop()       |                                |
   |   IE3 &= ~08       |                                |
   |   OSCCR=0x18 (4MHz)|                                |
   +--------+-----------+                                |
            |                                            |
            +--------------------------------------------+
```

---

## 11. 핵심 포인트 3가지

1. **WDT 가 두 가지 역할을 한다**. 잠들 때는 알람시계 (INTERRUPT, 0.192 s), 깨어 있을 때는 안전망 (RESET, 2 s). 이걸 한 페리퍼럴로 처리하기 때문에 `BeforeStop` / `WD_Reset` 두 함수가 같은 `WDTCR` 을 매번 다르게 쓴다.

2. **time-base 는 카운터 + 모듈로**. 절대 시각 추적 없이 `system_count` 1개만 증가시키며 `% adc_time` / `% ck_led_time` 으로 분기. 단순하고 견고하다.

3. **버튼 반응속도 < 200 ms** 가 모든 wake 주기 설계의 1차 제약. ADC 8 s, Bat 56 s 같은 더 긴 주기는 모두 0.192 s 의 정수 배수로 표현된다. 만약 wake 를 더 길게 잡으면 (예: 1 s) 버튼이 답답해진다.
