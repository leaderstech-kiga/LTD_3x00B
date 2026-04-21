/***************************************************************************//**
*   @file       Main.c
*   @brief      A96L41x Main  function file 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       29. MAY. 2020
*
* Copyright(C) 2019, ABOV Semiconductor
* All rights reserved.
*
*   @mainpage   Project : A96L41x (20TSSOP) 
*   - Introduction: This is a guide program for using A96L41x .
*
*   @section    Compiler Version
*   - uVision C51 Compiler ( Version 5.13 )
*
*   @section    Configure Option
*   - Code Read Protection :                Disable
*   - Code Write Protection :               Disable
*   - Vector Area (00H~FFH) Protection :    Disable
*   - Select RESETB pin :                   Disable
*
*   @section    Clock Setting
*   - HFIRC OSC/1 (Internal 4Mhz) 
*
*   @section    Project Program Version : V1.00
*   @section    Modify Infomation
*   - V1.00 :    First Start.    
*
*   @section    Caution
*   - Nothing special.
*
*   @section Common
*   - ABOV Semiconductor Co.,Ltd.
*   - URL : <http://www.abovsemi.com>
*
********************************************************************************
* ABOV Disclaimer
*
*IMPORTANT NOTICE - PLEASE READ CAREFULLY
*ABOV Semiconductor ("ABOV") reserves the right to make changes, corrections, enhancements, 
*modifications, and improvements to ABOV products and/or to this document at any time without notice. 
*ABOV does not give warranties as to the accuracy or completeness of the information included herein.
*Purchasers should obtain the latest relevant information of ABOV products before placing orders. 
*Purchasers are entirely responsible for the choice, selection, and use of ABOV products and 
*ABOV assumes no liability for application assistance or the design of purchasers' products. No license, 
*express or implied, to any intellectual property rights is granted by ABOV herein. 
*ABOV disclaims all express and implied warranties and shall not be responsible or
*liable for any injuries or damages related to use of ABOV products in such unauthorized applications. 
*ABOV and the ABOV logo are trademarks of ABOV.
*All other product or service names are the property of their respective owners. 
*Information in this document supersedes and replaces the information previously
*supplied in any former versions of this document.
*2020 ABOV Semiconductor  All rights reserved
*
*/



/* Includes ------------------------------------------------------------------*/
#include    "Intrins.h"
#include    "delay.h"     //
#include "a96l41x_gpio.h"
#include "a96l41x_clock.h"

#include "a96l41x_adc.h"
#include "a96l41x_opamp.h"
#include "a96l41x_usart.h"

#include "a96l41x_usart_spi.h"
#include "a96l41x_timer0.h"
#include "a96l41x_timer1.h"
#include "a96l41x_timer2.h"


#include "main.h"
#include "audio.h"
#include "flash.h"


/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////Start  오디오 변수//////////// 
// Audio Data : 8000bps , 8bit 
uint16_t Audio_length;
uint16_t Audio_addr;
uint8_t Audio_start_address;
uint16_t Audio_max_length;
/* [BUG-06 FIX] Duplicate declaration removed - SystemStatus already declared at line 168 */
/* BEFORE: uint8_t SystemStatus; */
/* uint8_t SystemStatus; */

uint8_t EM_AL_Status = 0;
uint8_t Audio_start =0;

uint16_t  Audio1_Address  = 0x0;
uint16_t  Audio1_length  = 10801;
uint8_t  Audio1_runtime  = 1;
uint16_t  Audio2_Address  = 0x2baa;
uint16_t  Audio2_length  = 21478;
uint8_t  Audio2_runtime  = 1;
uint16_t  Audio3_Address  = 0x8108;
uint16_t  Audio3_length  = 24324;
uint8_t  Audio3_runtime  = 1;
////////////////End  오디오 변수//////////// 

////////////////Start  Debug용 uart text//////////// 
uint8_t Dust_MODE[10] = "Dust_MODE:";
uint8_t Visu_MODE[10] = "Visu_MODE:";
uint8_t Temp_MODE[10] = "Temp_MODE:";

uint8_t ADC[4] = "ADC:";
uint8_t CHR[4] = "CHR:";
uint8_t INF[4] = "INF:";

