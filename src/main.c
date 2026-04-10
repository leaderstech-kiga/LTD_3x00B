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

//#define Dust_1amp_Val_old	3 // opamp  1개 bat 미 체크시 adc 값 낮음
//#define Dust_1amp_Val_old	5	// 240119 , 축적 감도 보정 , 5일때 2개 앰프 활성화시 100 이하값 측정 
//#define Dust_1amp_Val_old	4	// 240624, 축적 감도 보정 , 240223 버전보다 둔감한 제품용 
#define Dust_1amp_Val_old	3	// 240624, 축적 감도 보정 , Stand_along_20240624_1 버전보다 둔감한 제품용 

//#define Dust_Val_old						110   // 10%에 동작하도록 설정 
//#define Dust_Val_old						170   // 10%에 동작하도록 설정   , 230504 단독형 민감 장비 대응으로 감도 10% 낮춤
//#define Dust_Val_old						150   // 10%에 동작하도록 설정   , 240223  둔감한제품 수정용
//#define Dust_Val_old						130   // 10%에 동작하도록 설정   , 240223 버전보다 둔감한 제품용 
//#define Dust_Val_old						110   // 10%에 동작하도록 설정   , Stand_along_20240624_1 버전보다 둔감한 제품용 
//#define Dust_Val_old						90   // 10%에 동작하도록 설정   , Stand_along_20240624_2 버전보다 둔감한 제품용 
#define Dust_Val_old						70   // 10%에 동작하도록 설정   , Stand_along_20250122_1 버전보다 둔감한 제품용 

////230120 시각형 감도 차이로 인한 장비 분별과 감도값 재 적용
//#define Dust_Visual_1amp_Val	10// opamp  1개 bat 미 체크시 adc 값 낮음
//#define Dust_Visual_Val						200   // 10%에 동작하도록 설정 
/////////////////////////////////////////////////////

////230811 시각형 w저항 10옴 오삽입 으로  인한 장비 분별과 감도값 재 적용
#define Dust_Visual_1amp_Val	5// opamp  1개 bat 미 체크시 adc 값 낮음
//#define Dust_Visual_Val						150   // 10%에 동작하도록 설정 
#define Dust_Visual_Val						130   // 240223  둔감한제품 수정용
/////////////////////////////////////////////////////


uint8_t visual_type = 0;

uint16_t Dust_Val, Dust_1amp_Val;

////230120 시각영 감도 차이오 인한 장비 분별과 감도값 재 적용 완료

/*2112월 설정값
#define Temp_det_Val		677			//  -> 이값에 도달하면 adc를 1초에 한번 한다. 화재 경보 근처값임으로 8초나 기다리지 않게 하기 위함이다,
																							  //  -> 측정치 값보다 3정도 작게 한다.
#define Temp_Val						680			//회로 변경후 값( 70초 동작)  , 73 도
 */
 
 /*22년 1월 설정값*/  // 동작  시간을 25초 당겨달라고 요청   22.01.26
																//  21년11월10일 시험기준 25도를 댕기려면			22정도 값을 나타낸다.		

 //#define Temp_det_Val		654			//  -> 이값에 도달하면 adc를 1초에 한번 한다. 화재 경보 근처값임으로 8초나 기다리지 않게 하기 위함이다,
																							  //  -> 측정치 값보다 3정도 작게 한다.
//#define Temp_Val						657			//회로 변경후 값( 70초 동작)  , 73 도
 
 
  /*22년 1월27  설정값*/  // 동작  시간을 25초 당겨달라고 요청   22.01.26 -> 10초정도 더 요청
																//  21년11월10일 시험기준 25도를 댕기려면			22정도 값을 나타낸다.		 -> 22 추가 낮춤

// #define Temp_det_Val		632			//  -> 이값에 도달하면 adc를 1초에 한번 한다. 화재 경보 근처값임으로 8초나 기다리지 않게 하기 위함이다,
																							  //  -> 측정치 값보다 3정도 작게 한다.
//#define Temp_Val						635			//회로 변경후 값( 70초 동작)  , 73 도

/*22년 2월7일 설정값*/  // 동작  시간을 25초 당겨달라고 요청   22.01.26 -> 적용값이 10초 정도 빨리 동작해 중간값 요청 
																//  Temp_Val = 657 , 635 중간값 646 ,  Temp_det_Val = Temp_Val - 3
																
#define Temp_det_Val		643			//  -> 이값에 도달하면 adc를 1초에 한번 한다. 화재 경보 근처값임으로 8초나 기다리지 않게 하기 위함이다,
																							  //  -> 측정치 값보다 3정도 작게 한다.
#define Temp_Val						646			//회로 변경후 값( 70초 동작)  , 73 도

#define Temp_Low_Val		500			//53도
#define Temp_err_Val			960		//145 이상 에러
#define open_Val_L					150		//
#define open_Val_H					1000		//

#define  ADC_BUFFER_SIZE 16

#define uart_debug_On 	1
#define uart_debug_Off 	0

uint8_t  uart_debug = uart_debug_On ;
//uint8_t  uart_debug = 0 ;

//초기 디버그 진입 모드 선택
uint8_t  start_uart_debug = 1 ; //uart 디버그 모드 진입 가능 .
//uint8_t  start_uart_debug = 0 ;


/* 2022 11 17 베터리 비교값 변경 */

uint16_t  Bat_Low = 265;		//2.65V 
uint16_t  Bat_high = 270;		// 2.7V  


uint8_t deviceID[3];
uint8_t SystemStatus;

uint16_t adc_time	= 8 * 5;	// adc time	, 0.192초 주기
uint16_t ck_led_time	= 56 * 5  + 12;	// 8 * 7 초 , , 0.192초 주기 > 0.192 * 5* 56  = 53.7  -> 2.3초 모자름 : 0.192 * 12 더해줌
uint16_t Bat_al_time	= 280  * 5  + 58 ;	// 280 초 , , 0.192초 주기  -> 0.192 * 5* 280  = 268.8  -> 11.2초 모자름 : 0.192 * 58 더해줌  

/* [BUG-09 NOTE] fire_off_cnt: Intended as alarm-off counter but never read.
   Reserved for future auto-cancel logic. */

uint8_t  fire_off_cnt ;
uint8_t  bat_first = 1 ;


