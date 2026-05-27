# ADC 코드 정밀 검토 — LTD-3x00B 펌웨어

검토 대상

- `src/a96l41x_adc.c` / `a96l41x_adc.h`
- `src/main.c` 의 ADC 호출부 8건 (`Dust_ADC_1AMP`, `Dust_ADC_2AMP`, `TEMP_ADC`, `BAT_ADC`, `BAT_ADC_Avr`, `Dust_ADC_Single`, `Dust_ADC_Dual`, `hw_initial`)
- `src/A96L41x.h` 의 ADCCRH / ADCCRL 레지스터 비트 정의

대조 데이터시트 정보 (`A96L41x.h` SFR 정의 기준)

| 레지스터 | 주소 | 비트 | 의미 |
| --- | --- | --- | --- |
| ADCCRL | 0xCA | b7 STBY | 0=ADC 모듈 Disable, 1=Enable |
|  |  | b6 ADST | 1 쓰면 변환 시작, auto-clear |
|  |  | b5 REFSEL | 0=Internal(VDD), 1=External(AVREF) |
|  |  | b4 AFLAG | 변환 완료(read-only — STBY=0 또는 STOP 진입에서 clear) |
|  |  | b3:0 CHSEL | 채널 선택 |
| ADCCRH | 0xCB | b7 ADCIFR | ADC 인터럽트 플래그, write-0-to-clear |
|  |  | b3 TRGSEL | 0=ADST(SW), 1=Timer0 match |
|  |  | b2 ALIGN | 0=MSB align, 1=LSB align |
|  |  | b1:0 CKSEL | 00=4MHz / 01=2MHz / 10=1MHz / 11=0.5MHz |
| ADCDR | 0xCC | 10-bit | 변환 결과 (정렬에 따라 MSB/LSB) |

ADC 변환 시간 = ADCLK × 28 클럭 → 2 MHz 에서 14 µs (데이터시트 최소).

---

## 1. ADC_Initial 의 숨겨진 함정 — `ADC_CLK_4M` 이 강제로 `ADC_CLK_2M` 으로 다운그레이드

a96l41x_adc.c 91 행

```c
void ADC_Initial(uint8_t clock_sel, uint8_t trigger_sel, uint8_t ref_voltage, uint8_t align_sel)
{
    ...
    if(clock_sel == 0){            // ADC_CLK_4M == 0
        clock_sel = 1;             // -> ADC_CLK_2M 으로 무조건 변경
    }

    temp_reg = ADCCRH;
    temp_reg &= ~(0xFF);
    temp_reg |= ((trigger_sel << 3) | (align_sel << 2) | (clock_sel << 0));
    ...
}
```

main.c 의 6 곳이 `ADC_CLK_4M` 을 인자로 넘기지만 (1736, 1769, 1813, 1894, 1929, 1930 행) 실제 레지스터에는 항상 `01b`(2 MHz) 가 기록된다. 코드 의도와 실제 동작이 어긋난다는 점이 가장 큰 가독성/유지보수 문제다. Stand_along_Abov 참조 코드도 같은 패턴이라 의도된 강제 다운그레이드로 보이지만, 매크로 이름은 잘못된 인상을 준다.

권장 조치

- 호출부 6 곳을 `ADC_CLK_2M` 으로 통일하고, `ADC_CLK_4M` 인자 자체를 사용하지 않는다 (또는 헤더에서 deprecated 표기).
- `ADC_Initial` 의 강제 변경 분기를 명시 주석으로 남긴다 — “4 MHz 는 변환 시간 7 µs 가 되어 데이터시트 최소 14 µs 를 위반함. 안전한 2 MHz 로 강제 변환.”

---

## 2. `adc_count`, `adc_buff`, `buff_cnt` 가 volatile 이 아님 — ISR/메인 thread 공유 (C2 패턴)

```c
uint8_t  adc_count = 0;
uint16_t *adc_buff;
uint8_t  buff_cnt  = 0;
```

`ADC_GetDataWithInterrupt()` 마지막 줄

```c
while(0 < adc_count);   // adc interrupt subroutine execute
```

`adc_count` 는 ISR (`ADC_Int_Handler`) 에서 `--` 되고 메인 스레드에서 busy-wait. Keil C51 은 최적화 옵션에서 변수를 레지스터에 캐시할 수 있어 무한 루프 위험. 이미 C2 패턴으로 처리한 `Audio_*`, `SystemStatus` 와 동일한 유형의 버그다.

