

/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/


//////////////// Start: System state and mode constants ////////////////

/* Side-effect macros wrapped in ((void)(...)) so 'LED_G_ON;' is a valid
 * statement but 'if (LED_G_ON)' produces a compile error instead of
 * silently writing to the port and evaluating to the assigned value. */
#define LED_G_ON         ((void)(P06 = 0))
#define LED_G_OFF        ((void)(P06 = 1))

#define LED_R_ON         ((void)(P07 = 0))
#define LED_R_OFF        ((void)(P07 = 1))

#define AUDIO_ON         ((void)(P14 = 1))
#define AUDIO_OFF        ((void)(P14 = 0))

#define CVDD_ON          ((void)(P20 = 1))
#define CVDD_OFF         ((void)(P20 = 0))

#define SLAVESELECT      ((void)(P13 = 0))
#define SLAVEDESELECT    ((void)(P13 = 1))
	
#define LDO_ON				LDOCR = 0x01
#define LDO_OFF				LDOCR = 0x00

//--------------------- Macro Definition --------------------------------------
#define Stop()          {PCON = 0x03; _nop_( ); _nop_( ); _nop_( );}

/* ADC operating mode flag (variable ADC_mode). */
#define Dust_mode           1
#define Temp_mode           2

/* SYS_mode flag - main-loop state machine. */
#define Normal_mode         1
#define In_Fire_al_mode     3
#define Al_Stop_mode        6

/* Bat_mode flag set by Bat_Ck based on battery ADC reading. */
#define Bat_High_mode       1
#define Bat_Low_mode        2

#define uart_debug_On      1           
#define uart_debug_Off      0
extern uint8_t  uart_debug_mode;
	

extern volatile uint8_t  Timer2_cnt;

/* T_USEC and TIME_2S are used by hw_initial / hw_audio_initial to program
 * Timer0 (PWM) and Timer1 (audio sample tick). */
#ifdef SysClock_1MHZ
	#define T_USEC               (unsigned short)(500)                          // n[usec] at 1MHz
	#define TIME_2S              (unsigned short)(1000000/512*2)                // 2sec
#else
	#define T_USEC               (unsigned short)(500*4)                        // n[usec] at 4MHz
	#define TIME_2S              (unsigned short)(1000000/512*4*2)              // 2sec
#endif

	

//////////////// End:   System state and mode constants ////////////////


//////////////// Start: Audio playback globals ////////////////

/* OPAMP auto-disable mode for AMPCR0.
 *   0 = "Always" mode: OPAMP enable bits (AMPCR1) stay set across ADC
 *       conversions. Required for multi-sample OPAMP+ADC measurements
 *       like Dust_ADC_1AMP that read OP0OUT multiple times.
 *   1 = "Auto disable after ADC" mode: AMPCR1 enable bits are
 *       auto-cleared by hardware after every ADC conversion. Breaks
 *       multi-shot OPAMP measurements (second read returns 0).
 *
 * Sleep current is kept low by BeforeStop() writing AMPCR1=0 explicitly
 * just before Stop entry, plus each ADC function cleans AMPCR1 at exit.
 * This makes AMP_AUTO_DIS=0 safe to use here. */
#define AMP_AUTO_DIS         0

/* Audio clip table descriptor.
 * Use Play_Clip(AUDIO_CLIP_xxx) to play; see Audio_Clips[] in main.c
 * for the actual address/length values. */
typedef struct {
	uint16_t address;     /* SPI flash byte offset where ADPCM data starts */
	uint16_t length;      /* sample count (8 kHz, 8-bit ADPCM)             */
	uint8_t  runtime;     /* repeat count                                  */
} AudioClip_t;

#define AUDIO_CLIP_BOOT      0    /* startup beep   (~1.35 s) */
#define AUDIO_CLIP_LOWBAT    1    /* low-battery    (~2.68 s) */
#define AUDIO_CLIP_FIRE      2    /* fire alarm     (~3.04 s) */
#define AUDIO_CLIP_COUNT     3

extern code AudioClip_t Audio_Clips[AUDIO_CLIP_COUNT];

// Audio Data : 8000bps , 8bit
// volatile - shared between TIMER1_Int / TIMER2_Int (audio.c, main.c) and
// main-thread code. Prevents Keil C51 from caching the values in registers.
extern volatile uint16_t Audio_length;
extern volatile uint16_t Audio_addr;
extern volatile uint8_t  Audio_start;
extern volatile uint16_t Audio_max_length;
extern volatile uint8_t  SystemStatus;

//////////////// End:   Audio playback globals ////////////////



/* Private Function Prototype ------------------------------------------------*/

//////////////// Start: System initialization helpers ////////////////
void hw_initial(void);
void Delay_ms(uint16_t msec);
void hw_initial_Wait(int msec);
uint8_t SPI_Memory_Check(void);
void T2_init(void);
void TIMER2_Int(void);
void Start_Debug_Mode(void);
//////////////// End: System initialization helpers ////////////////

//////////////// Start: Watchdog / sleep helpers ////////////////
void BeforeStop(void);
void AfterStop(void);
void WD_Reset(void);
void WDT_Int(void);
//////////////// End:   Watchdog / sleep helpers ////////////////

//////////////// Start: Audio helpers ////////////////
void Variable_Initial(uint16_t Audio_st_address);
void Audio_Run(uint16_t Address , uint16_t Length,  uint8_t Run_time);
void Play_Clip(uint8_t clip_id);
//////////////// End: Audio helpers ////////////////


//////////////// Start: UART helpers ////////////////

extern uint8_t Flash_Read_OK[17];
extern uint8_t Flash_Read_Fail[19];

void Uart_Out(void);
void Uart_Out_Fire(uint16_t Value);
void Uart_Out_Int(uint16_t Value);
void Uart_Out_Text(uint8_t *send_data, uint8_t count);

//////////////// End:   UART helpers ////////////////

//////////////// Start: ADC helpers ////////////////

uint16_t Data_Avr(uint16_t *adc_data, uint8_t count);
uint16_t Check_System(void);
uint16_t TEMP_ADC(void);
uint16_t Get_Bat_Voltage_cV(void);
uint16_t Dust_ADC_1AMP(void);
void Set_Temp_Table(void);

/**********************************************************************
 * @brief		Dust_1ADC
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t Dust_ADC_1AMP(void);
uint16_t Dust_ADC_2AMP(void);
//////////////// End:   ADC helpers ////////////////