// Audio Data : 8000bps , 8bit 
uint16_t Audio_length;
uint16_t Audio_addr;
uint8_t Audio_start;
uint8_t Audio_start_address;
uint16_t Audio_max_length;
/* [BUG-06 FIX] Duplicate declaration removed - SystemStatus already declared at line 168 */
/* BEFORE: uint8_t SystemStatus; */
/* uint8_t SystemStatus; */

uint8_t EM_AL_Status = 0;

uint16_t  Audio1_Address  = 0x0;
uint16_t  Audio1_length  = 10801;
uint8_t  Audio1_runtime  = 1;
uint16_t  Audio2_Address  = 0x2baa;
uint16_t  Audio2_length  = 21478;
uint8_t  Audio2_runtime  = 1;
uint16_t  Audio3_Address  = 0x8108;
uint16_t  Audio3_length  = 24324;
uint8_t  Audio3_runtime  = 1;

uint16_t  ADC_wait = 8;
uint16_t  charge_wait= 10;
uint16_t  bat_alarm_wait= 10;
uint16_t  alarm_off_wait= 340;

uint16_t bat_val;
uint32_t Visu_data;
uint32_t Dust_o_data;
uint32_t Dust_data;
uint32_t Dust_Sum_data;
uint16_t Tmp_data;
uint16_t Dust_Count;
uint16_t Temp_Count;
uint16_t Al_Stop_key_Count;

uint16_t	init_Dust_val ;
uint16_t	delta_Dust_val = 0 ;

uint16_t system_count = 0;
uint16_t Low_Bat_count = 0;

uint16_t 	Fire_Alarm_LED = 0;
uint16_t 	Bat_Alarm_LED = 0;

uint8_t  SYS_mode , ADC_mode , Bat_mode;

uint16_t temp_data[ADC_MAX_BUFFER_SIZE];

/* [BUG-01 FIX] Buffer overflow: array size 8->10 to match charge_wait(10) */
/* BEFORE: uint16_t temp_charge_data[8]; */
uint16_t temp_charge_data[10];
uint16_t charge_data_Avr;
uint32_t temp_charge_data_sum;

uint16_t adc_read_val;
uint8_t	ADC_port ;

uint8_t uart_pkey_cnt;


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

uint8_t Timer2_cnt;

int adc_tmp[10];

uint32_t Dust_Low_o_data;				// led off 상태 adc 값 op0
//#define ext_Optic_Low_Val		10 // opamp1 외부 광원 있을시 low value
uint32_t Dust_Low_data;				// led off 상태 adc 값 op1

#define ext_Optic_Low_Val		1// opamp1 외부 광원 있을시 low value  
																								//  20221106  챔버 변경 버전 기본값이 2 정도 높음 
#define ext_Optic_High_Val		150 // opamp1 외부 광원 있을시 low value

uint8_t Charge_mode,Charge_mode_cnt = 5 ;		// 축적 활성화시 100ms 간격으로 Charge_mode_cnt 만큼 다시 읽어 평균값으로 재확인 
uint16_t Charge_ch_val ;

int j;

struct Adc_Val_Def
{
	uint16_t LED_Off_Val;
	uint16_t LED_On_Val;
	uint16_t Bat_Val;
};

struct Adc_Val_Def  Single_Amp_Adc, Dual_Amp_Adc;

uint8_t Audio_start =0;
/* Public Variable -----------------------------------------------------------*/
/* Public Function -----------------------------------------------------------*/

/**********************************************************************
 * @brief		Main program
 * @param   	None
 * @return	    None
 **********************************************************************/