권장 조치 — `a96l41x_adc.c` 48~50 행

```c
volatile uint8_t  adc_count = 0;
uint16_t * volatile adc_buff;
volatile uint8_t  buff_cnt  = 0;
```

`ADC_GetDataWithInterrupt` 가 실제로 사용되는지 확인: main.c 어디에서도 호출되지 않는다 (모두 `ADC_GetDataWithPolling` 사용). 미사용 코드라도 vendor 드라이버이므로 volatile 만 보강해두는 것이 안전.

---

## 3. `ADC_GetDataWithPolling` 의 무한 대기 — SPI 와 동일한 hang 위험

```c
while(!(ADC_GetConversionStatus()));     // ADCCRL & 0x10
```

- 정상이라면 14~28 µs 안에 AFLAG 가 1 로 올라가 빠져나간다.
- 그러나 AVREF 핀이 떠 있거나, ADC 모듈에 latch-up 이 걸리면 영원히 hang. SPI 의 C5/C7 와 동일한 패턴.
- 활성 모드 RESET-WDT (FIX C4 이후 2 s) 가 작동하므로 “리셋된다”는 안전망은 있다.

권장 조치 — bounded wait + 타임아웃 플래그

```c
uint8_t ADC_GetDataWithPolling(uint16_t *adc_data, uint8_t count)
{
    uint8_t  i;
    uint16_t wait;

    for(i = 0; i < count; i++) {
        if(((ADCCRH & 0x08) == 0x00))
            ADC_StartSoftwareTrigger();

        wait = 0x2000;                       /* ~ 수 ms 상한 */
        while(!(ADC_GetConversionStatus()) && (--wait));
        if(wait == 0) {
            ADC_Timeout = 1;                 /* 신설 플래그 */
            return 0;                         /* 호출자에 통보 */
        }
        adc_data[i] = ADCDR;
    }
    ADC_Timeout = 0;
    return 1;
}
```

SPI 의 `SPI_Timeout` 와 같은 패턴.

---

## 4. AFLAG 클리어 정책 — 데이터시트 명시 부족, 잠재적 false-pass 위험

`A96L41x.h` 의 AFLAG 비트 주석:

> AFLAG : This bit is cleared to '0' when the STBY bit is set to '0' or when the CPU is at STOP mode.

이 설명만 본다면 변환 후에도 AFLAG 가 1 로 sticky 가 되어, 다음 SW 트리거 직후 polling 루프가 즉시 빠져나오는(=새 변환 완료 전에 ADCDR 읽음) 위험이 있다. 다만 실제 동작에서는 큰 문제 없이 측정값이 나오고 있어, “트리거 시 또는 ADCDR 읽기 시 자동 clear” 가 실제 동작일 가능성이 높다(데이터시트 설명 불완전). 그래도 코드 차원의 안전장치를 두는 것이 좋다.

권장 조치 — 각 SW 트리거 직전에 AFLAG 명시 클리어

```c
ADCCRL &= ~(1 << 4);                /* clear AFLAG before new conversion */
ADC_StartSoftwareTrigger();
while(!(ADCCRL & (1 << 4)) && (--wait));
adc_data[i] = ADCDR;
```

이렇게 하면 데이터시트 문구와 실제 하드웨어 동작 차이에 무관하게 안전.

---

## 5. ADC 시작 안정화 시간 (TSTAB) 부재

`ADC_Initial` 직후 `ADC_SelectChannel` → `ADC_GetDataWithPolling` 가 바로 호출된다. ABOV 데이터시트는 통상 ADC 모듈 enable 후 수 µs~수십 µs 의 stabilization 을 요구한다. 특히 VBGR 채널 (내부 밴드갭 1.2 V 기준) 은 ADC 코어 안정화 + 밴드갭 자체 안정화까지 필요해 첫 1~2 샘플이 부정확할 수 있다.

현재 코드의 완화책

