# LTD-3x00B 메인 동작 구조 분석 (v3 — 정리 후)

## 1. 진입 시퀀스

```
RESET / Power-on
  └─ ?C_START runtime (Keil)
       └─ main()
            ├─ hw_initial_Wait(10)                  ; GPIO/타이머/와치독 초기화 + 10 ms 안정화
            ├─ SystemStatus = SPI_Memory_Check()    ; SPI flash JEDEC ID 검증, LED 1회 표시
            ├─ ADC_mode = Dust_mode                 ; 모드 플래그(현재 본문 무동작)
            ├─ hw_initial_Wait(10)
            ├─ system_count = 0
            ├─ SYS_mode = Normal_mode
            └─ while(1) { ... }                     ; 메인 루프
```

## 2. 메인 루프 (while(1)) — 시간축으로 본 동작

루프 1 회 = 깨어남 → 처리 → Stop() 진입 → WDT(0.192 s) 만료 → 깨어남 …
**system_count 는 매 사이클 1 씩 증가**.

```
loop top
 │
 ├─ WD_Reset()                                  ; 활성 모드 WDT(RESET, 2 s) 재시동
 │
 ├─ if (system_count % ck_led_time == 0)        ; ck_led_time = 56·5+12 = 292 cycles
 │  └─ hw_initial_Wait(10)
 │     LED_G 5 ms 펄스
 │     Bat_Ck()                                 ; ── 배터리 ADC + UART 로그 + Bat_mode 갱신
 │     hw_initial_Wait(10)
 │
 ├─ if (system_count % adc_time == 0)           ; adc_time = 8·5 = 40 cycles
 │  ├─ hw_initial_Wait(10)
 │  ├─ LED_G 5 ms 펄스
 │  ├─ (Dust_mode  → 본문 비어 있음)            ; ── 사용자 의도로 제거
 │  ├─ (Temp_mode  → 본문 비어 있음)
 │  └─ hw_initial_Wait(10)
 │
 ├─ if (SYS_mode == In_Fire_al_mode)            ; ── 외부 진입 경로 없음 (잠재 dead)
 │  └─ EMR_IO 토글 + 10 ms 대기 (음성 재생 없음)
 │
 ├─ if (SYS_mode == Normal_mode && SW2 눌림)    ; ── 수동 화재 테스트 트리거
 │  ├─ EMR_IO HIGH (P10=1)
 │  ├─ T2_init() / Fire_Alarm_LED=1 / Timer2_Start()
 │  ├─ Play_Clip(AUDIO_CLIP_FIRE)               ; ── 약 3 s ADPCM 재생
 │  ├─ hw_audio_initial() + Delay_ms(10)
 │  ├─ Timer2_Stop / LED off / EMR_IO LOW
 │  └─ hw_initial_Wait(10)
 │
 ├─ if (Bat_mode == Bat_Low_mode)               ; ── 본문 비어 있음 (저전압 알람 미구현)
 │
 ├─ if (SYS_mode == Al_Stop_mode)               ; ── TIMER2_Int 가 SW2 long-press 인식 시 설정
 │  ├─ Play_Clip(AUDIO_CLIP_BOOT)               ; ── stop 확인음 (~1.35 s)
 │  ├─ Timer2_Stop / EMR_IO LOW
 │  ├─ SYS_mode = Normal_mode
 │  ├─ 8 × 40 s 침묵 루프 (SW2 누르면 조기 탈출)
 │  └─ hw_initial_Wait(10)
 │
 ├─ if (SYS_mode == Normal_mode && P10 입력 == 1)  ; ── 외부 EMR_IO 핀이 HIGH 일 때 재생
 │  ├─ Play_Clip(AUDIO_CLIP_FIRE)
 │  └─ hw_audio_initial() + 20 ms 대기
 │
 ├─ system_count++
 │
 └─ BeforeStop() → Stop() → AfterStop()         ; ── 0.192 s sleep (WDT 인터럽트로 wake)
```

## 3. ISR / 백그라운드 일감

| 벡터 | 핸들러 | 역할 |
| --- | --- | --- |
| `T2_MATCH_VECT` | `TIMER2_Int` | 10 Hz 인터럽트. Fire_Alarm_LED / Bat_Alarm_LED 깜박임 + SW2 long-press 감지 → `SYS_mode = Al_Stop_mode` |
| `T1_MATCH_VECT` (audio.c) | `TIMER1_Int` | ADPCM 8 kHz 샘플 틱 |
| `T0_MATCH_VECT` (audio.c) | `TIMER0_Int` | PWM 캐리어 갱신 |
| `WDT_VECT` | `WDT_Int` | INTERRUPT 모드 WDT (sleep wake) — `WDTCR &= ~0x01` |

## 4. 도달 가능한 함수 트리

```
main
 ├─ hw_initial_Wait → hw_initial, Delay_ms, Port_*
 ├─ SPI_Memory_Check → CVDD_ON/OFF, NOP_20us_Delay, USART_SPI_Initial, Get_Identification, LED_*, Delay_ms
 ├─ WD_Reset
 ├─ Bat_Ck
 │   ├─ BAT_ADC_Avr → ADC_Initial, ADC_SelectChannel, ADC_GetDataWithPolling, Data_Sorting, NOP_20us_Delay
 │   ├─ Uart_Out → Uart_Send_Decimal, USART_*
 │   └─ Uart_Set → USART_Initial, Port_SetAlterFunctionpin
 ├─ T2_init → Timer2_*
 ├─ Play_Clip → Audio_Run
 │              └─ CVDD_ON/OFF, AUDIO_ON/OFF, USART_SPI_Initial, Variable_Initial,
 │                 Audio_Initial (audio.c), Timer1_Start, WD_Reset
 ├─ hw_audio_initial
 ├─ BeforeStop / Stop / AfterStop
 └─ ISRs: TIMER2_Int, WDT_Int, TIMER0_Int(audio.c), TIMER1_Int(audio.c)
```