void Main(void)
{
	
	int i;
	
	hw_initial_Wait(10);
	
	SystemStatus = SPI_Memory_Check();
	
	ADC_mode = Dust_mode;
	
	Check_Uart_Mode(1);
	
	hw_initial_Wait(10);
	system_count = 0;		
	
	//Bat check start 시작시
  Bat_Ck();
	
	SYS_mode =  In_Fire_al_mode;
	
	while(1)
	{

	//전원 감시 led 56초 , 베터리 체크 
	//////////////////////////////////////////////////////////////////////////////////////////
		
		if(system_count%ck_led_time == 0){
		
			hw_initial_Wait(10);
			LED_G_ON;
			Delay_ms(5);
		  LED_G_OFF;
			Bat_Ck();
			hw_initial_Wait(10);
			
		}
	//////////////////////////////////////////////////////////////////////////////////////////
				
	//ADC check start 8초 
	//////////////////////////////////////////////////////////////////////////////////////////		
		if(system_count%adc_time == 0){

			// system 초기화 
			hw_initial_Wait(10);
			
			LED_G_ON;
			Delay_ms(5);
			LED_G_OFF;
			
			if(ADC_mode == Dust_mode){

					Dust_ADC_Single(5);
					
			}
			else if(ADC_mode == Temp_mode){
				
		}
			
		hw_initial_Wait(10);
	}
	//ADC check start 8초  end
	//////////////////////////////////////////////////////////////////////////////////////////		

	//내부 화재 확인
	//////////////////////////////////////////////////////////////////////////////////////////		
	if((SYS_mode ==  In_Fire_al_mode)){
		
		char In_charge_wait = 4;
		
		// system 초기화 
		hw_initial_Wait(10);
		
		// 연동 출력 
		Port_SetOutputpin(PORT1, PIN0, 0);
		Delay_ms(100);
		P10 = 1;
			
		while(1){

			Fire_Alarm();
			
			if(SYS_mode  == Al_Stop_mode){
				break;
			}

			Delay_ms(100);
			
			if(ADC_mode == Dust_mode){
				Dust_data = 0;
				
				//축적 4회 실시 
				for(i=0;i<In_charge_wait;i++){
					Dust_o_data = Dust_ADC_1AMP(5);
					Dust_data = Dust_ADC_2AMP(5);
				
					temp_charge_data[i] = Dust_data - Dust_Low_data;
					
					Delay_ms(5);
							
					//Uart Test//
						if(uart_debug == 1){
							Uart_Out(3);
						}
						else{
							Uart_Set();
						}
						//Uart Test end//
				}
						
				// 축적 평균 값 계산
				temp_charge_data_sum = 0;
				for(i=0;i<In_charge_wait;i++){
					temp_charge_data_sum = temp_charge_data_sum + temp_charge_data[i];
				}
						
				charge_data_Avr = temp_charge_data_sum / In_charge_wait;
									
				//Uart Test//
				if(uart_debug == 1){
					Uart_Out_avr(charge_data_Avr);
				}
				else{
					Uart_Set();
				}
				//Uart Test end//
						
				if(charge_data_Avr > Dust_Val){
					Dust_Count = 0;
					fire_off_cnt = 0;
					SYS_mode =  In_Fire_al_mode;
				}
				else{
					Dust_Count = 0;
					SYS_mode =  Normal_mode;
					Fire_Alarm_LED = 0;
					Bat_Alarm_LED = 0;
					Timer2_Stop();
					Port_SetInputpin(PORT1, PIN0, 0);  // EM IO 
					P10 = 0;
					break;
				}	
			}
	}
		
	// system 초기화 
	hw_initial_Wait(10);
	
}
	
	//내부 화재 확인 end
	//////////////////////////////////////////////////////////////////////////////////////////		
	
  // 경보 테스트 
	//////////////////////////////////////////////////////////////////////////////////////////		
	if((SYS_mode ==  Normal_mode)&(Port_GetInputpinValue(PORT1, PIN2)  == 0)){
		
		// system 초기화 
		hw_initial_Wait(10); 
		

		Port_SetOutputpin(PORT1, PIN0, 0);
		Delay_ms(100);
		P10 = 1;
		
		T2_init();
		Fire_Alarm_LED = 1;
		Bat_Alarm_LED = 0;
		Timer2_Start();
		
		// 시험 방송 1회 만 진행 
		
		//Fire_Alarm();
		
		Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);
		
		GLOBAL_INTERRUPT_DIS();     
		hw_audio_initial(); 
		Delay_ms(10);
		GLOBAL_INTERRUPT_EN();  
		
		
		Timer2_Stop();

		Fire_Alarm_LED = 0;
		Bat_Alarm_LED = 0;
		Timer2_Stop();
		LED_R_OFF;
		LED_G_OFF;
		
		Port_SetInputpin(PORT1, PIN0, 0);  // EM IO 
		P10 = 0;
		
		// system 초기화 
			hw_initial_Wait(10); 
		
	}
	
	// 경보 테스트 end
	//////////////////////////////////////////////////////////////////////////////////////////		

	//Bat_Low_mode check start
	//////////////////////////////////////////////////////////////////////////////////////////
	if(Bat_mode == Bat_Low_mode){
		
		if(Low_Bat_count == 0){
			// system 초기화 
			hw_initial_Wait(10); 
			
			T2_init();
			Fire_Alarm_LED = 0;
			Bat_Alarm_LED = 1;
			Timer2_Start();
			
			LowBat_Alarm();
			
			Fire_Alarm_LED = 0;
			Bat_Alarm_LED = 0;
			Timer2_Stop();
			
			// system 초기화 
			hw_initial_Wait(10); 
		}
		Low_Bat_count++;
		if(Low_Bat_count == Bat_al_time){
			Low_Bat_count = 0;
		}
	}		
	//Bat_Low_mode check end
	//////////////////////////////////////////////////////////////////////////////////////////

	//Stop_mode Start
	//////////////////////////////////////////////////////////////////////////////////////////
	
	if(SYS_mode  == Al_Stop_mode){
	
	int i,j;
	int break_mode = 0;
	
	// system 초기화 
	hw_initial_Wait(10);
	
	Audio_Run(Audio1_Address , Audio1_length  , Audio1_runtime);	
	
	Timer2_Stop();
	
	Port_SetInputpin(PORT1, PIN0, 0);  // EM IO 
	P10 = 0;
	
	SYS_mode  = Normal_mode;
	
	LED_G_ON;
	NOP_20us_Delay(25000); // 500ms
	LED_G_OFF;
	NOP_20us_Delay(25000); // 500ms
	
	while(1){
		if(Port_GetInputpinValue(PORT1, PIN2)  == 1){
				break;
			}
	}

	
	for(i=0;i<8;i++){
		
		// 100ms
		LED_G_ON;
		NOP_20us_Delay(500); // 10ms
		LED_G_OFF;
		NOP_20us_Delay(4500); // 90ms
					
		//100ms + 400ms = 500ms
		for(j=0;j<4;j++){
		NOP_20us_Delay(5000);	// 100ms
			if(Port_GetInputpinValue(PORT1, PIN2)  == 0){
				break_mode = 1;
				break;
			}
		}
						
		// 500ms + 100ms = 600ms
		LED_G_ON;
		Delay_ms(10);
		LED_G_OFF;
		Delay_ms(90);
					
		//600ms + (100ms * 394) = 600ms + 39400ms = 40000ms = 40s
		for(j=0;j<394;j++){
			Delay_ms(100);
			if(Port_GetInputpinValue(PORT1, PIN2)  == 0){
				break_mode = 1;
				break;			
			}
		}
	
		if(break_mode == 1){
			break;
		}
	}

	LED_G_ON;
	Delay_ms(500);
	LED_G_OFF;

	while(1){
		if(Port_GetInputpinValue(PORT1, PIN2)  == 1){
				break;
			}
	}
	
		// system 초기화 
	hw_initial_Wait(10);
}
	
  //Stop_mode End
	//////////////////////////////////////////////////////////////////////////////////////////



  // EMO 경보 확인 
	//////////////////////////////////////////////////////////////////////////////////////////
	if((SYS_mode ==  Normal_mode)&(Port_GetInputpinValue(PORT1, PIN0)  == 1)){
		
		// system 초기화 
		hw_initial_Wait(10);
		
		Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);
		
		GLOBAL_INTERRUPT_DIS();     
		hw_audio_initial(); 
		NOP_20us_Delay(1000);
		GLOBAL_INTERRUPT_EN();  
		

		// system 초기화 
		hw_initial_Wait(10);
		
	}
	// EMO 경보 확인 end
	//////////////////////////////////////////////////////////////////////////////////////////
	
	system_count++;

		// 와치독 타이머 
	BeforeStop();
	GLOBAL_INTERRUPT_EN();  
	Stop();
	AfterStop();
			
	}
}