- `Dust_ADC_Single` / `Dust_ADC_Dual` : `ADC_SelectChannel(VBGR); Delay_ms(1);` (1 ms — 충분)
- `Dust_ADC_1AMP` / `Dust_ADC_2AMP` : 별도 안정화 지연 없이 바로 측정 → buf_cnt(5~10) 샘플 후 중앙값(median)을 취함으로써 초기 글리치를 어느 정도 흡수
- `TEMP_ADC` : `LDO_ON; NOP_20us_Delay(10)` 200 µs 후 측정
- `BAT_ADC` : `NOP_20us_Delay(10)` 만 — VBGR 안정화 시간 충분치 않을 수 있음
- `BAT_ADC_Avr` : 동일

권장 조치 — `ADC_Initial` 끝에 일관된 stabilization 지연 삽입

```c
ADCCRH = temp_reg;
ADCCRL = temp_reg2;
NOP_20us_Delay(5);          /* 100 µs ADC + VBGR settle */
```

또는 호출부에서 VBGR 채널 선택 후 명시적으로 100 µs 이상 지연.

---

## 6. **[CRITICAL]** `BAT_ADC_Avr` 의 버퍼 오버플로우 + 인덱스 불일치

main.c 2189~2223 행

```c
uint16_t temp_data[ADC_MAX_BUFFER_SIZE];    /* 232 행, ADC_MAX_BUFFER_SIZE = 3 */
...
uint16_t BAT_ADC_Avr(uint8_t buf_cnt){
    ...
    ADC_GetDataWithPolling(temp_data, buf_cnt);    /* buf_cnt=5 or 10 */
    Data_Sorting(temp_data, buf_cnt);

    if(temp_data[buf_cnt/2] == 0) {                /* buf_cnt/2 -> 2 or 5 */
        temp_data[buf_cnt/2] = 1;
    }
    op = 92;
    op *= 1024;
    op /= temp_data[ADC_MAX_BUFFER_SIZE/2];        /* <-- 항상 index 1 */
    Bat_ADC = op;

    return Bat_ADC;
}
```

호출부

- `main.c:2173` (Check_Uart_Mode) `BAT_ADC_Avr(10)` — temp_data 에 **10개 write**
- `main.c:2417` (Bat_Ck)         `BAT_ADC_Avr(5)`  — temp_data 에 **5개 write**

**문제 1 — Buffer overflow.** `temp_data` 의 실제 크기는 3 (`ADC_MAX_BUFFER_SIZE`). buf_cnt=10 호출은 7 워드(14 byte)를 인접 글로벌 변수에 덮어쓴다. 232 행 직후에 선언된 변수들이 클로버된다:

```c
232  uint16_t temp_data[ADC_MAX_BUFFER_SIZE];      // 3 워드, 6 byte
236  uint16_t temp_charge_data[10];                // 다음 변수 - 클로버 대상
237  uint16_t charge_data_Avr;
238  uint32_t temp_charge_data_sum;
240  uint16_t adc_read_val;
241  uint8_t  ADC_port;
...
```

UART 디버그 모드의 첫 측정에서 인접 글로벌이 깨지면, 이후 충전(charge) 로직과 sort 결과가 오염된다. **수정 필수**.

**문제 2 — 인덱스 불일치.** `if(temp_data[buf_cnt/2] == 0)` 의 0 가드는 `buf_cnt/2` 위치(예: 5)를 본 다음, 실제 나눗셈은 `temp_data[ADC_MAX_BUFFER_SIZE/2] = temp_data[1]` 에서 한다. 0 가드와 분모가 서로 다른 셀을 본다.

권장 수정

```c
uint16_t BAT_ADC_Avr(uint8_t buf_cnt){
    uint16_t Bat_ADC;
    uint32_t op;
    uint16_t adc_buf[ADC_BUFFER_SIZE];     /* size 16, buf_cnt <= 16 */

    if(buf_cnt > ADC_BUFFER_SIZE) buf_cnt = ADC_BUFFER_SIZE;

    P0FSRL = ...
    NOP_20us_Delay(10);

    ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);
    ADC_SelectChannel(ADC_VBGR);
    NOP_20us_Delay(5);                     /* VBGR settle */
    ADC_GetDataWithPolling(adc_buf, buf_cnt);
    Data_Sorting(adc_buf, buf_cnt);

    if(adc_buf[buf_cnt/2] == 0) adc_buf[buf_cnt/2] = 1;

    op  = 92;
    op *= 1024;
    op /= adc_buf[buf_cnt/2];              /* 0 가드와 동일 셀 */
    Bat_ADC = op;
    return Bat_ADC;
}
```