uint8_t Dust[5] = "Dust:";
uint8_t Dust_ori[7] = "Dust_O:";
uint8_t Dust_avr[7] = "Dust_A:";
uint8_t Temp[5] = "Temp:";
uint8_t Bat[4] = "Bat:";
uint8_t Tab[4] = "    ";
uint8_t End[4] = "\n\r";
uint8_t int_val ;
uint16_t tmp ;

uint8_t opamp1[5] = "OPA1:";
uint8_t opamp2[5] = "OPA2:";

uint8_t Avr[9] = "Average: ";

uint8_t Dust_Si_Off[7] = "Si_Off:";
uint8_t Dust_Si_On[6] = "Si_On:";
uint8_t Dust_Si_Deta[6] = "Si_Da:";

uint8_t Dust_Du_Off[7] = "Du_Off:";
uint8_t Dust_Du_On[6] = "Du_On:";
uint8_t Dust_Du_Deta[6] = "Si_Da:";
////////////////End  Debug용 uart text//////////// 

////////////////Start  시스템 상태 변수 //////////// 
uint8_t deviceID[3];
uint8_t SystemStatus;

uint8_t  SYS_mode , ADC_mode , Bat_mode;

#define Dust_mode		1
#define Temp_mode	2

#define Normal_mode	1
#define UART_mode		2
#define In_Fire_al_mode		3
#define Ex_Fire_al_mode		4
#define Al_Stop_mode		6


#define Bat_High_mode		1
#define Bat_Low_mode		2

uint8_t visual_type = 0;	//시각형 type


uint16_t system_count = 0;
uint16_t adc_time	= 8 * 5;	// adc time	, 0.192초 주기
uint16_t ck_led_time	= 56 * 5  + 12;	// 8 * 7 초 , , 0.192초 주기 > 0.192 * 5* 56  = 53.7  -> 2.3초 모자름 : 0.192 * 12 더해줌
uint16_t Bat_al_time	= 280  * 5  + 58 ;	// 280 초 , , 0.192초 주기  -> 0.192 * 5* 280  = 268.8  -> 11.2초 모자름 : 0.192 * 58 더해줌  
////////////////End  시스템 상태 변수//////////// 

////////////////Start  ADC 변수 //////////// 

#define  ADC_BUFFER_SIZE 8
uint16_t ADC_temp_data[ADC_BUFFER_SIZE];

uint16_t ADC1_Bat_Val;
uint16_t ADC1_On_Dust_Val;
uint16_t ADC1_Off_Dust_Val;

////////////////End  ADC 변수 //////////// 


////////////////Start  debug 변수 //////////// 
//초기 디버그 진입 모드 선택

#define Debug_On	1
#define Debug_Off	0

uint8_t  uart_debug_mode = Debug_Off ;	// 연기 감지시  uart출력 설정 , 양산시 미적용
uint8_t  start_uart_debug_mode = Debug_On ;	//전원 인가시 연동 버튼을 누르면 uart 모드로 adc 출력하는 모드 ,  양산시 미적용
//uint8_t  start_uart_debug = 0 ;

////////////////End  debug 변수 //////////// 

/* Public Variable -----------------------------------------------------------*/
/* Public Function -----------------------------------------------------------*/

/**********************************************************************
 * @brief		Main program
 * @param   	None
 * @return	    None
 **********************************************************************/