/**********************************************************************
 * @brief		Variable_Initial
 * @param   	None
 * @return	None
 **********************************************************************/
void Variable_Initial(uint16_t Audio_st_address)
{

	Audio_addr= Audio_st_address + ADDR_START;
	Audio_start=0;
	Audio_length=0;

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
	#ifdef DELAY_TIME
	AMPCR1 = 0x00
			|(0<<7)                                                       // If 0/1, Disable/Enable Amp1
			|(0<<4)                                                       // If 0/1/2/3/4/5/6/7, "Disable gain"/x1/x2/x10/x15/x20/x30/x60
			|(0<<3)                                                       // If 0/1, Disable/Enable Amp0
			|(0<<0);                                                      // If 0/1/2/3, "Disable gain"/x5/x10/x20
	#else
	AMPCR1 = 0x00
			|(1<<7)                                                       // If 0/1, Disable/Enable Amp1
			|(0<<4)                                                       // If 0/1/2/3/4/5/6/7, "Disable gain"/x1/x2/x10/x15/x20/x30/x60
			|(1<<3)                                                       // If 0/1, Disable/Enable Amp0
			|(0<<0);                                                      // If 0/1/2/3, "Disable gain"/x5/x10/x20
	#endif
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



void hw_audio_initial(void) {
	BITCR = 0x60;                                              // BIT cnt cleared, Select 0Bit overflow period
	
#ifdef SysClock_1MHZ	
	OSCCR = 0x08;                                            // System clock: 1MHz
#else	
	OSCCR = 0x18;                                            // System clock: 4MHz
	                                                        // If 0x00/0x08/0x10/0x18, 0.5MHz/1MHz/2MHz/4MHz
#endif

	RSTFR = 0x00;                                            // Clear all reset flags
	
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
	#ifdef DELAY_TIME
	AMPCR1 = 0x00
			|(0<<7)                                                       // If 0/1, Disable/Enable Amp1
			|(0<<4)                                                       // If 0/1/2/3/4/5/6/7, "Disable gain"/x1/x2/x10/x15/x20/x30/x60
			|(0<<3)                                                       // If 0/1, Disable/Enable Amp0
			|(0<<0);                                                      // If 0/1/2/3, "Disable gain"/x5/x10/x20
	#else
	AMPCR1 = 0x00
			|(1<<7)                                                       // If 0/1, Disable/Enable Amp1
			|(0<<4)                                                       // If 0/1/2/3/4/5/6/7, "Disable gain"/x1/x2/x10/x15/x20/x30/x60
			|(1<<3)                                                       // If 0/1, Disable/Enable Amp0
			|(0<<0);                                                      // If 0/1/2/3, "Disable gain"/x5/x10/x20
	#endif
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


//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void AfterStop(void)
{

	IE3 &= ~(0x08);											// WDT interrupt disable
	//WDTCR = 0x00;
	
	/*
#ifdef SysClock_1MHZ	
	OSCCR = 0x08;                                            // System clock: 1MHz
#else	
	OSCCR = 0x18;                                            // System clock: 4MHz
	                                                        // If 0x00/0x08/0x10/0x18, 0.5MHz/1MHz/2MHz/4MHz
#endif
	*/
	OSCCR = 0x18;                                            // System clock: 4MHz
	                                                        // If 0x00/0x08/0x10/0x18, 0.5MHz/1MHz/2MHz/4MHz
}

void WD_Reset(void)
{
	//WDTDR = 30;												// entrance to every 2sec
	WDTDR = 2;												// entrance to every 0.192sec
	//WDTCR = 0xA3;											// set WDT enable , clock source LFIRC, WDT interrupt
	WDTCR = 0xE5;											// set WDT enable , clock source WDTRC, WDT interrupt
	IE3 |= 0x08;											// WDT interrupt enable	
	
}






/**********************************************************************
 * @brief		Fire_Alarm
 * @param   	None
 * @return	None
 **********************************************************************/
void Fire_Alarm(void){

	//Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);

	GLOBAL_INTERRUPT_DIS();     
	//hw_audio_initial(); 
	hw_initial();
	Port_SetOutputpin(PORT1, PIN0, 0);
	P10 = 1;
	NOP_20us_Delay(1000);
	NOP_20us_Delay(1000);
	GLOBAL_INTERRUPT_EN();  
		
		
	Timer2_Stop();
	T2_init();
	Fire_Alarm_LED = 1;
	Bat_Alarm_LED = 0;
	Timer2_Start();
	
	
	
		/* Audio Initial */
	if(SYS_mode  == Al_Stop_mode){
		
		Timer2_Stop();
		Fire_Alarm_LED = 0;
		Bat_Alarm_LED = 0;

	}
	else{
		
		Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);
		
		NOP_20us_Delay(25000); // 500ms
}
	

	if(SYS_mode  == Al_Stop_mode){
		
		Timer2_Stop();
		Fire_Alarm_LED = 0;
		Bat_Alarm_LED = 0;

	}
	else{
		
		GLOBAL_INTERRUPT_DIS();     
		//hw_audio_initial(); 
		hw_initial();
		Port_SetOutputpin(PORT1, PIN0, 0);
		P10 = 1;
		NOP_20us_Delay(1000);
		NOP_20us_Delay(1000);
		GLOBAL_INTERRUPT_EN();  

		
		T2_init();
		Fire_Alarm_LED = 1;
		Bat_Alarm_LED = 0;
		Timer2_Start();
		
		Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);
		
		
		NOP_20us_Delay(25000); // 500ms
}

	if(SYS_mode  == Al_Stop_mode){
		
		Timer2_Stop();
		Fire_Alarm_LED = 0;
		Bat_Alarm_LED = 0;

	}
	else{
		
		GLOBAL_INTERRUPT_DIS();     
		//hw_audio_initial(); 
		hw_initial();
		Port_SetOutputpin(PORT1, PIN0, 0);
		P10 = 1;
		NOP_20us_Delay(1000);
		NOP_20us_Delay(1000);
		GLOBAL_INTERRUPT_EN();  
		
		T2_init();
		Fire_Alarm_LED = 1;
		Bat_Alarm_LED = 0;
		Timer2_Start();
		
		Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);
		
		
		NOP_20us_Delay(25000); // 500ms
	}
	
}