`temp_data` 글로벌은 BAT_ADC()/TEMP_ADC() 에서만 ADC_MAX_BUFFER_SIZE(3) 로 일관되게 쓰이므로 그대로 둔다.

---

## 7. 매번 ADC_Initial 재호출 — 비효율

`Dust_ADC_1AMP` 한 호출 안에서 `ADC_Initial` 이 두 번 호출된다 (1790, 1823 행).

```c
ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);   // VBGR 측정용
ADC_SelectChannel(ADC_VBGR);
ADC_GetDataWithPolling(...);
...
LDO_ON;
NOP_20us_Delay(5);
ADC_SelectChannel(ADC_OP0OUT);     // <-- ADC_Initial 없이 채널만 변경
ADC_GetDataWithPolling(...);
LDO_OFF;
...
ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);   // 다시 초기화
ADC_SelectChannel(ADC_OP0OUT);
ADC_GetDataWithPolling(...);
```

세 번째 측정 직전의 `ADC_Initial` 은 같은 인자라 사실상 NOP. ADCCRH/ADCCRL 을 한 번 더 쓰면서 ADCEN(STBY) 비트 toggle 까지 안 하므로 단순한 redundancy. 그러나 ADCCRL 의 채널 비트가 0(AN0) 으로 되돌아갔다가 즉시 OP0OUT 으로 재설정되므로, 만약 그 사이에 ADC 가 idle 상태가 아니라면 짧은 글리치 가능. 안전상 큰 문제는 아니지만 코드 정리 차원에서 제거 권장.

권장 조치 — 한 함수 내 동일 설정이면 ADC_Initial 은 진입부에 1회만 호출, 나머지는 SelectChannel + Delay 로 충분.

---

## 8. 클럭 설정 불일치 — 호출부별 2 MHz / "4 MHz(실제 2 MHz)" 혼재

호출부별 `clock_sel` 매크로:

| 함수 | 매개변수 | 실제 ADCLK |
| --- | --- | --- |
| Dust_ADC_1AMP × 3 | ADC_CLK_4M | 2 MHz (강제 다운그레이드) |
| Dust_ADC_2AMP × 1 | ADC_CLK_4M | 2 MHz |
| TEMP_ADC | ADC_CLK_4M | 2 MHz |
| BAT_ADC | ADC_CLK_4M | 2 MHz |
| BAT_ADC_Avr | ADC_CLK_2M | 2 MHz |
| Dust_ADC_Single | ADC_CLK_2M | 2 MHz |
| Dust_ADC_Dual | ADC_CLK_2M | 2 MHz |

결과적으로 모두 2 MHz 로 통일되지만 인자만 다른 “죽은 변형” 이다. 호출부 6 곳을 `ADC_CLK_2M` 로 통일하면 의도가 명확해진다.

---

## 9. `ADC_Int_Handler` 의 잉여 트리거 1 회 (마이너)

```c
void ADC_Int_Handler(void) interrupt ADC_VECT
{
    if(((ADCCRH & 0x08) == 0x00) && (adc_count != 0)) //SW trigger
        ADC_StartSoftwareTrigger();              /* (A) */

    adc_count--;                                  /* (B) */
    adc_buff[buff_cnt++] = ADCDR;
    if(adc_count == 0)
        ADC_ConfigureInterrupt(FALSE);
}
```

`count=5` 로 시작했을 때 ISR 가 다섯 번 호출되며, **마지막 호출에서도** (A) 의 조건 (`adc_count != 0`) 이 통과해 “6번째” 변환을 트리거한 직후 (B) 에서 `--` 되어 0 이 되고 인터럽트가 비활성화된다. 이 6번째 변환은 결과를 읽지 않으므로 단순히 낭비. 보정안:

```c
if(((ADCCRH & 0x08) == 0x00) && (adc_count > 1))   /* > 1 로 변경 */
    ADC_StartSoftwareTrigger();
```

`ADC_GetDataWithInterrupt` 가 현재 메인 코드에서 사용되지 않으므로 영향은 없지만, 외부에서 인터럽트 모드를 도입할 경우 동작에 영향 가능.

---

## 10. ALIGN = LSB 가 일관됐는지 — OK

모든 호출이 `ADC_LSB` 사용. ADCDRH[1:0], ADCDRL[7:0] 에서 10-bit 값 읽기. `ADCDR` 의 sfr16 정의(0xCC) 가 자동으로 little-endian 16-bit 로 묶어주므로 `adc_data[i] = ADCDR;` 만으로 정렬-맞춤된 값이 들어온다.

