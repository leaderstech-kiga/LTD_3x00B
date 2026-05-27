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
#include "math.h"
#include "audio.h"
#include "flash.h"
#include "calib.h"



/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/

//////////////// Start: System state and mode constants ////////////////

/* ---------- System status flag (set by SPI_Memory_Check, read by Audio_Initial in ISR) ---------- */
/* volatile - ISR/main boundary. */
volatile uint8_t SystemStatus;

/* ---------- Main-loop cadence (units = sleep cycles, ~0.192 s each) ---------- */
uint16_t adc_time    = 8 * 5;          /* sensor cadence  (currently empty body, ~7.7 s)  */
uint16_t ck_led_time = 56 * 5 + 12;    /* power LED + Bat_Ck cadence (~56 s)              */

/* ---------- Run-time state ---------- */
uint16_t Al_Stop_key_Count;            /* TIMER2_Int: SW2 long-press counter */
uint16_t system_count   = 0;           /* main-loop sleep tick counter        */
uint16_t Fire_Alarm_LED = 0;           /* TIMER2_Int: alarm LED state         */
uint16_t Bat_Alarm_LED  = 0;           /* TIMER2_Int: low-bat LED state       */
uint8_t  SYS_mode, ADC_mode, Bat_mode;


/* ---------- TIMER2_Int internal counter ---------- */
/* volatile - ISR sets it, read by other paths indirectly. */
volatile uint8_t Timer2_cnt;


#define Dust_mode		1
#define Temp_mode	2

#define Normal_mode	1
#define UART_mode		2
#define In_Fire_al_mode		3
#define Ex_Fire_al_mode		4
#define Al_Stop_mode		6


#define Bat_High_mode		1
#define Bat_Low_mode		2

uint8_t visual_type = 1;	// visual indicator type

//////////////// End:   System state and mode constants ////////////////


//////////////// Start: Debug-mode flags ////////////////

/* ---------- Compile-time constants ---------- */
#define uart_debug_On      1           
#define uart_debug_Off      0

#define Debug_On	1
#define Debug_Off	0

uint8_t  uart_debug_mode = Debug_Off ;
uint8_t  start_uart_debug_mode = Debug_On ;


//////////////// End:   Debug-mode flags ////////////////

//////////////// Start: Audio playback globals ////////////////

/* ---------- Audio streaming globals (shared with TIMER1_Int in audio.c) ---------- */
/* volatile - ISR/main boundary. */
volatile uint16_t Audio_length;
volatile uint16_t Audio_addr;
volatile uint8_t  Audio_start = 0;     /* explicit init - was previously two-def */
volatile uint16_t Audio_max_length;

/* Audio clip table in Flash (code) memory.
 *  Index    Macro                  Addr     Len     Duration (8 kHz 8-bit)
 *    0      AUDIO_CLIP_BOOT        0x0000   10801   ~1.35 s   boot beep
 *    1      AUDIO_CLIP_LOWBAT      0x2BAA   21478   ~2.68 s   low-battery
 *    2      AUDIO_CLIP_FIRE        0x8108   24324   ~3.04 s   fire alarm
 */
code AudioClip_t Audio_Clips[AUDIO_CLIP_COUNT] = {
    /* [AUDIO_CLIP_BOOT]   */ { 0x0000, 10801, 1 },
    /* [AUDIO_CLIP_LOWBAT] */ { 0x2BAA, 21478, 1 },
    /* [AUDIO_CLIP_FIRE]   */ { 0x8108, 24324, 1 },
};

//////////////// End:   Audio playback globals ////////////////

////////////////Start  Debug uart text//////////// 
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
uint8_t Space[1] = " ";
uint8_t Tab[4] = "    ";
uint8_t End[4] = "\n\r";



uint8_t opamp1[5] = "OPA1:";
uint8_t opamp2[5] = "OPA2:";

uint8_t Avr[9] = "Average: ";

uint8_t Dust_Si_Off[7] = "Si_Off:";
uint8_t Dust_Si_On[6] = "Si_On:";
uint8_t Dust_Si_Deta[6] = "Si_Da:";

uint8_t Dust_Du_Off[7] = "Du_Off:";
uint8_t Dust_Du_On[6] = "Du_On:";
uint8_t Dust_Du_Deta[6] = "Si_Da:";

/* Calibration debug labels (see Uart_Out below). */
uint8_t Cal_Norm[6] = "Norm:";
uint8_t Cal_Base[6] = "Base:";
uint8_t Cal_Gain[6] = "Gain:";
uint8_t Cal_Valid[4] = "Cv:";
////////////////End  Debug uart text////////////

//////////////// Start: ADC globals ////////////////

#define VBGR_CV        92u      // VBGR voltage * 100  (chip-specific, calibrate!)
#define ADC_FS         1024u    // 10-bit

#define	System_Ck_Value	40

#define ADC_BUFFER_SIZE   8u   /* element count of ADC_temp_data[] */
#define ADC_BUFFER_COUNT  8u   /* element count passed to ADC_GetDataWithPolling/
                                * Data_Avr/Data_Sorting/Data_TrimmedMean - MUST
                                * match ADC_BUFFER_SIZE. Use direct constant
                                * instead of sizeof() to avoid Keil C51
                                * macro-expansion-context surprises. */
uint16_t ADC_temp_data[ADC_BUFFER_SIZE];


#define TEMP_OFFSET  20
#define TEMP_IDX(t)  ((t) + TEMP_OFFSET)