void Main(void)
{
	

	LDO_OFF;
	FLASH_OFF;
	AUDIO_OFF;
	system_count = 0;
	
	hw_initial_Wait(10);
	
	LED_G_ON;
	Delay_ms(5);		
	LED_G_OFF;
	
	LED_R_ON;
	Delay_ms(5);		
	LED_R_OFF;	
	
	
	Port_SetInputpin(PORT1, PIN2, 1);  //key input & mode0  -> button input , 초기 눌려 있으면 UART mode
	Delay_ms(10);		
	
	ADC_mode = Temp_mode;
	
		/* 시작시 uart 출력 모드  */
	if(start_uart_debug_mode == Debug_On){
		
		if(Port_GetInputpinValue(PORT1, PIN2)  == 0){
				SYS_mode = UART_mode;
			}
			else{
				SYS_mode = Normal_mode;
			}
			
			LED_G_ON;
			Delay_ms(5);	
			LED_G_OFF;
			
			// 연동 key 풀릴때까지 대기 
			if(SYS_mode == UART_mode){
					while(1){
						LED_G_ON;
						if(Port_GetInputpinValue(PORT1, PIN2)  == 1){
							break;
						}
					}
					
					LED_G_OFF;
					
					//UART mode 동작 
			//		Uart_Set();
					
					while(1){
		
						LED_R_ON;
					
						Uart_Out();
						
						Delay_ms(250);	
						LED_R_OFF;
						Delay_ms(250);	
					}

		}
	}
	
	while(1)
	{
		
		
		if(system_count%adc_time == 0){
			
			hw_initial_Wait(1);
			
			LED_G_ON;
			
			Delay_ms(5);
			
		  LED_G_OFF;
			
			hw_initial_Wait(1);
			
			system_count = 0;
		}
		
		system_count++;

		
		// 와치독 타이머 
		BeforeStop();
		GLOBAL_INTERRUPT_EN();  
		Stop();
		AfterStop();
			
	}

}

////////////////Start  딜레이 변수 //////////// 

/**********************************************************************
 * @brief		Delay_ms
 * @param   	None
 * @return	None
 **********************************************************************/
void Delay_ms(uint16_t msec)
{

	int i;
	
	for(i=0;i<msec;i++){
		NOP_20us_Delay(50);
	}

}

/**********************************************************************
 * @brief		Delay_s
 * @param   	None
 * @return	None
 **********************************************************************/
void Delay_s(uint16_t sec)
{

	int i;
	
	for(i=0;i<sec;i++){
		NOP_20us_Delay(50000);
	}

}

////////////////End  딜레이 변수 //////////// 

	
////////////////Start  시스템 초기화  변수 //////////// 

/**********************************************************************
 * @brief		hw_initial_Wait
 * @param   	None
 * @return	None
 **********************************************************************/
// 하드웨어를 초가화 한 후 ms 대기한다. 
void hw_initial_Wait(int msec){
	
	GLOBAL_INTERRUPT_DIS();     
	hw_initial();
	
	CVDD_OFF;
	Port_SetOutputpin(PORT1, PIN3, 0); // CS 핀

	Delay_ms(msec);
	
		/* Enable INT */
	GLOBAL_INTERRUPT_EN();  
	
}