확인: `A96L41x.h:205 sfr16 ADCDR = 0xCC;` — Keil C51 의 sfr16 은 LSB-first 컴파일. ADCDRL 에 LSB, ADCDRH 에 MSB. LSB align 에서 ADCDRH 상위 6 비트는 0. 즉 10-bit 범위 0~1023 으로 읽힘. 일관됨.

만약 MSB align 으로 변경한다면 결과를 6 비트 우측 시프트해야 하므로, 향후 다른 사람이 align 인자를 바꿔도 폭주하지 않도록 호출부에서 `ADC_LSB` 를 하드코딩하기보다는 매크로 상수로 묶어두는 편이 안전.

---

## 11. P0FSRL 재설정 영향 — 정상

각 ADC 함수는 자신이 필요한 채널만 enable 하기 위해 P0FSRL 을 매번 통째로 재설정한다.

| 함수 | P0FSRL 설정 | P00 | P01 | P02 | P03 |
| --- | --- | --- | --- | --- | --- |
| hw_initial | 기본 | I/O (CVDD) | AN1 (BAT) | OP1P | OP0OUT |
| Dust_ADC_1AMP | 1,2,1,1 | OP1OUT? | AN1 | OP1P | OP0OUT |
| Dust_ADC_2AMP | 동일 (변경 없음) | — | — | — | — |
| TEMP_ADC | 0,2,2,2 | I/O | AN1 | AN2 | AN3 |
| BAT_ADC | 0,1,1,1 | I/O | AN1 | OP1P | OP0OUT |
| BAT_ADC_Avr | 0,1,1,1 | 동일 | | | |
| Dust_ADC_Single | 1,2,1,1 | OP1OUT? | AN1 | OP1P | OP0OUT |
| Dust_ADC_Dual | 1,1,1,1 | OP1OUT? | OP1N | OP1P | OP0OUT |

주의점

- `P00` 을 `1 (OP1OUT)` 으로 설정하는 함수와 `0 (I/O)` 로 설정하는 함수가 섞여 있다. `P00` 은 회로상 `OP1_OUT` 테스트 포인트(C_VDD 출력이 아님 — 사용자 정정 내역 기반). 측정 후 P00 이 `OP1OUT` 으로 남아있어도 무방하나, `hw_initial` 의 기본값과 어긋날 수 있다.
- TEMP_ADC 가 P02, P03 을 `AN2, AN3` 으로 두지만, 다음에 dust 측정으로 진입하면서 다시 `OP1P, OP0OUT` 으로 되돌아오므로 순서상 안전. 단 함수 단위 “상태 보존” 원칙은 깨져 있다 → 향후 호출 순서 변경 시 회귀 위험.

권장 — 각 ADC 함수의 끝에 `hw_initial` 시의 기본 P0FSRL 로 복귀하는 1 행을 두면 캡슐화 가능.

---

## 12. OP-amp / LDO 설정 일관성

- `OPAMP_Initial(GAIN1_DIS, GAIN0_DIS, chp_125KHz)` — Dust_ADC_1AMP / Dust_ADC_Single : 두 OP-amp 모두 게인 비활성, OP0 만 사용
- `OPAMP_Initial(GAIN1_X30, ...)` / `GAIN1_X15` — Dust_ADC_2AMP : visual_type 에 따라 30 배 또는 15 배 게인 (OP1 사용)
- `OPAMP_Initial(GAIN1_X30, GAIN0_DIS, chp_125KHz)` — Dust_ADC_Dual : 항상 30 배
- TEMP_ADC / BAT_ADC : OP1/OP0 모두 `FALSE` (전류 절감)
- BAT_ADC_Avr : OPAMP_Initial 호출 자체가 없음 — 이전 함수의 OPAMP 설정이 그대로 유지됨

권장 — BAT_ADC_Avr 도 명시적으로 `OP0_Enable(FALSE); OP1_Enable(FALSE);` 를 호출 시작부에 두어, 어떤 컨텍스트에서 호출되어도 결정론적 결과가 되도록.

---

## 13. Settling delay 표준화 — 불일치 정리