code uint16_t Temp_Table[140] = {
	/* -20C */ 30, 32, 34, 36, 38, 40, 42, 45, 47, 50, 
	/* -10C */ 52, 55, 58, 61, 64, 67, 71, 74, 78, 82, 
	/*  0C */ 86, 90, 95, 99, 104, 109, 114, 119, 124, 130, 
	/*  10C */ 135, 141, 147, 153, 159, 166, 172, 179, 186, 192, 
	/*  20C */ 199, 207, 214, 221, 229, 236, 244, 252, 261, 269, 
	/*  30C */ 278, 286, 295, 304, 313, 322, 331, 341, 350, 359, 
	/*  40C */ 369, 379, 388, 398, 407, 417, 427, 437, 446, 456, 
	/*  50C */ 466, 476, 485, 495, 504, 514, 524, 533, 542, 552, 
	/*  60C */ 561, 570, 579, 588, 597, 606, 615, 623, 632, 640, 
	/*  70C */ 648, 656, 664, 672, 680, 688, 695, 703, 710, 717, 
	/*  80C */ 724, 731, 738, 744, 751, 757, 763, 769, 775, 781, 
	/*  90C */ 787, 793, 798, 803, 809, 814, 819, 824, 829, 833, 
	/* 100C */ 838, 842,  847,  851,  855,  859,  863,  867,  871,  874,
	/* 110C */ 878, 881,  885,  888,  891,  895,  898,  901,  904,  906
	};

uint16_t ADC_Bat_Val;

uint16_t ADC_Temp_Val;

uint16_t ADC1_Bat_Val;
uint16_t ADC1_Dust_Val;
uint16_t ADC1_On_Dust_Val;
uint16_t ADC1_Off_Dust_Val;
	
uint16_t ADC2_Bat_Val;
uint16_t ADC2_Dust_Val;
uint16_t ADC2_On_Dust_Val;
uint16_t ADC2_Off_Dust_Val;
uint16_t ADC2_Dust_Val_Norm;       /* calib_apply(ADC2_Dust_Val) - per-unit normalized */

/* Fire-alarm trigger: increments while Si_Da_norm exceeds CALIB_ALARM_THRESHOLD,
 * decays one step each sample below the threshold so a single noise spike
 * cannot promote the system to In_Fire_al_mode. */
#define FIRE_TRIGGER_COUNT     3u
uint8_t  fire_count = 0;

//////////////// End:   ADC globals ////////////////
	

/* Public Variable -----------------------------------------------------------*/
/* Public Function -----------------------------------------------------------*/

/**********************************************************************
 * @brief   main - standard C program entry point.
 * @param   None
 * @return  None
 **********************************************************************/