void hw_initial(void) {
	
	// hw_initial() 호출 전 오디오 완전 정지
	if(Audio_start == 1) {
			SLAVEDESELECT;      // CS 닫기
			AUDIO_OFF;          // 앰프 끄기
			Timer0_Stop();      // PWM 정지
			Audio_start = 0;
			Audio_length = 0;
	}

	BITCR = 0x60;                                              // BIT cnt cleared, Select 0Bit overflow period
	
#ifdef SysClock_1MHZ	
	OSCCR = 0x08;                                            // System clock: 1MHz
#else	
	OSCCR = 0x18;                                            // System clock: 4MHz
	                                                        // If 0x00/0x08/0x10/0x18, 0.5MHz/1MHz/2MHz/4MHz
#endif

	RSTFR = 0x00;                                            // Clear all reset flags

	
	// Port 0
	P0 = 0xC0;											// 0 : Low,	1 : High
	P0OD = 0x00; 								// 0 : Disable,	1 : Enable (Open-drain)
	P0PU = 0X00;                 // 0 : Disable,	1 : Enable (Pull-up)
	P0IO = 0xc1;                  // 0 : Input,	1 : Output
	//P0FSRH = 0x05;                                                          // P[7]: normal I/O, P[5:0]: Amp
	//P0FSRL = 0x55;
	P0FSRH  = 0
		| ( 0 << 6)       //P07     // 0 : I/O (EINT3), 1 : ICS1									// LED_R				
		| ( 0 << 4)       //P06     // 0 : I/O (EINT2), 1 : ICS0									// LED_G
		| ( 1 << 2)       //P05     // 0 : I/O, 1 : OP0P,    										// SENISNG OP0P
		| ( 1 << 0);      //P04     // 0 : I/O, 1 : OP0N													// SENSING OP0N
		
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 1 << 2)       //P01     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// Not use
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
		
	P0DB	= 0
		| ( 0 << 6) 	  //DBCLK[1:0] 	// 00 : fx (SCLK), 	01 : fx/4,	10 : fx/16,	11 : fx/64
		| ( 0 << 3) 	  //P07DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 2) 	  //P06DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 1) 	  //P10DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 0);	  //P00DB 	// 0 : Disable, 1 : Enable
	                                         // Low level

	// Port 1
	
	P1 = 0x00; 											// 0 : Low,	1 : High								// audio off
	P1OD = 0x00;									// 0 : Disable,	1 : Enable (Open-drain)
	P1PU = 0x04;               		// 0 : Disable,	1 : Enable (Pull-up) // 테스트 스위치 풀업 필요
	//P1PU = 0x0;               		// 0 : Disable,	1 : Enable (Pull-up)
	P1IO = 0x32;                 	// 0 : Input,	1 : Output
	

	//P12DB = 0x00;											//P1/P2 Debounce disable
	//P1FSRH = 0x08;                                                          // P1[5]: AN6, others: normal I/O
	//P1FSRL = 0x00;                                                          // P1[3:0]: Normal I/O
	P1FSRH  = 0
		| ( 1 << 2)       //P15  // 0 : I/O,	1 : LDO23,   2 : AN6,	3 : (SDA)						// LDO23
		| ( 0 << 0);      //P14  // 0 : I/O (EINT11),	1 : T1O/PWM1O,   2 : SCL,	3 : Not used		// /AUD ON
		
		P1FSRL  = 0
		| ( 0 << 6)       //P13  // 0 : I/O (EC1),	1 : (RXD/MISO)							// DSDA
		| ( 0 << 4)       //P12  // 0 : I/O (EC0),	1 : (TXD/MOSI)								// DSCL
		| ( 1 << 2)       //P11  // 0 : I/O (EINT10),	1 : T0O/PWM0O							// AUDIO SIG
		| ( 0 << 0);      //P10  // 0 : I/O, 	1 : (SCK)										// SENSE PW

		P12DB	= 0
		| ( 0 << 5) 	  //P21DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 4) 	  //P20DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 2) 	  //P14DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 1) 	  //P11DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 0);      //P10DB 	// 0 : Disable, 1 : Enable
		
	
    
	// Port 2
	P2 = 0x00;											  // 0 : Low,	1 : High
	P2OD = 0x00;										// 0 : Disable,	1 : Enable (Open-drain)
	P2PU = 0x00;										// 0 : Disable,	1 : Enable (Pull-up)
	P2IO = 0x0f;											// 0 : Input,	1 : Output
	//P2FSR = 0x00;										   // P2[3:0]: I/O
	P2FSR   = 0
		| ( 1 << 6) 		// 0 : P23,  1 : TXD/MOSI,	2 : AN5,     3 : Not used					//mosi
		| ( 1 << 4) 		// 0 : P22,  1 : RXD/MISO,	2 : AN4,     3 : Not used					// miso
		| ( 1 << 2)         // 0 : P21 (EC2),  1 : SCK,   2 : (SCL),     3 : Not used						// sck
		| ( 0 << 0);        // 0 : P20 (EINT12),  1 : T2O/PWM2O,   2 : SDA,     3 : Not used			// cs 



	
		// Timer 0 
	T0CRL = 0x80;                                                           // Timer 0: fx/4
	T0CRH = 0x01;                                                           // Timer 0: disable, clear counter
	T0ADRH = (unsigned char)(T_USEC>>8);
	T0ADRL = (unsigned char)(T_USEC&0xff);
	// Timer 1
	T1CRL = 0x00
		  |(1<<5);                                                          // Timer 1: if 0/1/2.../7, fx divided by 2048/512/64/8/4/2/1/EC1
	T1CRH = 0x81;                                                           // Timer 1: if 0/8, Dis/Enable, clear counter
	//T1CRH = 0x01;                                                           // Timer 1: if 0/8, Dis/Enable, clear counter
	T1ADRH = (unsigned char)(TIME_2S>>8);
	T1ADRL = (unsigned char)(TIME_2S&0xff);

	
	// Timer 2
	T2CRL = 0x00;                                            // Timer 2: fx/1
	T2CRH = 0x00;                                            // Timer 2: Disable, Clear counter

  // WDT
	WDTCR = 0x00
			|(0<<7)                                            // If 0/1, Diable/Enable WDT operation
			|(0<<6)                                            // If 0/1, "Free running 8-bit timer"/"Reset by WDT"
			|(1<<5)                                            // If 0/1, "No effect"/"Clear WDT Counter"
			|(0<<1)                                            // If 0/1/2, Select WDT Clock as "BIT Overflow"/ LFIRC /WDTRC
			|(0<<0);                                           // Clear WDTIFR
  
	// LVR
	LVRIDR = 0x59;                                           // Id value to write data to the LVRCR register
	LVRCR = 0x01;                                            // LVR Disable
 
	// Amp
	AMPCR0 = 0x00
			|(0<<6)                                                         // If 0/1, Disable/Enable filter OP1OUT for ADC
			|(0<<4)                                                         // If 0/1/2/3, Select No/AMP0/AMP1/"AMP0/1" discharge channel
			|(0<<2)                                                         // If 0/1/2/3, Select discharge time during Disable/100us/200us/300us
			|(AMP_AUTO_DIS<<1)                                              // If 0/1, AMP1 Always/"Auto disable after ADC"
			|(AMP_AUTO_DIS<<0);                                             // If 0/1, AMP0 Always/"Auto disable after ADC"
			
	AMPCR1 = 0x00
			|(0<<7)                                                       // If 0/1, Disable/Enable Amp1
			|(0<<4)                                                       // If 0/1/2/3/4/5/6/7, "Disable gain"/x1/x2/x10/x15/x20/x30/x60
			|(0<<3)                                                       // If 0/1, Disable/Enable Amp0
			|(0<<0);                                                      // If 0/1/2/3, "Disable gain"/x5/x10/x20
			
	CHPCR = (0<<0);                                                         // If 0/1/2/3, 125/167/250/500 [kHz] chopper clock
	// ADC
	ADCCRH = 0x00
			|(0<<7)                                                         // Clear ADCIFR
			|(0<<3)                                                         // If 0/1, ADC Trigger Signal: ADST/"Timer 0"
			|(0<<2)                                                         // If 0/1, ADC Data Align: MSB/LSB . 0 = MSB
			|(0<<0);                                                        // If 0/1/2/3, 1/0.5/0.25/0.125 [MHz]
	ADCCRL = 0x00
			|(0<<7)                                                         // If 0/1, Disable/Enable ADC Module
			|(0<<6)                                                         // If 0/1, No/Start
			|(0<<5)                                                         // If 0/1, Reference VDD/AVREF
			|(0x6<<0);                                                      // sel. AN6
			//|(0xA<<0);                                                      // If 0/1////, AN0/AN1/////AMP1
	// USART
	USTBD = (unsigned char)(4000000/9600/16 - 1);            // 4MHz/(Baud Rate)/16 - 1
	USTCR1 = 0x00
			|(0<<6)                                           // If 0/1/2/3, UART/Synchronous/Reserved/SPI Mode
			|(0<<4)                                           // If 0/1/2/3, No/Reserved/Even/Odd Parity
			|(3<<1)                                           // If 0/1/2/3/7, 5/6/7/8/9-bit Data
			|(0<<0);                                          // If 0/1, TxD change @Rising/@Falling only SPI mode
	USTCR2 = 0x00
			|(0<<7)                                           // If 0/1, Disable/Enable DRE Interrupt
			|(0<<6)                                           // If 0/1, Disable/Enable TXC Interrupt
			|(0<<5)                                           // If 0/1, Disable/Enable RXC Interrupt
			|(0<<4)                                           // If 0/1, Disable/Enable WAKE Interrupt
			|(0<<3)                                           // If 0/1, Disable/Enable Transmitter
			|(0<<2)                                           // If 0/1, Disable/Enable Receiver
			|(0<<1)                                           // If 0/1, Disable/Enable USART Block
			|(0<<0);                                          // If 0/1, Normal/Double Speed Operation
	USTCR3 = 0x00
			|(0<<7)                                           // If 0/1, Slave/Master in SPI or Synchronous mode
			|(0<<6)                                           // If 0/1, Normal/"Loop back" Operation
			|(0<<5)                                           // When synchronous mode only
			|(0<<4)                                           // If 0/1, Disable/Enable SS Pin
			|(0<<3)                                           // If 0/1, No/Exchange MOSI and MISO
			|(0<<2);                                          // If 0/1, 1/2 Stop bit

	// ICSx
	#ifdef CC_USE

	ICSCR = 0x00;                                            // If 0x00/0x05/0x0a, Disable/"Enable ICS0"/"Enable ICS1"
	ICSDR0 = 0x06;                                           // If 0/1/.../15, abot 50 + 15 x data [mA]
	ICSDR1 = 0x06;                                           // If 0/1/.../15, abot 50 + 15 x data [mA]
	P0FSRH |= 0xF0;											// P07 function set to ICS1, P06 function set to ICS0
	#else
	ICSCR = 0x00;                                            // If 0x00/0x05/0x0a, Disable/"Enable ICS0"/"Enable ICS1"
	ICSDR0 = 0x00;                                           // If 0/1/.../15, abot 50 + 15 x data [mA]
	ICSDR1 = 0x00;                                           // If 0/1/.../15, abot 50 + 15 x data [mA]
	
	#endif
	// External Interrupt
	EIPOL0 = 0x00                                            // If 0x00/0x01/0x02/0x03, Disable/Rising/Falling/Both
			|(0x00<<6)                                        // EINT3
			|(0x00<<4)                                        // EINT2
			|(0x00<<2)                                        // EINT1
			|(0x00<<0);                                       // EINT0
	EIPOL1 = 0x00                                            // If 0x00/0x01/0x02/0x03, Disable/Rising/Falling/Both
			|(0x00<<4)										  // EINT12
			|(0x00<<2)                                        // EINT11
			|(0x00<<0);                                       // EINT10
	EIFLAG = 0x00;                                           // Clear all external interrupt flags

	// Initialize Interrupt Registers
	IP = 0x00;				                                 // Priority level 0
	IP1 = 0x00;				                                 // Priority level 0
	IE = 0x00
		|(0<<5)                                              // EINT11
		|(0<<4)                                              // EINT10
		|(0<<3)                                              // EINT3
		|(0<<2)                                              // EINT2
		|(0<<1)                                              // EINT1
		|(0<<0);                                             // EINT0
	IE1 = 0x00
		|(0<<5)                                              // EINT12 Interrupt
		|(0<<1)                                              // I2C Interrupt
		|(0<<0);                                             // ADC Interrupt
	IE2 = 0x00
		|(0<<5)                                              // USART Tx Interrupt
		|(0<<4)                                              // USART Rx Interrupt
		|(0<<2)                                              // Timer 2 Interrupt
		|(0<<1)                                              // Timer 1 Interrupt
		|(0<<0);                                             // Timer 0 Interrupt
	IE3 = 0x00
		|(0<<4)                                              // BIT Interrupt
		|(0<<3);                                             // WDT Interrupt

}


