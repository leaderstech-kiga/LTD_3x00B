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


/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////////////////////

//////////////// Start: Audio playback globals ////////////////
// Audio Data : 8000bps , 8bit
// (FIX C2) ISR (TIMER1_Int in audio.c) shared variables MUST be volatile.
//          Without volatile, Keil C51 optimizer may cache the value in a register
//          and miss ISR-side updates. Adding volatile to all 5 ISR-shared globals.
// ORIG: uint16_t Audio_length;
// ORIG: uint16_t Audio_addr;
// ORIG: uint8_t Audio_start_address;
// ORIG: uint16_t Audio_max_length;
volatile uint16_t Audio_length;
volatile uint16_t Audio_addr;
volatile uint8_t  Audio_start_address;
volatile uint16_t Audio_max_length;
/* [BUG-06 FIX] Duplicate declaration removed - SystemStatus already declared at line 168 */
/* BEFORE: uint8_t SystemStatus; */
/* uint8_t SystemStatus; */

uint8_t EM_AL_Status = 0;
// (FIX C2) ISR(TIMER1_Int)/main shared flag - volatile required
// ORIG: uint8_t Audio_start =0;
volatile uint8_t Audio_start = 0;

uint16_t  Audio1_Address  = 0x0;
uint16_t  Audio1_length  = 10801;
uint8_t  Audio1_runtime  = 1;
uint16_t  Audio2_Address  = 0x2baa;
uint16_t  Audio2_length  = 21478;
uint8_t  Audio2_runtime  = 1;
uint16_t  Audio3_Address  = 0x8108;
uint16_t  Audio3_length  = 24324;
uint8_t  Audio3_runtime  = 1;
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
////////////////End  Debug uart text//////////// 

//////////////// Start: System state and mode constants ////////////////
uint8_t deviceID[3];
// (FIX C2) SystemStatus is read by Audio_Initial() (audio.c) - shared with ISR context.
//          Adding volatile for optimizer safety.
// ORIG: uint8_t SystemStatus;
volatile uint8_t SystemStatus;

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

uint8_t visual_type = 0;	// visual indicator type

// Tick counters - one tick == 0.512 s (BeforeStop WDT wake period; WDTDR=7).
// The short tick is required so the button on P12 (which has no EINT
// wake-from-Stop capability) can be polled within the 0.5 s spec.
// Sensor work and LED blinks are throttled by counter to keep the
// original 8 / 56 / 280 s cadence on top of the 0.512 s base tick.
//
//   adc_time     = 16  : every  16 wakes ( 8.19 s)  -> green LED heartbeat
//   ck_led_time  = 112 : every 112 wakes (57.3 s)   -> red LED status blink
//   Bat_al_time  = 560 : every 560 wakes (286.7 s)  -> BOTH LEDs together
//                                                       (low-battery / health)
//
// Most wakes do nothing visible - they exist purely to poll the button.
// system_count rolls over at Bat_al_time so the three modulo checks
// produce a clean repeating pattern. Ratios 1:7:35 are preserved.
// ORIG: uint16_t adc_time     = 8 * 5;          (0.192-s tick days)
// ORIG: uint16_t ck_led_time  = 56 * 5 + 12;
// ORIG: uint16_t Bat_al_time  = 280 * 5 + 58;
// ORIG: uint16_t adc_time     = 1;              (8-s tick intermediate revision)
// ORIG: uint16_t ck_led_time  = 7;
// ORIG: uint16_t Bat_al_time  = 35;
uint16_t system_count = 0;
uint16_t adc_time     = 16;
uint16_t ck_led_time  = 112;
uint16_t Bat_al_time  = 560;

// (NEW) Button-press latched flag - set by the main loop's wake-time button
// poll, cleared by whoever consumes the event. volatile because the value
// is shared between the main loop and (future) ISR / consumer paths.
volatile uint8_t button_pressed = 0;
//////////////// End:   System state and mode constants ////////////////

//////////////// Start: ADC globals ////////////////