void main(void)
{

	//LDO_OFF;
	//CVDD_OFF;
	//AUDIO_OFF;
	system_count = 0;

	hw_initial_Wait(10);

 SystemStatus = SPI_Memory_Check();

	ADC_mode = Check_System();

	hw_initial_Wait(10);
	system_count = 0;

	SYS_mode =  Normal_mode;

	/* Per-unit calibration: load saved {baseline, gain_Q8} from DataFlash
	 * into the RAM globals g_calib_*. If no valid record is found the
	 * defaults (baseline=0, gain=1.0) leave readings uncompensated so
	 * detection still works for an uncalibrated unit (the operator can
	 * always force calibration with the SW2-held-at-boot entry below). */
	calib_load();

	/* SW2 held at boot -> production calibration mode. calib_run_procedure
	 * never returns; the operator power-cycles to leave it. This takes
	 * priority over the legacy UART debug entry. */
	Port_SetInputpin(PORT1, PIN2, 1);
	Delay_ms(300);
	if (Port_GetInputpinValue(PORT1, PIN2) == 0) {
		calib_run_procedure();
		/* never returns */
	}

	if(start_uart_debug_mode == Debug_On){
		Start_Debug_Mode();
	}
	
	while(1)
	{

		// Arm RESET-mode WDT (~2 s timeout) at the top of every loop
		// iteration for active-code hang protection. BeforeStop() swaps
		// it back to INTERRUPT mode before Stop() so the wake timer
		// still works.
		WD_Reset();

	// Power LED + battery check every 56 s
	//////////////////////////////////////////////////////////////////////////////////////////

		if(system_count%ck_led_time == 0){
		
			hw_initial_Wait(10);
			LED_G_ON;
			Delay_ms(5);
		  LED_G_OFF;
			hw_initial_Wait(10);
			
		}
	//////////////////////////////////////////////////////////////////////////////////////////
				
	//ADC check start 8 
	//////////////////////////////////////////////////////////////////////////////////////////		
		if(system_count%adc_time == 0){

			// system
			hw_initial_Wait(10);

			LED_G_ON;
			Delay_ms(5);
			LED_G_OFF;

			if(ADC_mode == Dust_mode){

				/* Dust read + per-unit calibration + fire-detect counter.
				 *   ADC2_Dust_Val      = raw (Si_On - Si_Off) from Dust_ADC_2AMP
				 *   ADC2_Dust_Val_Norm = ((raw - baseline) * gain_Q8) >> 8
				 * fire_count increments while normalized value is above the
				 * fixed alarm threshold and decays by one each sample below,
				 * so promotion to In_Fire_al_mode requires sustained smoke. */
				ADC2_Dust_Val      = Dust_ADC_2AMP();
				ADC2_Dust_Val_Norm = calib_apply(ADC2_Dust_Val);

				if (ADC2_Dust_Val_Norm > CALIB_ALARM_THRESHOLD) {
					if (fire_count < 0xFF) {
						fire_count++;
					}
					if (fire_count >= FIRE_TRIGGER_COUNT &&
					    SYS_mode == Normal_mode) {
						SYS_mode   = In_Fire_al_mode;
						fire_count = 0;
					}
				}
				else if (fire_count > 0) {
					fire_count--;
				}
			}
			else if(ADC_mode == Temp_mode){

		}

		hw_initial_Wait(10);
	}
	//ADC check start 8  end
	//////////////////////////////////////////////////////////////////////////////////////////

	// Internal fire-alarm state - exits when Timer2 ISR detects SW2 held
	// for ~0.5 s and transitions SYS_mode to Al_Stop_mode.
	//////////////////////////////////////////////////////////////////////////////////////////
	if((SYS_mode ==  In_Fire_al_mode)){
		
		char In_charge_wait = 4;
		
		// system init / settle before alarm output
		hw_initial_Wait(10);

		// Drive EMR_IO output high to signal external alarm equipment
		// (e.g., a relay or annunciator) on CON3.
		Port_SetOutputpin(PORT1, PIN0, 0);
		Delay_ms(100);
		P10 = 1;
			

		
	// system  
	hw_initial_Wait(10);
	
}
	
	//   end
	//////////////////////////////////////////////////////////////////////////////////////////

  // Manual test trigger: Normal mode + SW2 held -> forced fire-alarm test.
	//////////////////////////////////////////////////////////////////////////////////////////
	if((SYS_mode ==  Normal_mode) && (Port_GetInputpinValue(PORT1, PIN2)  == 0)){

		// system
		hw_initial_Wait(10);


		Port_SetOutputpin(PORT1, PIN0, 0);
		Delay_ms(100);
		P10 = 1;
		
		T2_init();
		Fire_Alarm_LED = 1;
		Bat_Alarm_LED = 0;
		Timer2_Start();
		
		//   1   
		
		//Fire_Alarm();
		
		Play_Clip(AUDIO_CLIP_FIRE);
		
		GLOBAL_INTERRUPT_DIS();     

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
		
		// system  
			hw_initial_Wait(10); 
		
	}
	
	//   end
	//////////////////////////////////////////////////////////////////////////////////////////		

	//Bat_Low_mode check start
	//////////////////////////////////////////////////////////////////////////////////////////
	if(Bat_mode == Bat_Low_mode){
		

	}		
	//Bat_Low_mode check end
	//////////////////////////////////////////////////////////////////////////////////////////

	//Stop_mode Start
	//////////////////////////////////////////////////////////////////////////////////////////
	
	if(SYS_mode  == Al_Stop_mode){

	uint8_t  i;
	uint16_t j;
	int break_mode = 0;
	
	// system  
	hw_initial_Wait(10);
	
	Play_Clip(AUDIO_CLIP_BOOT);	
	
	Timer2_Stop();
	
	Port_SetInputpin(PORT1, PIN0, 0);  // EM IO 
	P10 = 0;
	
	SYS_mode  = Normal_mode;
	
	LED_G_ON;
	NOP_20us_Delay(25000); // 500ms
	LED_G_OFF;
	WD_Reset();
	NOP_20us_Delay(25000); // 500ms

	/* Refresh active-mode WDT (2s) on every iteration so a long SW2 press
	 * does not cause a spurious reset. */
	while(1){
		WD_Reset();
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
			WD_Reset();                        /* per-100ms dog kick */
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
		/* 39.4 s inner wait - refresh active-mode WDT (2 s) on every
		 * 100 ms iteration so the SW2 acknowledge wait runs to completion. */
		for(j=0;j<394;j++){
			WD_Reset();
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

	/* Second SW2-release polling loop. */
	while(1){
		WD_Reset();
		if(Port_GetInputpinValue(PORT1, PIN2)  == 1){
				break;
			}
	}

		// system
	hw_initial_Wait(10);
}
	
  //Stop_mode End
	//////////////////////////////////////////////////////////////////////////////////////////



  // EMO   
	//////////////////////////////////////////////////////////////////////////////////////////
	if((SYS_mode ==  Normal_mode) && (Port_GetInputpinValue(PORT1, PIN0)  == 1)){

		// system
		hw_initial_Wait(10);

		Play_Clip(AUDIO_CLIP_FIRE);
		
		GLOBAL_INTERRUPT_DIS();     

		NOP_20us_Delay(1000);
		GLOBAL_INTERRUPT_EN();  
		

		// system  
		hw_initial_Wait(10);
		
	}
	// EMO   end
	//////////////////////////////////////////////////////////////////////////////////////////
	
	system_count++;

		// Sleep cycle: BeforeStop arms WDT as INTERRUPT-mode wake timer
		// (WDTDR=2, ~0.192 s) for button-polling resolution.
		// After wake, the top-of-loop WD_Reset() switches WDT back to
		// RESET mode for the next iteration's hang protection.
	BeforeStop();
	GLOBAL_INTERRUPT_EN();
	Stop();
	AfterStop();

	}
}

void Start_Debug_Mode(void){
	
	Port_SetInputpin(PORT1, PIN2, 1);  // key input & mode0 -> button input; held at power-up == enter UART mode
	Delay_ms(300);														// settle time before ADC mode sampling

	if(Port_GetInputpinValue(PORT1, PIN2)  == 0){
		SYS_mode = UART_mode;
	}
	else{
		SYS_mode = Normal_mode;
	}

	LED_G_ON;
	Delay_ms(5);
	LED_G_OFF;

	// Wait until the button is released, then enter UART debug loop
	if(SYS_mode == UART_mode){
			while(1){
				LED_G_ON;
				if(Port_GetInputpinValue(PORT1, PIN2)  == 1){
					break;
				}
			}

			LED_G_OFF;

			// ===== UART debug mode infinite loop =====
			
			while(1){

				LED_R_ON;
				
				ADC_Bat_Val = Get_Bat_Voltage_cV();
				ADC_Temp_Val = TEMP_ADC();
				ADC1_Dust_Val = Dust_ADC_1AMP();
				ADC2_Dust_Val = Dust_ADC_2AMP();
				ADC2_Dust_Val_Norm = calib_apply(ADC2_Dust_Val);

				Uart_Out();

				Delay_ms(250);
				LED_R_OFF;
				Delay_ms(250);
				WD_Reset();
			}

		}
}



//////////////// Start: System initialization helpers ////////////////

void hw_initial(void) {
	
	if(Audio_start == 1) {
			SLAVEDESELECT;  
			AUDIO_OFF;          
			Timer0_Stop();     
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
	P1PU = 0x04;               		// 0 : Disable, 1 : Enable (Pull-up) // SW2 test-switch pull-up
	//P1PU = 0x0;               		// 0 : Disable,	1 : Enable (Pull-up)
	P1IO = 0x32;                 	// 0 : Input,	1 : Output
	

	//P12DB = 0x00;											//P1/P2 Debounce disable
	//P1FSRH = 0x08;                                                          // P1[5]: AN6, others: normal I/O
	//P1FSRL = 0x00;                                                          // P1[3:0]: Normal I/O
	/* P15 = LDO23 alternate function so LDO_ON drives the 2.32 V output
	 * out to the external AVREF / divider network. Boot sleep current
	 * was previously verified not to depend on this bit (the 90 uA
	 * leakage tracked to AMPCR0 "Always" mode, since fixed). LDOCR is
	 * still cleared below so the LDO is OFF at boot until LDO_ON is
	 * called. */
	P1FSRH  = 0
		| ( 1 << 2)       //P15  // 0 : I/O,	1 : LDO23,   2 : AN6,	3 : (SDA)
		| ( 0 << 0);      //P14  // 0 : I/O (EINT11),	1 : T1O/PWM1O,   2 : SCL,	3 : Not used		// /AUD ON

	LDOCR = 0x00;     /* LDO block OFF at boot until ADC code calls LDO_ON */
		
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
	/* OPAMP block disabled at boot. AMP_AUTO_DIS = 1 in main.h puts the
	 * OPAMP into "auto-disable" mode so the analog block goes idle when
	 * ADC is not converting - critical for ~2 uA sleep current.
	 * If dust sensing is ever re-introduced, set the relevant enable bits
	 * (AMPCR1 b7=Amp1, b3=Amp0) at the call site, then clear them after. */
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


/**********************************************************************
 * @brief		Delay_ms
 * @param   	None
 * @return	None
 **********************************************************************/
void Delay_ms(uint16_t msec)
{

	uint16_t i;

	for(i=0;i<msec;i++){
		NOP_20us_Delay(50);
	}

}

/**********************************************************************
 * @brief		hw_initial_Wait
 * @param   	None
 * @return	None
 **********************************************************************/
//     ms . 
void hw_initial_Wait(int msec){
	
	GLOBAL_INTERRUPT_DIS();     
	hw_initial();
	
	CVDD_OFF;
	Port_SetOutputpin(PORT1, PIN3, 0); // CS 

	Delay_ms(msec);
	
		/* Enable INT */
	GLOBAL_INTERRUPT_EN();  
	
}

uint8_t SPI_Memory_Check(void){
	
	uint8_t SystemStatus_Read;
	uint8_t deviceID[3];
	
	  // SPI Memory Check
	// CVDD_ON drives P20 = 1, which is the C_VDD signal supplying VCC to
	// U3 (MX25L1006E). P20 is a regular GPIO output (~50 ohm), so the
	// flash supply rail (C14 decoupling) charges through that impedance.
	CVDD_ON;             // P20 = 1 -> C_VDD high -> flash VCC ON

	// Wait for flash VCC to settle after C_VDD goes high (10 uF cap
	// on the rail charges through the P20 GPIO output impedance).
	NOP_20us_Delay(500);  // ~10 ms settle time for 10 uF flash VCC

	// SPI Initial
	USART_SPI_Initial(SPI_MASTER_MODE, 500000, SPI_MSB, SPI_CPOL_LOW, SPI_CPHA_1EDGE, SPI_TX_RX_MODE, SPI_SS_HW_DISABLE);	 // 500000

	Get_Identification(deviceID);

	// Match expected MX25L1006E JEDEC ID + verify no SPI timeout
	// (so a 0xFF-on-each-byte timeout doesn't look like a partial match).
	if( (deviceID[0]==0xC2)&&(deviceID[1]==0x20)&&(deviceID[2]==0x11) && (SPI_Timeout == 0) )
	{
		SystemStatus_Read=1;
	}
	else{
		SystemStatus_Read=0;
	}

	CVDD_OFF;
	
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

void TIMER2_Int(void) interrupt T2_MATCH_VECT
{
	
	char Gled_mode, GLED_CNT;
	
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


//////////////// End: System initialization helpers ////////////////



//////////////// Start: Watchdog / sleep helpers ////////////////

//----------------------------------------------------------------------------
void BeforeStop(void)
{
	/* Defensive OPAMP disable before sleep.
	 * Root cause was AMP_AUTO_DIS=0 in main.h + missing DELAY_TIME macro
	 * which selected the OPAMP-enable branch in hw_initial. Both are now
	 * fixed, but keep this write as a safety net in case future code
	 * enables OPAMP and forgets to clear it before sleep. */
	AMPCR1 = 0x00;
	LDOCR  = 0x00;     /* defensive: LDO OFF in case ADC code forgot LDO_OFF */

	/* Defensive: drop audio amp / flash VCC / LEDs in case anything was
	 * left on by the previous active-code path. */
	AUDIO_OFF;
	CVDD_OFF;
	LED_G_OFF;
	LED_R_OFF;

	OSCCR = 0x84;       /* LFIRC enable + HIRC disable */
	WDTCR = 0xA2;       /* WDT enable, INTERRUPT mode, LFIRC clock */
	WDTDR = 2;          /* ~0.192 s wake period */
	IE3 |= 0x08;        /* WDT interrupt enable */
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

/**********************************************************************
 * @brief   WD_Reset - configure WDT in RESET mode for hang protection.
 *
 *   Purpose: while the MCU is awake and executing code, the WDT must be
 *   armed in RESET mode so that any hang (stuck SPI poll, infinite loop,
 *   ISR deadlock, EMI-induced PC corruption, etc.) triggers an automatic
 *   MCU reset rather than leaving the device silently dead. Safety-
 *   critical for a fire detector.
 *
 *   The complementary wake-from-Stop timer (BeforeStop) keeps WDT in
 *   INTERRUPT mode with WDTDR=2 (~0.192 s). On every wake we call this
 *   function to switch WDT into RESET mode with the longer timeout;
 *   BeforeStop will switch it back before the next Stop.
 *
 *   Behaviour:
 *     - Clock source : WDTRC (~3.072 MHz internal, independent of MCLK)
 *     - Timeout      : WDTDR = 30  ->  approximately 2 seconds.
 *                      Long enough to cover normal active code paths
 *                      (Dust_ADC_Single, Delay_ms(100), hw_initial_Wait,
 *                      etc.); shorter than the time a real hang would
 *                      take to be noticeable. Longer blocking calls
 *                      (Audio_Run audio playback, Fire_Alarm loop) must
 *                      refresh the counter by calling WD_Reset()
 *                      themselves - see those functions below.
 *     - Mode         : RESET (WDTCR bit 6 set) -> MCU reset on overflow.
 *
 *   Note: WDTCR also has bit 5 (WDTCL) set, which clears the counter on
 *   every write, so each WD_Reset() call effectively "kicks the dog".
 **********************************************************************/
void WD_Reset(void)
{
	WDTDR = 30;												// ~2 sec active-mode timeout
	//WDTCR = 0xA3;											// set WDT enable , clock source LFIRC, WDT interrupt
	//WDTCR = 0xE5;											// WDT enable, RESET mode (bit6=1), WDTRC clock, clear counter
	WDTCR = 0xE3;     /* 11100011: WDTEN=1, RESET mode, WDTCL=1, WDTCK=01 (LFIRC) */
	IE3 |= 0x08;											// WDT interrupt enable (no effect in reset mode but harmless)
}

void WDT_Int(void) interrupt WDT_VECT
{
	WDTCR &= ~0x01;
}

//////////////// End:   Watchdog / sleep helpers ////////////////

//////////////// Start: Audio helpers ////////////////

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
 * @brief		Audio_Run
 * @param   	None
 * @return	None
 **********************************************************************/
void Audio_Run(uint16_t Address , uint16_t Length,  uint8_t Run_time){
		 
	CVDD_ON;
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

	// Audio clips run ~3 seconds (Audio3_length=24324 samples @ 8 kHz =
	// 3.04 s) which exceeds the 2-s active-mode WDT timeout. Refresh the
	// watchdog inside this polling loop so playback is not aborted by a
	// false WDT reset. The CPU is otherwise idle here while the
	// TIMER1_Int ISR feeds samples in the background.
	//
	// Stall detector.
	//
	// The WD_Reset() inside this loop also means the active-mode WDT will
	// NEVER fire here, even if the audio ISR stops making progress
	// (Audio_length not incrementing because of e.g. SPI fault, SPI clock
	// glitch, TIMER1_Int disabled). The loop would then spin forever and
	// the device would appear hung from the outside.
	//
	// We track Audio_length progress and bail out if it does not advance
	// for ~200 ms. At ~125 us per audio sample the ISR should increment
	// Audio_length about 1600 times during 200 ms, so 200 ms of zero
	// progress is unambiguous evidence of a stall.
	{
		uint16_t prev_length  = 0xFFFF;   /* sentinel - guaranteed != on first iter */
		uint16_t stall_count  = 0;

		while (Audio_length < Audio_max_length) {
			WD_Reset();                   /* kick dog during long playback */

			if (Audio_length == prev_length) {
				/* No progress this iteration. ~4 us per while iter at 4 MHz,
				 * so 50000 iterations ~= 200 ms of no progress = stall. */
				if (++stall_count > 50000) {
					break;
				}
			} else {
				prev_length = Audio_length;
				stall_count = 0;
			}
		}
	}

	CVDD_OFF;
	AUDIO_OFF;
	
	//Port_SetOutputpin(PORT1, PIN0, 0);
	//P10 = 1;
	
	Audio_start = 0;
}

/**********************************************************************
 * @brief   Play_Clip - play one of the clips in Audio_Clips[].
 *          Out-of-range clip_id is silently ignored.
 * @param   clip_id   index into Audio_Clips[] (AUDIO_CLIP_BOOT etc.)
 **********************************************************************/
void Play_Clip(uint8_t clip_id)
{
    if (clip_id >= AUDIO_CLIP_COUNT) return;
    Audio_Run(Audio_Clips[clip_id].address,
              Audio_Clips[clip_id].length,
              Audio_Clips[clip_id].runtime);
}



//////////////// End: Audio helpers ////////////////


//////////////// Start: UART helpers ////////////////


void Uart_Out(void){

	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
						
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
	
	if(ADC_mode == Dust_mode){
		
		if(visual_type == 1){
			USART_SendDataWithPolling(&Visu_MODE, sizeof(Visu_MODE));
		}
		else{
			USART_SendDataWithPolling(&Dust_MODE, sizeof(Dust_MODE));
		}
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Bat, sizeof(Bat));
		Uart_Out_Int(ADC_Bat_Val);
		
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Dust_Si_Off, sizeof(Dust_Si_Off));
		Uart_Out_Int(ADC1_Off_Dust_Val);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Dust_Si_On, sizeof(Dust_Si_On));
		Uart_Out_Int(ADC1_On_Dust_Val);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Dust_Si_Deta, sizeof(Dust_Si_Deta));
		Uart_Out_Int(ADC1_Dust_Val);
		
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Dust_Du_Off, sizeof(Dust_Du_Off));
		Uart_Out_Int(ADC2_Off_Dust_Val);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Dust_Du_On, sizeof(Dust_Du_On));
		Uart_Out_Int(ADC2_On_Dust_Val);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Dust_Du_Deta, sizeof(Dust_Du_Deta));
		Uart_Out_Int(ADC2_Dust_Val);

		/* Per-unit calibration trace: normalized Si_Da + the offset/gain
		 * pair that calib_apply() is using right now, plus a valid flag
		 * (0 = uncalibrated / running on defaults, 1 = loaded from DataFlash). */
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Cal_Norm, sizeof(Cal_Norm));
		Uart_Out_Int(ADC2_Dust_Val_Norm);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Cal_Base, sizeof(Cal_Base));
		Uart_Out_Int(g_calib_baseline);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Cal_Gain, sizeof(Cal_Gain));
		Uart_Out_Int(g_calib_gain_Q8);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Cal_Valid, sizeof(Cal_Valid));
		Uart_Out_Int((uint16_t)g_calib_valid);

	}
	else if(ADC_mode == Temp_mode){
		USART_SendDataWithPolling(&Temp_MODE, sizeof(Temp_MODE));
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Bat, sizeof(Bat));
		Uart_Out_Int(ADC_Bat_Val);
		USART_SendDataWithPolling(&Space, sizeof(Space));
		USART_SendDataWithPolling(&Temp, sizeof(Temp));
		Uart_Out_Int(ADC_Temp_Val);
	}
	
	USART_SendDataWithPolling(&End, 4);
	
	Port_SetAlterFunctionpin(PORT1, PIN2, 0);
}