/**********************************************************************
 * @brief		LowBat_Alarm
 * @param   	None
 * @return	None
 **********************************************************************/
void LowBat_Alarm(void){
	
	T2_init();
	Fire_Alarm_LED = 0;
	Bat_Alarm_LED = 1;
	Timer2_Start();
	Audio_Run(Audio2_Address , Audio2_length  , Audio2_runtime);
	Fire_Alarm_LED = 0;
	Bat_Alarm_LED = 0;
	LED_R_OFF;
	LED_G_OFF;
	Timer2_Stop();
	
}

/**********************************************************************
 * @brief		Audio_Run
 * @param   	None
 * @return	None
 **********************************************************************/
void Audio_Run(uint16_t Address , uint16_t Length,  uint8_t Run_time){
		 
	FLASH_ON;
	AUDIO_ON;	
	
	Audio_start = 1;

	P2 = 0x01;
	/* SPI Initial */
	USART_SPI_Initial(SPI_MASTER_MODE, 500000, SPI_MSB, SPI_CPOL_LOW, SPI_CPHA_1EDGE, SPI_TX_RX_MODE, SPI_SS_HW_DISABLE);	 // 500000
	
	NOP_20us_Delay(5000);         // 0.1sec
	NOP_20us_Delay(5000);         // 0.1sec
	NOP_20us_Delay(5000);         // 0.1sec
	
	Audio_max_length = Length;
	Variable_Initial(Address);
	
		/* Audio Initial */
	Audio_Initial();	
	
	NOP_20us_Delay(1000);         // 0.02sec
	NOP_20us_Delay(1000);         // 0.02sec
	NOP_20us_Delay(1000);         // 0.02sec
	
	
	Timer1_Start();	

	while(Audio_length < Audio_max_length){
	}
	
	FLASH_OFF;
	AUDIO_OFF;
	P2 = 0x00;
	
	//Port_SetOutputpin(PORT1, PIN0, 0);
	//P10 = 1;
	
	Audio_start = 0; 
}

/**********************************************************************
 * @brief		T2_init
 * @param   	None
 * @return	None
 **********************************************************************/
void T2_init(void){
	/* Timer2 initialize @ 62500Hz */
	Timer2_Initial(T2_PPG_REPEAT_MODE, TIMER2_DIV64);
	
	 /* Timer2 PPG Polarity Start Low */
	Timer2_SetPPGPolarity(T2_START_LOW);
	
	 /* Timer2 PPG Period Counter (31250) 62500/31250 = 2Hz  */
	Timer2_SetPPGPeriodCounter(62500 / 10);
	
    /* Enable timer2 match INT */   
 	Timer2_ConfigureInterrupt(TRUE);
	
	Timer2_cnt = 0;
}
/**********************************************************************
 * @brief		TIMER2_Int(void) interrupt T2_MATCH_VECT
 * @param   	None
 * @return	None
 **********************************************************************/
char Gled_mode, GLED_CNT;
void TIMER2_Int(void) interrupt T2_MATCH_VECT
{
	
	if(Timer2_cnt == 0){
		
		if(Fire_Alarm_LED == 1){
			//Port_SetOutputTogglepin(PORT0, PIN7);
			LED_R_ON;
		}
		
		if(Bat_Alarm_LED == 1){
			//Port_SetOutputTogglepin(PORT0, PIN6);
			if (Gled_mode == 0){
				LED_G_ON;	
				GLED_CNT++;
				if(GLED_CNT > 1){
					Gled_mode = 1;
					GLED_CNT = 0;
				}
			}
			else{
				LED_G_OFF;	
				Gled_mode = 0;
			}
		}
		else{
			GLED_CNT = 0;
			LED_G_OFF;	
			Gled_mode = 0;
		}
	
	}
	else{
		LED_R_OFF;
		//LED_G_OFF;
		
		if(Bat_Alarm_LED == 1){
			//Port_SetOutputTogglepin(PORT0, PIN6);
			if (Gled_mode == 0){
				LED_G_ON;	
				//Gled_mode = 1;
			}
			else{
				LED_G_OFF;	
			}
		}
		
	
	}

	Timer2_cnt++;
	
	if(Timer2_cnt == 10){
		Timer2_cnt = 0;
	}
	
	if(SYS_mode == In_Fire_al_mode){
		
		if(Port_GetInputpinValue(PORT1, PIN2)  == 0){
			Al_Stop_key_Count++;
		}
		else{
			Al_Stop_key_Count = 0;
		}
		
		if(Al_Stop_key_Count > 5){
			SYS_mode  = Al_Stop_mode;
			LED_G_ON;
		}
			
	}
	
}

void ADC_Uart_Out(uint16_t Adc_data){
	
	//UART mode 설정

	//Set Alernative Function for USART P12(TX) / P13(RX)
	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
	//Port_SetAlterFunctionpin(PORT1, PIN3, 0x1);
				
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);

		
	int_val = Adc_data / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Adc_data % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Adc_data % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Adc_data % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);

	USART_SendDataWithPolling(&End, 4);

	/* [BUG-10 FIX] Restore P1.2 to GPIO (button input) after UART use - same as Uart_Out_avr/Uart_Set */
	/* BEFORE: (Port_SetAlterFunctionpin restore was missing - P1.2 stuck as UART TX) */
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);

}


void Uart_Out_avr(int avr_val){
	
	//Set Alernative Function for USART P12(TX) / P13(RX)
	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
	//Port_SetAlterFunctionpin(PORT1, PIN3, 0x1);
						
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
	
	
	USART_SendDataWithPolling(&Avr, 9);
	USART_SendDataWithPolling(&Tab, 4);
	
	int_val = avr_val/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= avr_val% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= avr_val% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= avr_val% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&End, 4);
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);
	
}