////////////////End  시스템 초기화  함수 //////////// 

////////////////Start  와치독  함수 //////////// 

void BeforeStop(void)
{
	
	OSCCR = 0x80;
	WDTCR = 0xA2;
	
	WDTDR = 2;							// entrance to every 0.192sec
	//WDTDR = 5;							// entrance to every 0.384sec
	//WDTDR = 7;							// entrance to every 0.512sec
	//WDTDR = 124;							// entrance to every 8sec
	
	//WDTCR = 0xA3;			// set WDT enable , clock source LFIRC, WDT interrupt
	//WDTCR = 0xA4;				// set WDT enable , clock source WDTRC
	
	IE3 |= 0x08;							// WDT interrupt enable	
	
}

void AfterStop(void)
{

	IE3 &= ~(0x08);											// WDT interrupt disable
	//WDTCR = 0x00;
	
	
#ifdef SysClock_1MHZ	
	OSCCR = 0x08;                                            // System clock: 1MHz
#else	
	OSCCR = 0x18;                                            // System clock: 4MHz
	                                                        // If 0x00/0x08/0x10/0x18, 0.5MHz/1MHz/2MHz/4MHz
#endif
	
}

void WD_Reset(void)
{
	//WDTDR = 30;												// entrance to every 2sec
	WDTDR = 2;												// entrance to every 0.192sec
	//WDTCR = 0xA3;											// set WDT enable , clock source LFIRC, WDT interrupt
	WDTCR = 0xE5;											// set WDT enable , clock source WDTRC, WDT interrupt
	IE3 |= 0x08;											// WDT interrupt enable	
	
}