#define VBGR_CV        92u      // VBGR voltage * 100  (chip-specific, calibrate!)
#define ADC_FS         1024u    // 10-bit

#define	System_Ck_Value	40

#define  ADC_BUFFER_SIZE 8
uint16_t ADC_temp_data[ADC_BUFFER_SIZE];

// (FIX C1) Vendor ADC_GetDataWithPolling() and Data_Avr/Sorting/TrimmedMean()
//          take ELEMENT COUNT, not byte count. Previously sizeof(ADC_temp_data)
//          was passed which equals 16 bytes (8 elements * 2 bytes/element),
//          causing the driver to write 16 uint16_t elements to an 8-element
//          array -> 16-byte stack/global buffer overflow (CRITICAL).
//          Use the element count macro below at all call sites instead.
#define ADC_BUFFER_COUNT  (sizeof(ADC_temp_data) / sizeof(ADC_temp_data[0]))


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
uint16_t ADC1_On_Dust_Val;
uint16_t ADC1_Off_Dust_Val;

//////////////// End:   ADC globals ////////////////


//////////////// Start: Debug-mode flags ////////////////

#define Debug_On	1
#define Debug_Off	0

// uart_debug_mode       : runtime flag - prints ADC values over UART each cycle
// start_uart_debug_mode : boot-time gate - if On, holding SW2 at power-up enters
//                         the UART debug while(1) loop. Set to Debug_Off in
//                         production builds (see firmware review issue H4).
uint8_t  uart_debug_mode = Debug_Off ;
uint8_t  start_uart_debug_mode = Debug_On ;
//uint8_t  start_uart_debug = 0 ;