void Uart_Out_Int(uint16_t Value){

	uint16_t int_val;
	uint8_t tmp;
	
	if(Value > 9999){
		Value = 9999;
	}
	
	tmp = Value / 1000 + 48;
	USART_SendDataWithPolling(&tmp, 1);
	int_val= Value % 1000 ;
	tmp = (int_val/ 100 )+ 48;
	USART_SendDataWithPolling(&tmp, 1);
	int_val= Value % 100 ;
	tmp = (int_val/ 10 )+ 48;
	USART_SendDataWithPolling(&tmp, 1);
	int_val= Value % 10 ;
	tmp = (int_val/ 1) + 48;
	USART_SendDataWithPolling(&tmp, 1);
}


//////////////// End:   UART helpers ////////////////


//////////////// Start: ADC helpers ////////////////

/**********************************************************************
 * @brief       Data_Avr - simple arithmetic mean over `count` ADC samples
 * @param       adc_data - pointer to array of uint16_t samples
 * @param       count    - number of elements (NOT bytes!)
 * @return      average value, or 0 if count==0
 **********************************************************************/
uint16_t Data_Avr(uint16_t *adc_data, uint8_t count){
	
	uint32_t temp = 0;
	uint8_t i;
	
	if(count < 1){
		return 0;
	}
	
	for(i=0; i<count ; i++){
		temp = temp + adc_data[i];
	}
	
	temp = temp / count;
	
	return temp;
	
}