void Uart_Set(void){
	
	//Set Alernative Function for USART P12(TX) / P13(RX)
	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
	//Port_SetAlterFunctionpin(PORT1, PIN3, 0x1);
						
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);
	
}
void Uart_Out(char mode){
	
	//Set Alernative Function for USART P12(TX) / P13(RX)
	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
	//Port_SetAlterFunctionpin(PORT1, PIN3, 0x1);
						
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
	
	int_val = Visu_data/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Visu_data% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Visu_data% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Visu_data% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
							
	USART_SendDataWithPolling(&Tab, 4);
	
	if (mode == 1){
		USART_SendDataWithPolling(&ADC, 4);
		USART_SendDataWithPolling(&Tab, 4);
		
		Dust_Low_data = 0;
		Dust_data = 0;
	}
	else if(mode == 2){
		USART_SendDataWithPolling(&CHR, 4);
		USART_SendDataWithPolling(&Tab, 4);
	}
	else if(mode == 3){
		USART_SendDataWithPolling(&INF, 4);
		USART_SendDataWithPolling(&Tab, 4);
	}
	
	USART_SendDataWithPolling(&Bat, 4);
					
	int_val = bat_val/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= bat_val% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= bat_val% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= bat_val% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
					
					
	USART_SendDataWithPolling(&Tab, 4);
					
	USART_SendDataWithPolling(&Dust_ori, 7);	
	int_val = Dust_Low_o_data / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_Low_o_data % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_Low_o_data % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_Low_o_data % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, 4);
	int_val = Dust_o_data / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_o_data % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_o_data % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_o_data % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, 4);
	int_val = (Dust_o_data - Dust_Low_o_data) / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= (Dust_o_data - Dust_Low_o_data)  % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= (Dust_o_data - Dust_Low_o_data)  % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= (Dust_o_data - Dust_Low_o_data)  % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	
	USART_SendDataWithPolling(&Tab, 4);
					
	USART_SendDataWithPolling(&Dust, 5);	
	int_val = Dust_Low_data / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_Low_data % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_Low_data % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_Low_data % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, 4);
					
	int_val = Dust_data / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_data % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_data % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Dust_data % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, 4);
					
	int_val = (Dust_data - Dust_Low_data)  / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= (Dust_data - Dust_Low_data) % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= (Dust_data - Dust_Low_data) % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= (Dust_data - Dust_Low_data) % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	
	USART_SendDataWithPolling(&Tab, 4);
	
	USART_SendDataWithPolling(&Temp, 5);
	int_val = Tmp_data / 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Tmp_data % 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Tmp_data % 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Tmp_data % 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);


	USART_SendDataWithPolling(&End, 4);
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);
	
}

void WDT_Int(void) interrupt WDT_VECT
{
	WDTCR &= ~0x01;
}

void Data_Sorting(uint16_t *adc_data, uint8_t count)
{
	uint8_t i,j;
	uint16_t temp;

	for(i = 0; i < (count-1); i++)
	{
		for(j = i+1; j < count; j++)
		{
			
			if(adc_data[i] > adc_data[j])
			{
				temp = adc_data[i];
				adc_data[i]=adc_data[j];
				adc_data[j]=temp;
			}
		}
	}
}

/**********************************************************************
 * @brief		Data_Avr
 * @param   	None
 * @return	None
 **********************************************************************/

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
 * @brief		Dust_ADC
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t Dust_ADC_1AMP(uint8_t buf_cnt)
{
	int Dust_adc_data= 0;
	uint32_t op;
	
	uint16_t ADC_temp_data[ADC_BUFFER_SIZE];
	
	/*
	P1IO = 0x3A;                 	// 0 : Input,	1 : Output // P13을 0으로 묶는다.
	P13 = 0;
	*/
	
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 2<< 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 1 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	
	OPAMP_Initial(GAIN1_DIS, GAIN0_DIS, chp_125KHz);
	OP0_Enable(TRUE);
	OP1_Enable(FALSE);

	ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	
	ADC_SelectChannel(ADC_VBGR);
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	Data_Sorting(ADC_temp_data,buf_cnt);
	
	/* [BUG-04 FIX #1] Prevent division by zero if ADC returns 0 (Dust_ADC_1AMP) */
	/* BEFORE: op=92; op*=1024; op/=ADC_temp_data[buf_cnt/2]; */
	if(ADC_temp_data[buf_cnt/2] == 0) ADC_temp_data[buf_cnt/2] = 1;
	op=92;
	op*=1024;
	op/=ADC_temp_data[buf_cnt/2];
  bat_val=op;

	//ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_LDO_REF, ADC_LSB);	
	LDO_ON;

	NOP_20us_Delay(5); // 100us
	
	ADC_SelectChannel(ADC_OP0OUT);
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	LDO_OFF;

	Data_Sorting(ADC_temp_data,buf_cnt);
	
	op=bat_val;
	op *=ADC_temp_data[buf_cnt/2];
	op/=1024;
  Dust_adc_data=op;
	
	//led off 시 센서값 읽기;
	ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	ADC_SelectChannel(ADC_OP0OUT);
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	Data_Sorting(ADC_temp_data,buf_cnt);
	
	op=bat_val;
	op *=ADC_temp_data[buf_cnt/2];
	op/=1024;
  Dust_Low_o_data=op;
	
	return Dust_adc_data;
	
}