| 함수 | LDO_ON 후 지연 | 다른 안정화 |
| --- | --- | --- |
| Dust_ADC_1AMP | 100 µs (5×20µs) | — |
| Dust_ADC_2AMP | 200 µs (10×20µs) | — |
| Dust_ADC_Single | 1 ms (Delay_ms(1)) | VBGR 선택 후 1 ms |
| Dust_ADC_Dual | 1 ms | — |
| TEMP_ADC | 200 µs (LDO 안정화) | — |
| BAT_ADC | — (LDO_ON 없음, INTERNAL_REF) | 200 µs |
| BAT_ADC_Avr | — | 200 µs |

LDO23 의 PSRR/turn-on 시간은 데이터시트 확인 필요(통상 100~500 µs). 5/10 배 차이는 측정 정확도/소비 전류 trade-off. **최소 500 µs 이상 통일** 을 권장 (소비 전류 영향은 측정 회당 µJ 단위로 무시 가능).

---

## 14. 종합 — 우선순위별 권고

### Critical

1. **`BAT_ADC_Avr` 버퍼 오버플로우 + 인덱스 불일치** (§6) — 로컬 배열 `adc_buf[ADC_BUFFER_SIZE]` 로 교체하고 `temp_data[buf_cnt/2]` 로 나눗셈.

### High

2. `adc_count / adc_buff / buff_cnt` 에 `volatile` 추가 (§2). 현재 미사용이지만 vendor 드라이버 차원의 안전.
3. `ADC_GetDataWithPolling` 에 bounded wait + `ADC_Timeout` 플래그 (§3) — SPI 와 동일한 패턴.

### Medium

4. AFLAG 명시 클리어 (§4) — 데이터시트 불일치 위험 방어.
5. `ADC_CLK_4M` 인자 6 곳을 `ADC_CLK_2M` 로 통일하고 헤더에서 `ADC_CLK_4M` deprecation 주석 (§1, §8).
6. `ADC_Initial` 종료부에 100 µs stabilization 또는 VBGR 채널 진입 후 100 µs 일관 적용 (§5).
7. `BAT_ADC_Avr` 진입부에 `OP0_Enable(FALSE); OP1_Enable(FALSE);` 명시 (§12).
8. LDO settling delay 를 함수별 500 µs 이상으로 통일 (§13).

### Low

9. `ADC_Int_Handler` 의 잉여 트리거 (§9) — `> 1` 로 변경. 현재 미사용 함수.
10. `Dust_ADC_1AMP` 의 두 번째 `ADC_Initial` 제거 (§7) — 중복.
11. 각 ADC 함수 끝에서 P0FSRL 을 hw_initial 기본값으로 복귀 (§11) — 캡슐화.

---

## 15. 빠른 grep 인덱스

```
# 호출부
grep -n "ADC_Initial\|ADC_SelectChannel\|ADC_GetDataWithPolling" src/main.c

# 드라이버
grep -n "ADCCRH\|ADCCRL\|ADCDR\|AFLAG" src/a96l41x_adc.c src/A96L41x.h

# 버퍼 정의 / 사용
grep -n "temp_data\|ADC_BUFFER_SIZE\|ADC_MAX_BUFFER_SIZE" src/main.c src/a96l41x_adc.h
```

---

## 16. ADC_Initial 자체에 대한 결론

`ADC_Initial(clock_sel, trigger_sel, ref_voltage, align_sel)` 의 동작 자체는 **호출만 봐서는 안전하지만 매크로 이름이 거짓말** 이라는 것이 핵심이다.

- ADCCRH 의 b3 (TRGSEL), b2 (ALIGN), b1:0 (CKSEL) 비트 위치는 데이터시트와 일치.
- ADCCRL 의 b7 (STBY/Enable), b5 (REFSEL) 비트 위치 일치.
- 단 `clock_sel == 0` 강제 변환으로 인해 사용자가 코드에서 본 ADC_CLK_4M 은 실제 ADC_CLK_2M 동작.
- ADC startup time / VBGR settling 은 caller 책임 — 현재 일부 함수만 1 ms 지연을 두고 나머지는 부족.
- ADC interrupt 미사용 — vendor 드라이버에 latent volatile/index 버그가 있지만 현재 동작에 영향 없음.

**가장 시급한 실 동작 영향은 §6 의 BAT_ADC_Avr 버퍼 오버플로우이며, 나머지는 견고성/유지보수성 개선 사항이다.**
