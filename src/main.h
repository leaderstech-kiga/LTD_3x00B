

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
//#define CC_USE					// if constant current use disable comment line
	
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


////////////////Start  오디오 변수//////////// 
	
// Audio Data : 8000bps , 8bit 

extern uint16_t Audio_length;
extern uint16_t Audio_addr;
extern uint8_t Audio_start;
extern uint8_t Audio_start_address;
extern uint16_t Audio_max_length;

////////////////End  오디오 변수//////////// 


////////////////Start  시스템 상태 함수//////////// 

extern uint8_t SystemStatus;


////////////////End  시스템 상태 함수//////////// 

/* Private Function Prototype ------------------------------------------------*/