/**********************************************************************
 * @brief		Dust_ADC_2amp
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t Dust_ADC_2AMP(uint8_t buf_cnt)
{
	uint16_t Dust_adc_data= 0 , i;
	
	uint32_t op;
	uint16_t ADC_temp_data[ADC_BUFFER_SIZE];
	
	/* [BUG-07 FIX] Replaced direct P1IO register write with Port API; GPIO state restored on exit */
	/* BEFORE: P1IO = 0x3A; P13 = 0; */
	Port_SetOutputpin(PORT1, PIN3, 0); /* Set P13 as output */
	P13 = 0;
	
	if(visual_type == 1){
		OPAMP_Initial(GAIN1_X30, GAIN0_DIS, chp_125KHz);
	}
	else{
		OPAMP_Initial(GAIN1_X15, GAIN0_DIS, chp_125KHz);
	}
	
	OP0_Enable(TRUE);
	OP1_Enable(TRUE);

	ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	
	ADC_SelectChannel(ADC_VBGR);
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	Data_Sorting(ADC_temp_data,buf_cnt);
	
	/* [BUG-04 FIX #2] Prevent division by zero if ADC returns 0 (Dust_ADC_2AMP) */
	/* BEFORE: op=92; op*=1024; op/=ADC_temp_data[buf_cnt/2]; */
	if(ADC_temp_data[buf_cnt/2] == 0) ADC_temp_data[buf_cnt/2] = 1;
	op=92;
	op*=1024;
	op/=ADC_temp_data[buf_cnt/2];
  bat_val=op;

	LDO_ON;
	NOP_20us_Delay(10); // 200us
	
	
	ADC_SelectChannel(ADC_OP1OUT);
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	LDO_OFF;

	Data_Sorting(ADC_temp_data,buf_cnt);
	
	op=bat_val;
	//op *=ADC_temp_data[buf_cnt/2];
	////////////////////
	Dust_Sum_data = 0;
	for(i=0;i< buf_cnt;i++){
		Dust_Sum_data += ADC_temp_data[i];
	}
	Dust_Sum_data = Dust_Sum_data / buf_cnt;
	op *= Dust_Sum_data;
	///////////////////
	op/=1024;
  Dust_adc_data=op;
	
	//led off 시 센서값 읽기;

	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	Data_Sorting(ADC_temp_data,buf_cnt);
	
	op=bat_val;
	op *=ADC_temp_data[buf_cnt/2];
	op/=1024;
  Dust_Low_data=op;
	
	/* [BUG-07 FIX] Restore P13 to input mode (match hw_initial state) */
	/* BEFORE: (missing - P13 was left as output after function returned) */
	Port_SetInputpin(PORT1, PIN3, 0);

	return Dust_adc_data;
	
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
	NOP_20us_Delay(10); // 200us
	
	ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_LDO_REF, ADC_LSB);
	ADC_SelectChannel(ADC_CH3);
	
	ADC_GetDataWithPolling(temp_data, ADC_MAX_BUFFER_SIZE);
	
	LDO_OFF;

	Data_Sorting(temp_data,ADC_MAX_BUFFER_SIZE);
	Temp_adc_data = temp_data[ADC_MAX_BUFFER_SIZE/2];
	
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
	

	NOP_20us_Delay(10); // 200us

	ADC_Initial(ADC_CLK_4M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	//ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);		
	
	ADC_SelectChannel(ADC_VBGR);

	ADC_GetDataWithPolling(temp_data, ADC_MAX_BUFFER_SIZE);
	

	Data_Sorting(temp_data,ADC_MAX_BUFFER_SIZE);
	
	/* [BUG-04 FIX #3] Prevent division by zero if ADC returns 0 (BAT_ADC) */
	/* BEFORE: op=92; op*=1024; op/=temp_data[ADC_MAX_BUFFER_SIZE/2]; */
	if(temp_data[ADC_MAX_BUFFER_SIZE/2] == 0) temp_data[ADC_MAX_BUFFER_SIZE/2] = 1;
	op=92;
	op*=1024;
	op/=temp_data[ADC_MAX_BUFFER_SIZE/2];
  Bat_ADC=op;
	//Bat_ADC=temp_data[ADC_MAX_BUFFER_SIZE/2];
	
	return Bat_ADC;
}
/* --------------------------------- End Of File ------------------------------ */




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
 * @brief		Enable_WD
 * @param   	None
 * @return	None
 **********************************************************************/
// 와치독을 활성화 한다.

void Enable_WD(void){
	
		// 와치독 타이머 
		BeforeStop();
		GLOBAL_INTERRUPT_EN();  
		Stop();
		AfterStop();
	
}

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

uint8_t SPI_Memory_Check(void){
	
	uint8_t SystemStatus_Read;
	uint8_t deviceID[3];
	
	  // SPI Memory Chack 
	FLASH_ON;	
	P2 = 0x0F;
	// SPI Initial 
	USART_SPI_Initial(SPI_MASTER_MODE, 500000, SPI_MSB, SPI_CPOL_LOW, SPI_CPHA_1EDGE, SPI_TX_RX_MODE, SPI_SS_HW_DISABLE);	 // 500000
	
	Get_Identification(deviceID);
	if( (deviceID[0]==0xC2)&&(deviceID[1]==0x20)&&(deviceID[2]==0x11) )
	{
		SystemStatus_Read=1;
	}
	else{
		SystemStatus_Read=0;	
	}

	FLASH_OFF;
	
		if(SystemStatus_Read == 1){
		
		LED_G_ON;
		Delay_ms(50);
		LED_G_OFF;
		
	}
	else{
		
		LED_R_ON;
		Delay_ms(50);
		LED_R_OFF;
	}
	
	
	Delay_ms(100);
	LED_G_ON;
	LED_R_ON;
	Delay_ms(50);
	LED_G_OFF;
	LED_R_OFF;
	
	return SystemStatus_Read;
}

/**********************************************************************
 * @brief		hw_initial_Wait
 * @param   	None
 * @return	None
 **********************************************************************/
void Check_Uart_Mode(uint8_t start_uart_debug){
	
	Port_SetInputpin(PORT1, PIN2, 1);  //key input & mode0  -> button input , 초기 눌려 있으면 UART mode
	
		if(start_uart_debug == 1){
			if(Port_GetInputpinValue(PORT1, PIN2)  == 0){
				SYS_mode = UART_mode;
			}
			else{
				SYS_mode = Normal_mode;
			}
			
			LED_G_ON;
			Delay_ms(100);
			LED_G_OFF;
			
			// key 풀릴때까지 대기 
			if(SYS_mode == UART_mode){
					while(1){
						LED_G_ON;
						if(Port_GetInputpinValue(PORT1, PIN2)  == 1){
							break;
						}
					}
					
					LED_G_OFF;
					//UART mode 설정
					
					while(1){
		
						LED_R_ON;
						
						Dust_ADC_Single(10);
						Dust_ADC_Dual(10);
						bat_val = BAT_ADC_Avr(10) ;
						Uart_Test_Out();
						Delay_ms(300);
						LED_R_OFF;
						Delay_ms(300);
					}
			}
		}
}

/**********************************************************************
 * @brief		BAT_ADC_Avr
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t BAT_ADC_Avr(uint8_t buf_cnt){
	
	uint16_t Bat_ADC;
	
	uint32_t op;
	
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 1<< 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	

	NOP_20us_Delay(10); // 200us

	ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	
	ADC_SelectChannel(ADC_VBGR);

	ADC_GetDataWithPolling(temp_data, buf_cnt);
	
	Data_Sorting(temp_data,buf_cnt);
	
	/* [BUG-04 FIX #3] Prevent division by zero if ADC returns 0 (BAT_ADC) */
	/* BEFORE: op=92; op*=1024; op/=temp_data[ADC_MAX_BUFFER_SIZE/2]; */
	if(temp_data[buf_cnt/2] == 0) {
		temp_data[buf_cnt/2] = 1;
	}
	op=92;
	op*=1024;
	op/=temp_data[ADC_MAX_BUFFER_SIZE/2];
  Bat_ADC=op;

	return Bat_ADC;
}