//////////////// End:   Debug-mode flags ////////////////

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
	
	SystemStatus = SPI_Memory_Check();
	
	ADC_mode = Dust_mode;

	LED_G_ON;
	Delay_ms(5);		
	LED_G_OFF;
	
	LED_R_ON;
	Delay_ms(5);		
	LED_R_OFF;	
	
	//Set_Temp_Table();
	
	Port_SetInputpin(PORT1, PIN2, 1);  // key input & mode0 -> button input; held at power-up == enter UART mode
	Delay_ms(300);														// settle time before ADC mode sampling

	ADC_mode = Check_System();

		/* UART debug entry check at power-up */
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
						/*
						Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
						USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
						
							for( i=0; i< sizeof(ADC_temp_data); i++){
								Uart_Out_Int(ADC_temp_data[i]);
								USART_SendDataWithPolling(&Space, sizeof(Space));
							}
							
							USART_SendDataWithPolling(&End, 4);
							
						
							Port_SetAlterFunctionpin(PORT1, PIN2, 0);
	
						*/
						
						ADC_Temp_Val = TEMP_ADC();

						Uart_Out();

						Delay_ms(250);
						LED_R_OFF;
						Delay_ms(250);
						// (FIX C4) UART debug loop also needs watchdog refresh -
						// Delay_ms(250)*2 + work approaches 600ms; refresh keeps
						// the 2-second active-mode timeout margin.
						WD_Reset();
					}

		}
	}
	
	while(1){

				// system 초기화 
		hw_initial_Wait(10); 
		

		Port_SetOutputpin(PORT1, PIN0, 0);
		Delay_ms(100);
		P10 = 1;
		
		T2_init();
		LED_R_ON;
		LED_G_ON;
		Timer2_Start();
		
		// 시험 방송 1회 만 진행 
		
		//Fire_Alarm();
		
		Audio_Run(Audio3_Address , Audio3_length  , Audio3_runtime);
		
		GLOBAL_INTERRUPT_DIS();     
		hw_audio_initial(); 
		Delay_ms(10);
		GLOBAL_INTERRUPT_EN();  
		
		Timer2_Stop();
		LED_R_OFF;
		LED_G_OFF;
		
		Port_SetInputpin(PORT1, PIN0, 0);  // EM IO 
		P10 = 0;
		
		// system 초기화 
			hw_initial_Wait(10); 
			
			
					// 와치독 타이머 
	BeforeStop();
	GLOBAL_INTERRUPT_EN();  
	Stop();
	AfterStop();
	}
	
	while(1)
	{
		// =================================================================
		// 0.512-second wake cycle - WDT fires every 0.512 s (WDTDR=7).
		// On every wake we:
		//   1. kick the active-mode reset-watchdog,
		//   2. POLL the button (P12) - this is why we wake so often,
		//   3. if this tick is also an LED boundary, run the LED pattern,
		//   4. advance the tick counter, wrap at Bat_al_time,
		//   5. re-arm WDT in interrupt mode (BeforeStop) and Stop.
		//
		// LED pattern priority over the 560-wake (~ 286.7 s) cycle:
		//   tick (system_count)        | event                | LED action
		//   ---------------------------+----------------------+----------------
		//   0                          | Bat_al_time boundary | GREEN + RED on
		//   112, 224, 336, 448         | ck_led_time boundary | RED only
		//   16, 32, 48, ..., 544       | adc_time boundary    | GREEN only
		//   anything else              | (button-poll-only)   | (no LED)
		//
		// Most ticks (525 out of 560 in each cycle) just poll the button
		// and immediately go back to sleep.
		// =================================================================

		// (FIX C4) Kick the active-mode reset-watchdog at every wake.
		WD_Reset();

		// ---------------------------------------------------------------
		// (BUTTON) Poll the user button on P12 (SW2 -> tact -> GND).
		// Active-low - reads 0 while pressed. Latch into button_pressed
		// so application code (alarm-stop, test, etc.) can react on the
		// next iteration. P12 has no EINT-wake hardware, so polling
		// every wake is the cheapest mechanism available; the 0.512 s
		// wake interval keeps recognition latency within the ~0.5 s spec.
		// ---------------------------------------------------------------
		if (Port_GetInputpinValue(PORT1, PIN2) == 0) {
			button_pressed = 1;
		}

		// ORIG block (0.192-s ticks - only the green branch was ever taken):
		// ORIG: if(system_count%adc_time == 0){
		// ORIG:     hw_initial_Wait(1);
		// ORIG:     LED_G_ON;  Delay_ms(5);  LED_G_OFF;
		// ORIG:     hw_initial_Wait(1);
		// ORIG:     WD_Reset();
		// ORIG:     system_count = 0;
		// ORIG: }
		{
			// Pre-compute the three "this wake should blink X" predicates.
			uint8_t both_event  = (system_count % Bat_al_time == 0);  // every ~287 s
			uint8_t red_event   = (system_count % ck_led_time == 0);  // every  ~57 s
			uint8_t green_event = (system_count % adc_time   == 0);   // every  ~8.2 s

			// Only run the heavy peripheral re-init when there is actually
			// LED (or future sensor) work to do this tick - skips it on
			// the ~94% of wakes that are pure button polling.
			if (both_event || red_event || green_event) {
				// TODO (C6): replace this kitchen-sink hw_initial() with a
				// per-peripheral init - heavy reset on every LED tick is
				// the biggest remaining battery drain.
				hw_initial_Wait(1);

				// Highest priority wins: both > red > green.
				if (both_event) {
					// ~287-second dual-blink (low-battery / health indicator).
					LED_G_ON;
					LED_R_ON;
					Delay_ms(5);
					LED_G_OFF;
					LED_R_OFF;
				} else if (red_event) {
					// ~57-second red status blink.
					LED_R_ON;
					Delay_ms(5);
					LED_R_OFF;
				} else /* green_event */ {
					// ~8.2-second green heartbeat.
					LED_G_ON;
					Delay_ms(5);
					LED_G_OFF;
				}

				hw_initial_Wait(1);
			}
		}

		// (FIX C4) Refresh watchdog after LED + hw_initial so subsequent
		// sensor / audio code (when added later) gets a fresh 2-s WDT budget.
		WD_Reset();

		// Advance and wrap. Bat_al_time (35) is the LCM of the three periods,
		// so wrapping there keeps every modulo check synchronised.
		system_count++;
		if (system_count >= Bat_al_time) {
			system_count = 0;
		}

		// Re-arm WDT in INTERRUPT mode (8-s wake) and enter Stop.
		BeforeStop();
		GLOBAL_INTERRUPT_EN();
		Stop();
		AfterStop();

	}

}