void Data_Sorting(uint16_t *adc_data, uint8_t count)
{
	uint8_t i,j;
	uint16_t temp;

	if(count < 1){
		return;
	}
	
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

uint16_t Data_TrimmedMean(uint16_t *adc_data, uint8_t count){

	uint32_t temp = 0;
	uint8_t i;
	
	if(count < 3){
		return 0;
	}
	
	
	Data_Sorting(adc_data, count);
	
	for(i=1; i<(count -1) ; i++){
		temp = temp + adc_data[i];
	}
	
	temp = temp / (count -2);
	
	return temp;
	
}

/**********************************************************************
 * @brief       Check_System - autodetect heat-vs-smoke variant.
 *              Probes AN3 with LDO on vs LDO off. A heat (NTC) variant
 *              shows a large delta; a smoke (photoelectric) variant does
 *              not. Returns Temp_mode or Dust_mode accordingly.
 **********************************************************************/
uint16_t Check_System(void)
{
	int V25_On_adc_data= 0 , V25_Off_adc_data= 0;
	uint16_t System_Mode_Ck;

	// OPAMP outputs disabled; route AN3/AN2/AN1 to ADC pins.
	P0FSRL  = 0
		| ( 2 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC
		| ( 2 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used							// Temperature_ADC
		| ( 2<< 2)        //P01     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				//
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD

	OP0_Enable(FALSE);
	OP1_Enable(FALSE);
	LDO_ON;
	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	 // INTERNAL_REF + LDO_ON sequence isolates heat-variant signal
	ADC_SelectChannel(ADC_CH3);
	
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes
	
	LDO_OFF;

	V25_On_adc_data = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;
	
	Delay_ms(20);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes
	V25_Off_adc_data = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;

	if( abs(V25_On_adc_data - V25_Off_adc_data) > System_Ck_Value){
		System_Mode_Ck = Temp_mode;
	}
	else{
		System_Mode_Ck = Dust_mode;
	}

	/* Cleanup: disable ADC + restore P0FSRL to hw_initial baseline so the
	 * caller does not inherit an active ADC block or AN3/AN2/AN1 pin mode. */
	ADCCRL = 0x00;
	P0FSRL = 0
		| (1 << 6)        /* P03 = OP0OUT (hw_initial baseline) */
		| (1 << 4)        /* P02 = OP1P                          */
		| (1 << 2)        /* P01 = OP1N                          */
		| (0 << 0);       /* P00 = I/O                           */

	return System_Mode_Ck;
}


/**********************************************************************
 * @brief       TEMP_ADC - read NTC thermistor through AN3 with LDO23 on
 *              as reference. Looks up the result in Temp_Table[] and
 *              returns temperature in degrees C (clamped 0..100).
 **********************************************************************/
uint16_t TEMP_ADC(void)
{
	int Temp_adc_data= 0 , Temp_Data;
	int i;

	// Route AN3 (NTC divider) to ADC; OPAMPs disabled.
	P0FSRL  = 0
		| ( 2 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC
		| ( 2 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					    // Temperature_ADC
		| ( 2<< 2)        //P01     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD

	OP0_Enable(FALSE);
	OP1_Enable(FALSE);
	LDO_ON;
	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_LDO_REF, ADC_LSB);	 // LDO_REF: NTC ratiometric measurement immune to LDO drift
	ADC_SelectChannel(ADC_CH3);
	
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes
	
	LDO_OFF;

	Temp_adc_data = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;
	
	for(i=TEMP_OFFSET;i<(TEMP_OFFSET + 100);i++){
		if(Temp_Table[i] > Temp_adc_data){
			break;
		}
	}
	
	if(i == (TEMP_OFFSET + 100)){
		Temp_Data = 100;
	}
	else if (i==TEMP_OFFSET){
		Temp_Data = 0;
	}
	else{
		Temp_Data = i -TEMP_OFFSET - 1 ;
	}

	/* Cleanup: disable ADC + restore P0FSRL to hw_initial baseline. */
	ADCCRL = 0x00;
	P0FSRL = 0
		| (1 << 6)        /* P03 = OP0OUT (hw_initial baseline) */
		| (1 << 4)        /* P02 = OP1P                          */
		| (1 << 2)        /* P01 = OP1N                          */
		| (0 << 0);       /* P00 = I/O                           */

	return Temp_Data;
}

/**********************************************************************
 * @brief       Get_Bat_Voltage_cV - measure battery voltage in units of
 *              0.01 V (e.g. return value 304 == 3.04 V).
 *              Reads the internal VBGR reference against VDD as ADC ref
 *              (ratiometric trick): VBGR_CV is the calibrated bandgap *100,
 *              ADC_FS = 1024 (10-bit FS), and the formula
 *                  Vbat_cv = (VBGR_CV * ADC_FS) / adc_reading
 *              gives Vbat in centivolts.
 **********************************************************************/
uint16_t Get_Bat_Voltage_cV(void)   // unit: 0.01 V
{
    uint32_t adc_avg;
    uint32_t vbat_cv;


    ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);   // INTERNAL_REF reads VBGR against VDD - ratiometric Vbat math
    ADC_SelectChannel(ADC_VBGR);
    Delay_ms(1);

    ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes

	  adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;

    if (adc_avg == 0) {
        ADCCRL = 0x00;            /* cleanup before early return */
        return 0;
    }

    vbat_cv = ((uint32_t)VBGR_CV * ADC_FS) / adc_avg;

    // sanity clamp (optional)
    if (vbat_cv > 400) vbat_cv = 400;   // ignore impossible >4.00 V readings

    /* Cleanup: disable ADC module. (P0FSRL untouched - this function does
     * not change pin functions.) */
    ADCCRL = 0x00;

    return (uint16_t)vbat_cv;
}

/**********************************************************************
 * @brief		Dust_1ADC
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t Dust_ADC_1AMP(void)
{
	uint16_t Dust_adc_data = 0;
	uint32_t adc_avg;
	uint32_t op;

	/* Pin function map - same as 2AMP for consistency */
	P0FSRL  = 0
		| ( 1 << 6)       /* P03 : OP0OUT (sensing OP-amp output to ADC) */
		| ( 1 << 4)       /* P02 : OP1P  (unused here)                    */
		| ( 2 << 2)       /* P01 : AN1   (battery ADC channel)            */
		| ( 1 << 0);      /* P00 : OP1OUT (test point)                    */

	OPAMP_Initial(GAIN1_DIS, GAIN0_DIS, chp_125KHz);
	OP0_Enable(TRUE);
	OP1_Enable(FALSE);

	ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);
	ADC_SelectChannel(ADC_VBGR);
	Delay_ms(1);

	/* 1) VBGR measurement -> ratiometric battery compensation factor.
	 * ADC1_Bat_Val ~= 300 at VBAT=3V; used to scale the OP0OUT readings
	 * so dust results stay constant across battery voltage drift. */
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
	adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);
	if (adc_avg == 0) {
		goto cleanup;          /* divide-by-zero guard */
	}
	op  = 92;
	op *= 1024;
	op /= adc_avg;
	ADC1_Bat_Val = op;

	/* 2) LED ON - chamber sensing.
	 * No divide-by-zero possible here (multiplication only, /1024 is the
	 * only division). adc_avg=0 is a legitimate dark-chamber reading. */
	LDO_ON;
	ADC_SelectChannel(ADC_OP0OUT);
	Delay_ms(1);

	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
	adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);

	LDO_OFF;
	Delay_ms(1);

	op  = ADC1_Bat_Val;
	op *= adc_avg;
	op /= 1024;
	ADC1_On_Dust_Val = op;

	/* 3) LED OFF - ambient reference (still on ADC_OP0OUT) */
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
	adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);

	op  = ADC1_Bat_Val;
	op *= adc_avg;
	op /= 1024;
	ADC1_Off_Dust_Val = op;

	/* Underflow guard: if optical noise pushes Off > On the uint16_t
	 * subtraction would wrap to ~65535 and trigger a false fire alarm.
	 * Treat that case as zero dust signal. */
	if (ADC1_On_Dust_Val > ADC1_Off_Dust_Val) {
		Dust_adc_data = ADC1_On_Dust_Val - ADC1_Off_Dust_Val;
	} else {
		Dust_adc_data = 0;
	}