/**********************************************************************
 * @brief		Dust_ADC_Single
 * @param   	None
 * @return	None
 **********************************************************************/

void Dust_ADC_Single(uint8_t buf_cnt)
{
	int Dust_adc_data= 0;
	uint32_t op;
	
	uint16_t ADC_temp_data[ADC_BUFFER_SIZE];
	
	/*
	P1IO = 0x3A;                 	// 0 : Input,	1 : Output // P13을 0으로 묶는다.
	P13 = 0;
	*/
	
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 2<< 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 1 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	
	OPAMP_Initial(GAIN1_DIS, GAIN0_DIS, chp_125KHz);
	OP0_Enable(TRUE);
	OP1_Enable(FALSE);

	ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
  ADC_SelectChannel(ADC_VBGR);
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	op=92;
	op*=1024;
	op/=Data_Avr(ADC_temp_data,buf_cnt);
	Single_Amp_Adc.Bat_Val = op;
	
	ADC_SelectChannel(ADC_OP0OUT);
	LDO_ON;
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
	
	Single_Amp_Adc.LED_On_Val =Data_Avr(ADC_temp_data,buf_cnt);
	
	LDO_OFF;
  Delay_ms(1);
	//led off 시 센서값 읽기;
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);

	Single_Amp_Adc.LED_Off_Val=Data_Avr(ADC_temp_data,buf_cnt);
	
}


/**********************************************************************
 * @brief		Dust_ADC_Dual
 * @param   	None
 * @return	None
 **********************************************************************/

void Dust_ADC_Dual(uint8_t buf_cnt)
{
	uint16_t Dust_adc_data= 0 ;
	
	uint32_t op;
	uint16_t ADC_temp_data[16];
	
	P1IO = 0x3A;                 	// 0 : Input,	1 : Output // P13을 0으로 묶는다.
	P13 = 0;
	
	
	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// 
		| ( 1 << 2)       //P10     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 1 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	
	OPAMP_Initial(GAIN1_X30, GAIN0_DIS, chp_125KHz);
	
	OP0_Enable(TRUE);
	OP1_Enable(TRUE);

	ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	
	ADC_SelectChannel(ADC_OP1OUT);
	LDO_ON;
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
  Dual_Amp_Adc.LED_On_Val = Data_Avr(ADC_temp_data,buf_cnt);
	
	LDO_OFF;
  Delay_ms(1);
	
	//led off 시 센서값 읽기
	ADC_GetDataWithPolling(ADC_temp_data, buf_cnt);
  Dual_Amp_Adc.LED_Off_Val=Data_Avr(ADC_temp_data,buf_cnt);


}

/**********************************************************************
 * @brief		Uart_Test_Out
 * @param   	None
 * @return	None
 **********************************************************************/

void Uart_Test_Out(void){
	
	uint16_t tmp , Read_temp;
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
						
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
	
	if(ADC_mode == Dust_mode){
		USART_SendDataWithPolling(&Dust_MODE, 10);
	}
	else if(ADC_mode == Temp_mode){
		USART_SendDataWithPolling(&Temp_MODE, 10);
	}
	
	//베터리 프린트 
	USART_SendDataWithPolling(&Bat, 4);
					
	Read_temp = bat_val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
					
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
	//단일 앰프 LED OFF
	USART_SendDataWithPolling(&Dust_Si_Off, sizeof(Dust_Si_Off));	
	
	Read_temp = Single_Amp_Adc.LED_Off_Val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
	
	//단일 앰프 LED ON
	USART_SendDataWithPolling(&Dust_Si_On, sizeof(Dust_Si_On));	
	
	Read_temp = Single_Amp_Adc.LED_On_Val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
		//단일 앰프 LED Del
	USART_SendDataWithPolling(&Dust_Si_Deta, sizeof(Dust_Si_Deta));	
	
	Read_temp = Single_Amp_Adc.LED_On_Val - Single_Amp_Adc.LED_Off_Val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
	//듀얼  앰프 LED OFF
	USART_SendDataWithPolling(&Dust_Du_Off, sizeof(Dust_Du_Off));	
	
	Read_temp = Dual_Amp_Adc.LED_Off_Val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
	//듀얼  앰프 LED ON
	USART_SendDataWithPolling(&Dust_Du_On, sizeof(Dust_Du_On));	
	
	Read_temp = Dual_Amp_Adc.LED_On_Val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
		//듀얼  앰프 LED Del
	USART_SendDataWithPolling(&Dust_Du_Deta, sizeof(Dust_Du_Deta));	
	
	Read_temp = Dual_Amp_Adc.LED_On_Val - Single_Amp_Adc.LED_Off_Val;
	int_val = Read_temp/ 1000 + 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 1000 ;
	int_val = (tmp/ 100 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 100 ;
	int_val = (tmp/ 10 )+ 48;
	USART_SendDataWithPolling(&int_val, 1);
	tmp= Read_temp% 10 ;
	int_val = (tmp/ 1) + 48;
	USART_SendDataWithPolling(&int_val, 1);
	
	USART_SendDataWithPolling(&Tab, sizeof(Tab));
	
	USART_SendDataWithPolling(&End, 4);
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);
	
}

void Bat_Ck(void){

			////베터리  체크///
			
			bat_val = BAT_ADC_Avr(5) ;
			
			if(bat_val < Bat_Low){
				Bat_mode = Bat_Low_mode;
			}
			else if(bat_val >= Bat_high ){
				Bat_mode = Bat_High_mode;
			}
			
						//Uart Test//
			if(uart_debug == uart_debug_On){
				Uart_Out(1);
			}
			else{
				Uart_Set();
			}
			
			//Uart Test end//
			
			LED_G_ON;
			NOP_20us_Delay(250);	// 5ms	
			
		  LED_G_OFF;
			
			system_count = 1;
			
			hw_initial_Wait(10);
}