//////////////// Start: Delay helpers ////////////////

/**********************************************************************
 * @brief       Delay_ms - busy-wait for `msec` milliseconds using the
 *              calibrated 20-us assembly delay (50 * 20 us = 1 ms).
 * @param       msec - delay duration in milliseconds
 **********************************************************************/
void Delay_ms(uint16_t msec)
{

	int i;
	
	for(i=0;i<msec;i++){
		NOP_20us_Delay(50);
	}

}

/**********************************************************************
 * @brief       Delay_s - busy-wait for `sec` seconds (50000 * 20 us = 1 s).
 *              Each iteration kicks the watchdog (FIX C4).
 * @param       sec - delay duration in seconds
 **********************************************************************/
void Delay_s(uint16_t sec)
{

	int i;

	for(i=0;i<sec;i++){
		NOP_20us_Delay(50000);
		// (FIX C4) Each iteration is 1 second - exceeds the 2-second active WDT
		//          margin for sec >= 2. Refresh the watchdog every iteration so
		//          long delays do not trigger a spurious reset.
		WDTCR |= 0x20;   // WDTCL bit: clear WDT counter
	}

}

//////////////// End:   Delay helpers ////////////////


//////////////// Start: System initialization helpers ////////////////

/**********************************************************************
 * @brief       hw_initial_Wait - run hw_initial() with interrupts off,
 *              then wait `msec` ms before re-enabling interrupts. Used to
 *              re-initialise peripherals on every wake-from-sleep cycle.
 *              (Frequent calls cause Audio/Timer disruption - see C6.)
 * @param       msec - settling delay in milliseconds
 **********************************************************************/
void hw_initial_Wait(int msec){

	GLOBAL_INTERRUPT_DIS();
	hw_initial();

	CVDD_OFF;
	Port_SetOutputpin(PORT1, PIN3, 0); // De-assert SPI flash CS

	Delay_ms(msec);

		/* Enable INT */
	GLOBAL_INTERRUPT_EN();

}


/**********************************************************************
 * @brief       hw_initial - full peripheral reset / re-init.
 *              Disables any in-flight audio playback first to avoid
 *              corrupting an active SPI/Timer stream, then reprograms
 *              every register (clock, GPIO, timers, WDT, LVR, OPAMP,
 *              ADC, USART, ICS, EINT, IE).
 **********************************************************************/