## 5. 제거 대상 함수 (도달 불가능)

| 함수 | 위치 | 사유 |
| --- | --- | --- |
| `Delay_s` | main.c:641 | 호출 없음 |
| `Fire_Alarm` | main.c:1130 | 본문 99 행. 유일한 호출 자리 `//Fire_Alarm();` 주석 처리됨 |
| `LowBat_Alarm` | main.c:1237 | 호출 없음 (저전압 알람은 Bat_Low_mode 빈 분기로 대체) |
| `ADC_Uart_Out` | main.c:1469 | 호출 없음 |
| `Uart_Out_avr` | main.c:1502 | 호출 없음 |
| `Data_Avr` | main.c:1653 | 호출자(`Dust_ADC_Single`, `Dust_ADC_Dual`)가 모두 제거 대상 |
| `Dust_ADC_1AMP` | main.c:1674 | dust 측정 본문 제거됨 |
| `Dust_ADC_2AMP` | main.c:1751 | 동일 |
| `TEMP_ADC` | main.c:1839 | Temp_mode 분기 비어 있음 |
| `BAT_ADC` | main.c:1874 | 호출 없음 (BAT_ADC_Avr 만 사용) |
| `Enable_WD` | main.c:1941 | main 이 BeforeStop/Stop/AfterStop 을 직접 호출 |
| `Check_Uart_Mode` | main.c:2045 | 호출 없음 (UART 디버그 진입 모드 비활성) |
| `Dust_ADC_Single` | main.c:2168 | Check_Uart_Mode 에서만 사용 |
| `Dust_ADC_Dual` | main.c:2225 | 동일 |
| `Uart_Test_Out` | main.c:2272 | Check_Uart_Mode 에서만 사용 |

**main.h 에서 함께 제거할 프로토타입** : 위 15 개 + `Audio_Init` (정의 없음, 잔존 선언) + `Visu_MODE_CK` (정의 없음) + `det_adc` (정의 없음).

## 6. 제거 대상 전역 변수 / 매크로

| 식별자 | 위치 | 사유 |
| --- | --- | --- |
| `#define Dust_1amp_Val_old` | 88 | 미사용 임계값 매크로 |
| `#define Dust_Val_old` | 90 | 동일 |
| `#define Dust_Visual_1amp_Val` | 92 | 동일 |
| `#define Dust_Visual_Val` | 94 | 동일 |
| `Dust_Val, Dust_1amp_Val` | 99 | 미사용 |
| `start_uart_debug` | 122 | Check_Uart_Mode 인자였음, 제거됨 |
| `Bat_al_time` | 145 | 미사용 |
| `fire_off_cnt`, `bat_first` | 150–151 | 미사용 |
| `ADC_wait, charge_wait, bat_alarm_wait, alarm_off_wait` | 206–209 | 미사용 |
| `Dust_Count, Temp_Count` | 217–218 | 미사용 |
| `init_Dust_val, delta_Dust_val` | 221–222 | 미사용 |
| `Low_Bat_count` | 225 | 미사용 |
| `temp_charge_data[10], charge_data_Avr, temp_charge_data_sum` | 236–238 | 미사용 (BAT_ADC_Avr 오버플로 타깃이었던 잔존물) |
| `adc_read_val, ADC_port` | 240–241 | 미사용 |
| `uart_pkey_cnt` | 243 | 미사용 |
| `int_val, tmp` (전역) | 262–263 | 미사용 — 동명의 지역 변수만 사용 |
| `Avr[9] = "Average: "` | 270 | Uart_Out_avr 에서만 참조 |
| `Dust_Si_Off/On/Deta`, `Dust_Du_Off/On/Deta` | 272–278 | Uart_Test_Out 에서만 참조 |
| `adc_tmp[10]` | 285 | 미사용 |
| `Charge_mode, Charge_mode_cnt, Charge_ch_val` | 295–296 | 미사용 |
| `int j` (전역) | 298 | shadow — 지역 j 만 사용 |
| `struct Adc_Val_Def` + `Single_Amp_Adc, Dual_Amp_Adc` | 300–307 | Dust_ADC_Single/Dual + Uart_Test_Out 에서만 사용 |

## 7. 유지 변수 (Uart_Out 가 참조)

다음은 Uart_Out 가 매번 출력하므로 유지. 값이 갱신되는 코드가 없어 현재는 모두 0 으로 출력되지만, 함수 시그너처/UART 로그 포맷을 보존.

`bat_val`, `Visu_data`, `Dust_o_data`, `Dust_data`, `Dust_Sum_data`, `Dust_Low_o_data`, `Dust_Low_data`, `Tmp_data`, `visual_type`, UART 라벨 배열 (`Dust_MODE`, `Visu_MODE`, `Temp_MODE`, `ADC`, `CHR`, `INF`, `Dust`, `Dust_ori`, `Temp`, `Bat`, `Tab`, `End`)

## 8. 영향

- 코드 크기 감소: 약 600 행 (전체 ~2400 행 → ~1800 행, 25 % 감소)
- IRAM 사용량 감소: 약 60 byte 추정 (구조체 인스턴스 12 byte + 배열 24 byte + 단일 변수 24 byte)
- 빌드 동작은 변하지 않음 — 제거 대상은 모두 dead code
- 향후 재도입 시: docs/backup/main.c.bak 또는 git 히스토리에서 복구
