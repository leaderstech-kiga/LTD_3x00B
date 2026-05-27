

/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/

/*
#define LED_G_ON			P06 = 1
#define LED_G_OFF		P06 = 0

#define LED_R_ON			P07 = 1
#define LED_R_OFF		P07 = 0

#define AUDIO_ON	P14 = 0
#define AUDIO_OFF 	P14 = 1
*/


#define LED_G_ON			P06 = 0
#define LED_G_OFF		P06 = 1

#define LED_R_ON			P07 = 0
#define LED_R_OFF		P07 = 1

#define AUDIO_ON		P14 = 1
#define AUDIO_OFF 	P14 = 0

#define  BAT_PW_ON			P13 = 1
#define  BAT_PW_OFF		P13 = 0

#define LDO_ON				LDOCR = 0x01
#define LDO_OFF				LDOCR = 0x00

#define CVDD_ON		P20 = 1
#define CVDD_OFF 	P20 = 0


//--------------------- Macro Definition --------------------------------------
#define Stop()          {PCON = 0x03; _nop_( ); _nop_( ); _nop_( );}
#define Idle()          {PCON = 0x01; _nop_( ); _nop_( ); _nop_( );}


//------------------- AMP/ADC Control Constants Definition -------------------
//#define CC_USE					// if constant current use disable comment line
#define DELAY_TIME			
#define AMP_AUTO_DIS         0

#ifdef SysClock_1MHZ		
	#define T_USEC               (unsigned short)(500)                        // n[usec] at 1MHz
	#define TIME_4S              (unsigned short)(1000000/512*4)                 // 4sec
	#define TIME_2S              (unsigned short)(1000000/512*2)                 // 2sec
	#define TIME_1S              (unsigned short)(1000000/512)                 // 1sec
	#define TIME_500mS           (unsigned short)(1000000/512/2)                  // 0.5sec
#else
	#define T_USEC               (unsigned short)(500*4)                        // n[usec] at 4MHz
	#define TIME_4S              (unsigned short)(1000000/512*4*4)                 // 4sec
	#define TIME_2S              (unsigned short)(1000000/512*4*2)                 // 2sec
	#define TIME_1S              (unsigned short)(1000000/512*4)                 // 1sec
	#define TIME_500mS           (unsigned short)(1000000/512*2)                  // 0.5sec	
#endif
	
#define CHK_T1IFR()          (T1CRL&(1<<4))
#define CLR_T1IFR()          T1CRL = (T1CRL&(~(1<<4)))
#define	CLR_T1CNT()			     (T1CRH=T1CRH|(1<<0))					                 //Clear Timer 1 Counter

#define START_DLY()          T0CRH = 0x81
#define STOP_DLY()           T0CRH = 0x01
#define CHK_T0IFR()          (T0CRL&(1<<4))
#define CLR_T0IFR()          T0CRL = (T0CRL&(~(1<<4)))

#define START_ADC()          ADCCRL = (ADCCRL|(1<<6))
#define ADC_ON()             ADCCRL = ADCCRL|(1<<7)
#define ADC_OFF()            ADCCRL = ADCCRL&(~(1<<7))
#define CHK_ADC()            (ADCCRL&(1<<4))	

#define ADC_AMP0	1
#define ADC_Temp	2
#define ADC_BAT			3	
	
#define Dust_mode		1
#define Temp_mode	2

#define Normal_mode	1
#define UART_mode		2
#define In_Fire_al_mode		3
#define Ex_Fire_al_mode		4
#define Al_Stop_mode		6


#define Bat_High_mode		1
#define Bat_Low_mode		2
	
// Audio Data : 8000bps , 8bit 

extern uint16_t Audio_length;
extern uint16_t Audio_addr;
extern uint8_t Audio_start;
extern uint8_t Audio_start_address;
extern uint16_t Audio_max_length;
extern uint8_t SystemStatus;

extern uint8_t Timer2_cnt;

/* Private Function Prototype ------------------------------------------------*/

void Variable_Initial(uint16_t Audio_st_address);
void Delay_s(uint16_t sec);
void hw_initial(void) ;
void hw_audio_initial(void);
void BeforeStop(void);
void AfterStop(void);
void WD_Reset(void);
void Audio_Init(void);
uint16_t Adc_Read(uint8_t Port);
uint16_t Dust_ADC_1AMP(uint8_t buf_cnt);
uint16_t Dust_ADC_2AMP(uint8_t buf_cnt);
uint16_t TEMP_ADC(void);
void Fire_Alarm(void);
uint16_t BAT_ADC(void);
uint16_t Visu_MODE_CK(void);
void Audio_Run(uint16_t Address , uint16_t Length,  uint8_t Run_time);
void T2_init(void);
void LowBat_Alarm(void);
void ADC_Uart_Out(uint16_t Dust_data);
void det_adc(void);
void Uart_Out_avr(int avr_val);
void Uart_Out(char mode);
void Data_Sorting(uint16_t *adc_data, uint8_t count);
void Uart_Set(void);

void Delay_ms(uint16_t msec);
void Enable_WD(void);
void hw_initial_Wait(int msec);
uint8_t SPI_Memory_Check(void);
void Check_Uart_Mode(uint8_t start_uart_debug);
void Uart_Test_Out(void);
uint16_t BAT_ADC_Avr(uint8_t buf_cnt);
void Dust_ADC_Single(uint8_t buf_cnt);
void Dust_ADC_Dual(uint8_t buf_cnt);
void Bat_Ck(void);
uint16_t Data_Avr(uint16_t *adc_data, uint8_t count);