/*
 *  main.h - LTD-3700B firmware top-level declarations
 *  (Note: project lacks an include guard historically; one is added below.)
 */
#ifndef __MAIN_H_
#define __MAIN_H_

/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/


#define LED_G_ON			P06 = 0
#define LED_G_OFF		P06 = 1

#define LED_R_ON			P07 = 0
#define LED_R_OFF		P07 = 1

#define AUDIO_ON		P14 = 1
#define AUDIO_OFF 	P14 = 0

#define LDO_ON				LDOCR = 0x01
#define LDO_OFF			LDOCR = 0x00

#define CVDD_ON		P20 = 1
#define CVDD_OFF 	P20 = 0


//--------------------- Macro Definition --------------------------------------
#define Stop()          {PCON = 0x03; _nop_( ); _nop_( ); _nop_( );}
#define Idle()          {PCON = 0x01; _nop_( ); _nop_( ); _nop_( );}

//------------------- AMP/ADC Control Constants Definition -------------------
//#define CC_USE					// uncomment to enable constant-current sourcing on ICS pins

#define AMP_AUTO_DIS         0

#define SysClock_4MHZ         1


#ifdef SysClock_1MHZ
	#define T_USEC               (unsigned short)(500)                            // n[usec] at 1MHz
	#define TIME_4S              (unsigned short)(1000000/512*4)                  // 4sec
	#define TIME_2S              (unsigned short)(1000000/512*2)                  // 2sec
	#define TIME_1S              (unsigned short)(1000000/512)                    // 1sec
	#define TIME_500mS           (unsigned short)(1000000/512/2)                  // 0.5sec
#else
	#define T_USEC               (unsigned short)(500*4)                          // n[usec] at 4MHz
	#define TIME_4S              (unsigned short)(1000000/512*4*4)                // 4sec
	#define TIME_2S              (unsigned short)(1000000/512*4*2)                // 2sec
	#define TIME_1S              (unsigned short)(1000000/512*4)                  // 1sec
	#define TIME_500mS           (unsigned short)(1000000/512*2)                  // 0.5sec
#endif



#define CHK_T1IFR()          (T1CRL&(1<<4))
#define CLR_T1IFR()          T1CRL = (T1CRL&(~(1<<4)))
#define	CLR_T1CNT()			     (T1CRH=T1CRH|(1<<0))					                 // Clear Timer 1 Counter

#define START_DLY()          T0CRH = 0x81
#define STOP_DLY()           T0CRH = 0x01
#define CHK_T0IFR()          (T0CRL&(1<<4))
#define CLR_T0IFR()          T0CRL = (T0CRL&(~(1<<4)))

#define START_ADC()          ADCCRL = (ADCCRL|(1<<6))
#define ADC_ON()             ADCCRL = ADCCRL|(1<<7)
#define ADC_OFF()            ADCCRL = ADCCRL&(~(1<<7))
#define CHK_ADC()            (ADCCRL&(1<<4))


////////////////  Audio playback globals  ////////////////
// Audio Data : 8000 bps, 8 bit
// (FIX C2) Declarations must match definitions in main.c (now volatile).
// ORIG: extern uint16_t Audio_length;
// ORIG: extern uint16_t Audio_addr;
// ORIG: extern uint8_t  Audio_start;
// ORIG: extern uint8_t  Audio_start_address;
// ORIG: extern uint16_t Audio_max_length;
extern volatile uint16_t Audio_length;
extern volatile uint16_t Audio_addr;
extern volatile uint8_t  Audio_start;
extern volatile uint8_t  Audio_start_address;
extern volatile uint16_t Audio_max_length;


////////////////  System status  ////////////////
// (FIX C2) SystemStatus is shared between main and audio.c - volatile required.
// ORIG: extern uint8_t SystemStatus;
extern volatile uint8_t SystemStatus;


/* Private Function Prototype ------------------------------------------------*/


////////////////  Delay helpers  ////////////////
void Delay_ms(uint16_t msec);
void Delay_s(uint16_t sec);


////////////////  System init  ////////////////
void hw_initial(void);
void hw_initial_Wait(int msec);


////////////////  Watchdog / sleep helpers  ////////////////
void BeforeStop(void);
void AfterStop(void);
void WD_Reset(void);


////////////////  ADC helpers  ////////////////
uint16_t Data_Avr(uint16_t *adc_data, uint8_t count);
uint16_t Data_TrimmedMean(uint16_t *adc_data, uint8_t count);
void     Data_Sorting(uint16_t *adc_data, uint8_t count);
uint16_t Check_System(void);
uint16_t TEMP_ADC(void);
uint16_t BAT_ADC(void);
uint16_t Get_Bat_Voltage_cV(void);   // unit: 0.01 V
uint16_t Dust_ADC_1AMP(void);
void     Set_Temp_Table(void);


////////////////  UART helpers  ////////////////
void Uart_Out(void);
void Uart_Out_Int(uint16_t Value);

////////////////  Audio helpers  ////////////////
void Variable_Initial(uint16_t Audio_st_address);
void Audio_Run(uint16_t Address , uint16_t Length,  uint8_t Run_time);
void Fire_Alarm(void);
void T2_init(void);

#endif /* __MAIN_H_ */