void hw_initial(void) {

	// If audio is currently playing, tear it down cleanly before reinitialising.
	if(Audio_start == 1) {
			SLAVEDESELECT;      // De-assert SPI flash CS
			AUDIO_OFF;          // Disable audio power
			Timer0_Stop();      // Stop PWM
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
	// (SLEEP-CURRENT FIX) P05/P04/P03/P02/P01 carry analog sensor signals
	// (smoke chamber photodiode +/-, opamp output, NTC divider in/out).
	// Leaving them as GPIO inputs forced the CMOS digital input buffer
	// to sit at an intermediate Vdd/2 level, causing 10-100 uA per pin
	// of shoot-through current and pushing sleep current above 20 uA.
	// The reference firmware (Stand_along_240628) sets these to their
	// OPAMP analog function which disconnects the digital input buffer
	// entirely - sleep current returns to ~1-2 uA.
	//   P05 -> OP0P    (smoke photodiode +)
	//   P04 -> OP0N    (smoke photodiode -)
	//   P03 -> OP0OUT  (opamp0 output node)
	//   P02 -> OP1P    (NTC divider +)
	//   P01 -> OP1N    (NTC divider -)
	//   P00 -> GPIO    (kept as I/O - matches reference)
	// AMPCR1 stays at 0 (opamps disabled) - pin function alone is
	// enough to disable the leakage; we save the opamp Iq on top.
	// Each ADC/sensor read still reprograms P0FSRL temporarily to AN
	// channels (see Check_System / TEMP_ADC / Dust_ADC_1AMP); they
	// will reset to this baseline on the next hw_initial() call.
	//
	// ORIG (high sleep current):
	// ORIG: P0FSRH = 0 | (0<<6) | (0<<4) | (0<<2) | (0<<0);
	// ORIG: P0FSRL = 0 | (0<<6) | (0<<4) | (0<<2) | (0<<0);
	P0FSRH  = 0
		| ( 0 << 6)       //P07     // 0 : I/O (EINT3), 1 : ICS1									// LED_R
		| ( 0 << 4)       //P06     // 0 : I/O (EINT2), 1 : ICS0									// LED_G
		| ( 1 << 2)       //P05     // 0 : I/O, 1 : OP0P,    										// SENSING OP0P (analog)
		| ( 1 << 0);      //P04     // 0 : I/O, 1 : OP0N													// SENSING OP0N (analog)

	P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC (analog)
		| ( 1 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// Temperature OP1P (analog)
		| ( 1 << 2)       //P01     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// Temperature OP1N (analog)
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD (GPIO)
		
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
	P1PU = 0x04;               		// 0 : Disable,	1 : Enable (Pull-up) //    
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


//////////////// End:   System initialization helpers ////////////////

//////////////// Start: Watchdog / sleep helpers ////////////////

/**********************************************************************
 * @brief       BeforeStop - prepare to enter Stop low-power mode. Slows
 *              the main oscillator and reconfigures WDT as a wake timer
 *              (interrupt mode, 0.192 s period using LFIRC).
 *              NOTE: this overrides the active-mode WDT_Reset() config;
 *              that is intentional - WDT must be in interrupt mode to
 *              wake from Stop, RESET mode would reset the chip instead.
 **********************************************************************/
void BeforeStop(void)
{

	OSCCR = 0x80;
	WDTCR = 0xA2;                       // WDT enable, INTERRUPT mode, clear counter, LFIRC clock

	// Wake-period selection.
	//   WDT timeout formula: T ~= (WDTDR + 1) * 0.064 s
	//     WDTDR =   2 -> 0.192 s
	//     WDTDR =   7 -> 0.512 s   <-- chosen (button polling deadline)
	//     WDTDR = 124 -> 8.0   s   (would be ideal for 10-yr battery but
	//                                button latency would also be 8 s)
	//
	// SW2 (button) is on P12 which does NOT have EINT wake-from-Stop
	// capability on A96L416, so we cannot use a true zero-power EINT
	// wake; the button has to be polled. The product spec requires
	// the button to be recognised within ~0.5 s, so we wake every
	// 0.512 s and poll P12 at the top of the main loop.
	//
	// Sensor / LED work is throttled by counter so it still happens at
	// the original 8 / 56 / 280 s cadence (see adc_time / ck_led_time /
	// Bat_al_time = 16 / 112 / 560 ticks).
	// ORIG: WDTDR = 2;	// entrance to every 0.192sec
	// ORIG: WDTDR = 124;	// (intermediate revision - 8 sec wake, too slow for button)
	WDTDR = 7;							// entrance to every 0.512sec
	//WDTDR = 124;							// entrance to every 8sec
	//WDTDR = 5;							// entrance to every 0.384sec
	//WDTDR = 2;							// entrance to every 0.192sec  (legacy)

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

// (FIX C4) Active-mode watchdog setup.
//   Purpose: When the MCU is awake and executing normal code, the WDT must be
//   in RESET mode (WDTCR bit 6 = 1) so that a hang/runaway recovers via
//   automatic reset. Sleep-mode wakeup keeps using interrupt-mode (see BeforeStop).
//
//   Behavior:
//     - Clock source : WDTRC (~3.072 MHz internal, independent of MCLK)
//     - Timeout      : WDTDR = 30 -> approximately 2 seconds (long enough for
//                      the longest active code paths such as Delay_ms(300) and
//                      ADC polling, but short enough to detect real hangs).
//     - Mode         : RESET (bit 6 set) -> MCU reset on overflow.
//   Must be called immediately after AfterStop() at the top of each main loop
//   iteration, and re-called from any long blocking section to refresh the
//   counter (effectively the "kick the dog" operation).
void WD_Reset(void)
{
	// ORIG: //WDTDR = 30;	// entrance to every 2sec
	// ORIG: WDTDR = 2;		// entrance to every 0.192sec
	WDTDR = 30;												// (FIX C4) ~2sec active-mode timeout
	//WDTCR = 0xA3;											// set WDT enable , clock source LFIRC, WDT interrupt
	WDTCR = 0xE5;											// set WDT enable , clock source WDTRC, RESET mode (bit6=1)
	IE3 |= 0x08;											// WDT interrupt enable (no effect in reset mode but harmless)
}

//////////////// End:   Watchdog / sleep helpers ////////////////

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

    if (adc_avg == 0) return 0;   // error / divide-by-zero guard

    vbat_cv = ((uint32_t)VBGR_CV * ADC_FS) / adc_avg;

    // sanity clamp (optional)
    if (vbat_cv > 400) vbat_cv = 400;   // ignore impossible >4.00 V readings

    return (uint16_t)vbat_cv;
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
		| ( 2<< 2)        //P01     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// BAT_ADC
		| ( 1 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
	
	OPAMP_Initial(GAIN1_DIS, GAIN0_DIS, chp_125KHz);
	OP0_Enable(TRUE);
	OP1_Enable(FALSE);

	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	
	ADC_SelectChannel(ADC_VBGR);
	
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes
	
	op=92;
	op*=1024;
	op/=Data_Avr(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;
  ADC1_Bat_Val=op;

	LDO_ON;
	ADC_SelectChannel(ADC_OP0OUT);
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes
	LDO_OFF;
	
	op=ADC1_Bat_Val;
	op *=Data_Avr(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;
	op/=1024;
  ADC1_On_Dust_Val=op;
	
	// LED-off (chamber IR off) reference read - subtracted from LED-on to remove ambient drift
	ADC_Initial(ADC_CLK_1M, ADC_SW_TRIG, ADC_INTERNAL_REF, ADC_LSB);	
	ADC_SelectChannel(ADC_OP0OUT);
	Delay_ms(1);
	
	ADC_GetDataWithPolling(ADC_temp_data, ADC_BUFFER_COUNT);  // (FIX C1) was sizeof(...) = bytes
	
	op=ADC1_Bat_Val;
	op *=Data_Avr(ADC_temp_data, ADC_BUFFER_COUNT) /* (FIX C1) was sizeof */;
	op/=1024;
  ADC1_Off_Dust_Val=op;
	
	Dust_adc_data = ADC1_On_Dust_Val - ADC1_Off_Dust_Val;
	
	return Dust_adc_data;
	
}
/*
void Set_Temp_Table(void){
	
Temp_Table[0]=86;
Temp_Table[1]=90;
Temp_Table[2]=95;
Temp_Table[3]=99;
Temp_Table[4]=104;
Temp_Table[5]=109;
Temp_Table[6]=114;
Temp_Table[7]=119;
Temp_Table[8]=124;
Temp_Table[9]=130;
Temp_Table[10]=135;
Temp_Table[11]=141;
Temp_Table[12]=147;
Temp_Table[13]=153;
Temp_Table[14]=159;
Temp_Table[15]=166;
Temp_Table[16]=172;
Temp_Table[17]=179;
Temp_Table[18]=186;
Temp_Table[19]=192;
Temp_Table[20]=199;
Temp_Table[21]=207;
Temp_Table[22]=214;
Temp_Table[23]=221;
Temp_Table[24]=229;
Temp_Table[25]=236;
Temp_Table[26]=244;
Temp_Table[27]=252;
Temp_Table[28]=261;
Temp_Table[29]=269;
Temp_Table[30]=278;
Temp_Table[31]=286;
Temp_Table[32]=295;
Temp_Table[33]=304;
Temp_Table[34]=313;
Temp_Table[35]=322;
Temp_Table[36]=331;
Temp_Table[37]=341;
Temp_Table[38]=350;
Temp_Table[39]=359;
Temp_Table[40]=369;
Temp_Table[41]=379;
Temp_Table[42]=388;
Temp_Table[43]=398;
Temp_Table[44]=407;
Temp_Table[45]=417;
Temp_Table[46]=427;
Temp_Table[47]=437;
Temp_Table[48]=446;
Temp_Table[49]=456;
Temp_Table[50]=466;
Temp_Table[51]=476;
Temp_Table[52]=485;
Temp_Table[53]=495;
Temp_Table[54]=504;
Temp_Table[55]=514;
Temp_Table[56]=524;
Temp_Table[57]=533;
Temp_Table[58]=542;
Temp_Table[59]=552;
Temp_Table[60]=561;
Temp_Table[61]=570;
Temp_Table[62]=579;
Temp_Table[63]=588;
Temp_Table[64]=597;
Temp_Table[65]=606;
Temp_Table[66]=615;
Temp_Table[67]=623;
Temp_Table[68]=632;
Temp_Table[69]=640;
Temp_Table[70]=648;
Temp_Table[71]=656;
Temp_Table[72]=664;
Temp_Table[73]=672;
Temp_Table[74]=680;
Temp_Table[75]=688;
Temp_Table[76]=695;
Temp_Table[77]=703;
Temp_Table[78]=710;
Temp_Table[79]=717;
Temp_Table[80]=724;
Temp_Table[81]=731;
Temp_Table[82]=738;
Temp_Table[83]=744;
Temp_Table[84]=751;
Temp_Table[85]=757;
Temp_Table[86]=763;
Temp_Table[87]=769;
Temp_Table[88]=775;
Temp_Table[89]=781;
Temp_Table[90]=787;
Temp_Table[91]=793;
Temp_Table[92]=798;
Temp_Table[93]=803;
Temp_Table[94]=809;
Temp_Table[95]=814;
Temp_Table[96]=819;
Temp_Table[97]=824;
Temp_Table[98]=829;
Temp_Table[99]=833;
}
*/
//////////////// End:   ADC helpers ////////////////


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

uint16_t 	Fire_Alarm_LED = 0;
uint16_t 	Bat_Alarm_LED = 0;

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
	
	//Timer2_cnt = 0;
}

//////////////// End:   Audio helpers ////////////////

//////////////// Start: UART helpers ////////////////


void Uart_Out(void){

	Port_SetAlterFunctionpin(PORT1, PIN2, 0x1);
						
	USART_Initial(9600, USART_DATA_8BIT, USART_STOP_1BIT, USART_PARITY_NO, USART_TX_RX_MODE);
	
	if(ADC_mode == Dust_mode){
		
		if(visual_type == 1){
			USART_SendDataWithPolling(&Visu_MODE, sizeof(Visu_MODE));
			USART_SendDataWithPolling(&Space, sizeof(Space));
			USART_SendDataWithPolling(&Bat, sizeof(Bat));
			Uart_Out_Int(ADC_Bat_Val);
		}
		else{
			USART_SendDataWithPolling(&Dust_MODE, sizeof(Dust_MODE));
			USART_SendDataWithPolling(&Space, sizeof(Space));
			USART_SendDataWithPolling(&Bat, sizeof(Bat));
			Uart_Out_Int(ADC_Bat_Val);
		}
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

/* --------------------------------- End Of File ------------------------------ */ 