cleanup:
	/* Disable ADC + OPAMP + restore P0FSRL to hw_initial baseline so the
	 * next call (or BeforeStop) inherits a clean state. */
	ADCCRL = 0x00;
	OP0_Enable(FALSE);
	OP1_Enable(FALSE);
	AMPCR1 = 0x00;
	P0FSRL = 0
		| (1 << 6)        /* P03 = OP0OUT (baseline) */
		| (1 << 4)        /* P02 = OP1P              */
		| (1 << 2)        /* P01 = OP1N              */
		| (0 << 0);       /* P00 = I/O               */

	return Dust_adc_data;
}


/**********************************************************************
 * @brief		Dust_ADC_2amp
 * @param   	None
 * @return	None
 **********************************************************************/

uint16_t Dust_ADC_2AMP(void)
{
	uint16_t Dust_adc_data = 0;
	uint32_t adc_avg;
	uint32_t op;

	/* Pin function map - same as 1AMP; OP-amp chain OP0 -> OP1 -> ADC */
	P0FSRL  = 0
		| ( 1 << 6)       /* P03 : OP0OUT (first stage out)             */
		| ( 1 << 4)       /* P02 : OP1P   (second stage non-inv input)  */
		| ( 2 << 2)       /* P01 : AN1    (battery ADC channel)         */
		| ( 1 << 0);      /* P00 : OP1OUT (final amp output, test pt)   */

	if (visual_type == 1) {
		OPAMP_Initial(GAIN1_X30, GAIN0_DIS, chp_125KHz);
	} else {
		OPAMP_Initial(GAIN1_X15, GAIN0_DIS, chp_125KHz);
	}

	/* Two-stage amp sequence: enable OP0 first, wait 200 us per User
	 * Manual Figure 43 NOTE 2 ("turn on the first OP-AMP and provide a
	 * delay of at least 200 us before turn on the second OP-AMP"), then
	 * enable OP1. Without this delay the first sample is unreliable. */
	OP0_Enable(TRUE);
	NOP_20us_Delay(10);          /* ~200 us OP0 stabilization */
	OP1_Enable(TRUE);

	ADC_Initial(ADC_CLK_2M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);
	ADC_SelectChannel(ADC_VBGR);
	Delay_ms(1);

	/* 1) VBGR measurement -> ratiometric battery compensation */
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
	adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);
	if (adc_avg == 0) {
		goto cleanup;            /* divide-by-zero guard */
	}
	op  = 92;
	op *= 1024;
	op /= adc_avg;
	ADC2_Bat_Val = op;

	/* 2) LED ON - chamber sensing through 2-stage amp.
	 * No divide-by-zero possible (multiplication only, /1024 is the only
	 * division). adc_avg=0 is a legitimate dark-chamber reading. */
	LDO_ON;
	ADC_SelectChannel(ADC_OP1OUT);
	Delay_ms(1);

	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
	adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);

	LDO_OFF;
	Delay_ms(1);

	op  = ADC2_Bat_Val;
	op *= adc_avg;
	op /= 1024;
	ADC2_On_Dust_Val = op;

	/* 3) LED OFF - ambient reference (still on ADC_OP1OUT) */
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);
	adc_avg = Data_TrimmedMean(ADC_temp_data, ADC_BUFFER_COUNT);

	op  = ADC2_Bat_Val;
	op *= adc_avg;
	op /= 1024;
	ADC2_Off_Dust_Val = op;

	/* Underflow guard - same rationale as Dust_ADC_1AMP */
	if (ADC2_On_Dust_Val > ADC2_Off_Dust_Val) {
		Dust_adc_data = ADC2_On_Dust_Val - ADC2_Off_Dust_Val;
	} else {
		Dust_adc_data = 0;
	}

cleanup:
	ADCCRL = 0x00;
	OP0_Enable(FALSE);
	OP1_Enable(FALSE);
	AMPCR1 = 0x00;
	P0FSRL = 0
		| (1 << 6)        /* P03 = OP0OUT (baseline) */
		| (1 << 4)        /* P02 = OP1P              */
		| (1 << 2)        /* P01 = OP1N              */
		| (0 << 0);       /* P00 = I/O               */

	return Dust_adc_data;
}

//////////////// End:   ADC helpers ////////////////