////////////////End  와치독  함수 //////////// 

////////////////Start  ADC  함수 //////////// 

/**********************************************************************
 * @brief		Data_Avr
 * @param   	None
 * @return	None
 **********************************************************************/
// 평균 값 계산 함수 
uint16_t Data_Avr(uint16_t *adc_data, uint8_t count){
	
	uint32_t temp = 0;
	uint8_t i;
	
	for(i=0; i<count ; i++){
		temp = temp + adc_data[i];
	}
	
	temp = (uint16_t)(temp / count);
	
	return temp;
	
}


/**********************************************************************
 * @brief		TEMP_ADC
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t TEMP_ADC(void)
{
	int Temp_adc_data= 0;
	
	
	
	P0FSRL  = 0
		| ( 2 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 2 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 2<< 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	
	OP0_Enable(FALSE);
	OP1_Enable(FALSE);
	LDO_ON;
	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_LDO_REF, ADC_LSB);
	ADC_SelectChannel(ADC_CH3);
	
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	
	LDO_OFF;

	Temp_adc_data = Data_Avr(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	
	return Temp_adc_data;
	
}
/*
**********************************************************************
 * @brief		BAT_ADC
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t BAT_ADC(void){
	
	uint16_t Bat_ADC;
	
	uint32_t op;
	
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 1<< 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	

	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);		
	ADC_SelectChannel(ADC_VBGR);

	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	
	op=92;
	op*=1024;
	op/= Data_Avr(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
  Bat_ADC=op;
	
	return Bat_ADC;
}

/**********************************************************************
 * @brief		Dust_ADC
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t Dust_ADC_1AMP(void)
{
	
	int Dust_adc_data= 0;
	uint32_t op;
	
	
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 2<< 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 1 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	
	OPAMP_Initial(GAIN1_DIS, GAIN0_DIS, chp_125KHz);
	OP0_Enable(TRUE);
	OP1_Enable(FALSE);

	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	
	ADC_SelectChannel(ADC_VBGR);
	
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	
	if(ADC_temp_data[buf_cnt/2] == 0) ADC_temp_data[buf_cnt/2] = 1;
	op=92;
	op*=1024;
	op/=Data_Avr(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
  ADC1_Bat_Val=op;

	LDO_ON;
	ADC_SelectChannel(ADC_OP0OUT);
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	LDO_OFF;
	
	op=ADC1_Bat_Val;
	op *=Data_Avr(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	op/=1024;
  ADC1_On_Dust_Val=op;
	
	//led off 시 센서값 읽기;
	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	ADC_SelectChannel(ADC_OP0OUT);
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	
	op=ADC1_Bat_Val;
	op *=Data_Avr(ADC_temp_data, ADC_MAX_BUFFER_SIZE);
	op/=1024;
  ADC1_Off_Dust_Val=op;
	
	Dust_adc_data = ADC1_On_Dust_Val - ADC1_Off_Dust_Val;
	
	return Dust_adc_data;
	
}


////////////////End  ADC  함수 //////////// 

////////////////Start  UART  함수 //////////// 


void Uart_Out(void){

	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
						
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
	
	if(ADC_mode == Dust_mode){
		
		if(visual_type == 1){
			USART_SendDataWithPolling(&Visu_MODE, 10);
			USART_SendDataWithPolling(&Tab, 4);
		}
		else{
			USART_SendDataWithPolling(&Dust_MODE, 10);
			USART_SendDataWithPolling(&Tab, 4);
		}
	}
	else if(ADC_mode == Temp_mode){
		
		USART_SendDataWithPolling(&Temp_MODE, 10);
		USART_SendDataWithPolling(&Tab, 4);
	}
	
	USART_SendDataWithPolling(&End, 4);
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);
}

void Uart_Out_Int(uint32_t Value){

	uint8_t int_val ,tmp;
	
	int_val = Value / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Value % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Value % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Value % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
}
////////////////End  UART  함수 //////////// 