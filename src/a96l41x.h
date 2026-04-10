/**
* @file     A96L41x.H
* @brief    A96L41x HW registers definitions and mem
* @author   Application Team, ABOV Semiconductor Co., Ltd.
* @version  V1.00
* @date     29. MAY. 2020
*
* Copyright(C) 2019, ABOV Semiconductor
* All rights reserved.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __A96L41x_H_
#define __A96L41x_H_
//-----------------------------------------------------
//  A96L416 Special Function Register Definitions
//----------------------------------------------------- 0x80
sfr     P0		   	= 0x80;
sbit    P07      	= 0x87;
sbit    P06	     	= 0x86;
sbit    P05      	= 0x85;
sbit    P04      	= 0x84;
sbit    P03      	= 0x83;
sbit    P02	     	= 0x82;
sbit    P01	     	= 0x81;
sbit    P00	     	= 0x80;

sfr	P1		   	= 0x88;
sbit    P15		   	= 0x8D;
sbit    P14		   	= 0x8C;
sbit    P13		   	= 0x8B;
sbit    P12		   	= 0x8A;
sbit    P11 		= 0x89;
sbit    P10		   	= 0x88;

sfr     P2		   	= 0x90;
sbit    P23			= 0x93;
sbit    P22			= 0x92;
sbit    P21			= 0x91;
sbit    P20			= 0x90;

sfr     EIFLAG	 	= 0xA0;
sbit    IICIFR	 	= 0xA7;
sbit    FLAG12	 	= 0xA6;
sbit    FLAG11	 	= 0xA5;
sbit    FLAG10	 	= 0xA4;
sbit    FLAG3		= 0xA3;
sbit    FLAG2		= 0xA2;
sbit    FLAG1		= 0xA1;
sbit    FLAG0 	 	= 0xA0;

sfr     IE	     	= 0xA8;
sbit    EA		  	= 0xAF;
sbit    INT5E		= 0xAD;
sbit    INT4E		= 0xAC;
sbit    INT3E		= 0xAB;
sbit    INT2E		= 0xAA;
sbit    INT1E		= 0xA9;
sbit    INT0E		= 0xA8;

sfr     IP	     	= 0xB8;
sbit    IP05 		= 0xBD;
sbit    IP04 		= 0xBC;
sbit    IP03 		= 0xBB;
sbit    IP02 		= 0xBA;
sbit    IP01 		= 0xB9;
sbit    IP00 		= 0xB8;

sfr     OSCCR	   	= 0xC8;
sbit    LFIRCE		= 0xCF;
sbit    HFIRCS1		= 0xCC;
sbit    HFIRCS0		= 0xCB;
sbit    HFIRCE		= 0xCA;

sfr     PSW	     	= 0xD0;
sbit    CY		   	= 0xD7;
sbit    AC		  	= 0xD6;
sbit    F0		  	= 0xD5;
sbit    RS1		   	= 0xD4;
sbit    RS0		   	= 0xD3;
sbit    OV		   	= 0xD2;
sbit    F1		   	= 0xD1;
sbit    P		    = 0xD0;

sfr     LVRCR	   	= 0xD8;
sbit    LVRST		= 0xDF;
sbit    LVRVS1	 	= 0xDA;
sbit    LVRVS0	 	= 0xD9;
sbit    LVREN		= 0xD8;

sfr     RSTFR	   	= 0xE8;
sbit    PORF		= 0xEF;
sbit    EXTRF		= 0xEE;
sbit    WDTRF		= 0xED;
sbit    OCDRF		= 0xEC;
sbit    LVRF		= 0xEB;

sfr     IP1      	= 0xF8;
sbit    IP15 		= 0xFD;
sbit    IP14 		= 0xFC;
sbit    IP13 		= 0xFB;
sbit    IP12 		= 0xFA;
sbit    IP11 		= 0xF9;
sbit    IP10 		= 0xF8;

sfr     SP		   	= 0x81;
sfr     DPL		   	= 0x82;
sfr     DPH		   	= 0x83;
sfr16	  DPTR	   	= 0x82;

sfr     DPL1		= 0x84;
sfr     DPH1		= 0x85;
sfr16	DPTR1	   	= 0x84;
sfr     PCON		= 0x87;

sfr     SCCR		= 0x8A;
sfr     BITCR		= 0x8B;
sfr     BITCNT	 	= 0x8C;
sfr     WDTCR		= 0x8D;
sfr     WDTDR		= 0x8E;
sfr     WDTCNT	 	= 0x8E;
sfr     IRCTCR	 	= 0x8F;

sfr     P0IO		= 0x91;
sfr     P0OD		= 0x92;
sfr     P0PU		= 0x93;
sfr     P0FSRL	 	= 0x94;
sfr     P0FSRH	 	= 0x95;
sfr     P0DB		= 0x96;
sfr     IRCTRM	 	= 0x97;

sfr     P1IO		= 0x99;
sfr     P1OD		= 0x9A;
sfr     P1PU		= 0x9B;
sfr     P1FSRL	 	= 0x9C;
sfr     P1FSRH   	= 0x9D;
sfr     P12DB   	= 0x9E;
sfr     IRCIDR	 	= 0x9F;

sfr     P2IO	   	= 0xA1;
sfr     EO		   	= 0xA2;
sfr     EIPOL0	 	= 0xA4;
sfr     EIPOL1	 	= 0xA5;

sfr     IE1		   	= 0xA9;
sfr     IE2		   	= 0xAA;
sfr     IE3		   	= 0xAB;
sfr     CHPCR    	= 0xAD;
sfr     AMPCR0	 	= 0xAE;
sfr     AMPCR1	 	= 0xAF;

sfr     T0CRL		= 0xB2;
sfr     T0CRH		= 0xB3;
sfr     T0ADRL	 	= 0xB4;
sfr     T0ADRH	 	= 0xB5;
sfr16   T0ADR    	= 0xB4;
sfr     T0BDRL	 	= 0xB6;
sfr     T0BDRH	 	= 0xB7;
sfr16   T0BDR    	= 0xB6;

//----------------------------------------------------- 0xB8                                                     
sfr     T1CRL		= 0xBA;
sfr     T1CRH		= 0xBB;
sfr     T1ADRL	 	= 0xBC;
sfr     T1ADRH	 	= 0xBD;
sfr16   T1ADR    	= 0xBC;
sfr     T1BDRL	 	= 0xBE;
sfr     T1BDRH	 	= 0xBF;
sfr16   T1BDR    	= 0xBE;

//----------------------------------------------------- 0xC0
sfr     T2CRL		= 0xC2;
sfr     T2CRH		= 0xC3;
sfr     T2ADRL	 	= 0xC4;
sfr     T2ADRH	 	= 0xC5;
sfr16   T2ADR    	= 0xC4;
sfr     T2BDRL	 	= 0xC6;
sfr     T2BDRH	 	= 0xC7;
sfr16   T2BDR    	= 0xC6;

//----------------------------------------------------- 0xC8
sfr     ADCCRL	 	= 0xCA;
sfr     ADCCRH	 	= 0xCB;
sfr     ADCDRL	 	= 0xCC;
sfr     ADCDRH	 	= 0xCD;
sfr16   ADCDR    	= 0xCC;
sfr     LDOCR	 	= 0xCE;

//----------------------------------------------------- 0xD0
sfr     P2OD	 	= 0xD2;
sfr     P2PU	 	= 0xD3;
sfr     P2FSR	 	= 0xD4;
sfr     FCDIN		= 0xD7;

//----------------------------------------------------- 0xD8
sfr     USTCR1	 	= 0xDA;
sfr     USTCR2	 	= 0xDB;
sfr     USTCR3	 	= 0xDC;
sfr     USTST	 	= 0xDD;
sfr     USTBD	 	= 0xDE;
sfr     USTDR	 	= 0xDF;

//----------------------------------------------------- 0xE0
sfr     ACC		   	= 0xE0;
sfr     ICSCR	 	= 0xE2;
sfr     ICSDR0	 	= 0xE3;
sfr     ICSDR1	 	= 0xE4;

//----------------------------------------------------- 0xE8
sfr     I2CCR	 	= 0xE9;
sfr     I2CSR	 	= 0xEA;
sfr     I2CSAR0	 	= 0xEB;
sfr     I2CDR	 	= 0xEC;
sfr     I2CSDHR	 	= 0xED;
sfr     I2CSCLR	 	= 0xEE;
sfr     I2CSCHR	 	= 0xEF;

//----------------------------------------------------- 0xF0
sfr     B		    = 0xF0;
sfr     I2CSAR1	 	= 0xF1;
sfr     DFSADRL	 	= 0xF2;
sfr     DFSADRH	 	= 0xF3;
sfr     DFIDR		= 0xF4;
sfr     DFMCR		= 0xF5;

//----------------------------------------------------- 0xF8
sfr     FSADRH	 	= 0xFA;
sfr     FSADRM	 	= 0xFB;
sfr     FSADRL	 	= 0xFC;
sfr     FIDR		= 0xFD;
sfr     FMCR		= 0xFE;

//==================================================================== //
// XSFR                                                                //
//==================================================================== //

#define     FCSARH      *(volatile unsigned char xdata*) 0x5050
#define     FCEARH      *(volatile unsigned char xdata*) 0x5051
#define     FCSARM       *(volatile unsigned char xdata*) 0x5052
#define     FCEARM      *(volatile unsigned char xdata*) 0x5053
#define     FCSARL      *(volatile unsigned char xdata*) 0x5054
#define     FCEARL      *(volatile unsigned char xdata*) 0x5055
#define     FCCR      *(volatile unsigned char xdata*) 0x5056
#define     FCDRH      *(volatile unsigned char xdata*) 0x5057
#define     FCDRL      *(volatile unsigned char xdata*) 0x5058
#define     LVRIDR      *(volatile unsigned char xdata*) 0x505F

//------------------------------------------------------------------------------
// Interrupt vectors address of A96L41x SFR.
#define HW_RESET_VECTOR             0x0000
#define EXTERNAL_INTERRUPT_0   	    0x0003
#define EXTERNAL_INTERRUPT_1   	    0x000B
#define EXTERNAL_INTERRUPT_2        0x0013
#define EXTERNAL_INTERRUPT_3        0x001B
#define EXTERNAL_INTERRUPT_10       0x0023
#define EXTERNAL_INTERRUPT_11    	 	0x002B
#define ADC_INTERRUPT        				0x0033
#define I2C_INTERRUPT         			0x003B
//#define           	0x0043
//#define            0x004B
//#define            0x0053
#define EXTERNAL_INTERRUPT_12       0x005B
#define T0_MATCH_INTERRUPT      	 	0x0063
#define T1_MATCH_INTERRUPT         	0x006B
#define T2_MATCH_INTERRUPT         	0x0073
//#define          	0x007B
#define USART_RX_INTERRUPT         	0x0083
#define USART_TX_INTERRUPT        	0x008B
//#define            0x0093
//#define          	0x009B
//#define            0x00A3
#define WDT_INTERRUPT               0x00AB
#define BIT_INTERRUPT               0x00B3
//#define            0x00BB

// Interrupt vectors Prioirty of A96L41x SFR. (For Keil C51)
#define EINT0_VECT         	0   // IE.0
#define EINT1_VECT         	1   // IE.1
#define EINT2_VECT       	 	2   // IE.2
#define EINT3_VECT         	3   // IE.3
#define EINT10_VECT        	4   // IE.4
#define EINT11_VECT       	5   // IE.5

#define ADC_VECT          	6   // IE1.0
#define I2C_VECT      			7   // IE1.1
//#define Reserved			      8   // IE1.2
//#define Reserved        		9   // IE1.3
//#define Reserved        		10  // IE1.4
#define EINT12_VECT         11  // IE1.5

#define T0_MATCH_VECT       12  // IE2.0
#define T1_MATCH_VECT       13  // IE2.1
#define T2_MATCH_VECT       14  // IE2.2
//#define Reserved       		15  // IE2.3
#define USART_RX_VECT		    16  // IE2.4
#define USART_TX_VECT      	17  // IE2.5

//#define Reserved           18  // IE3.0
//#define Reserved      			19  // IE3.1
//#define WT_VECT            20  // IE3.2
#define WDT_VECT            21  // IE3.3
#define BIT_VECT            22  // IE3.4

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Interrupt enable/disable control
// EA REG
#define EINT11_EN()         (IE |=0X20)
#define EINT11_DIS()        (IE &= ~(0X20))

#define EINT10_EN()         (IE |=0X10)
#define EINT10_DIS()        (IE &= ~(0X10))

#define EINT3_EN()         	(IE |=0X08)
#define EINT3_DIS()        	(IE &= ~(0X08))

#define EINT2_EN()         (IE |=0X04)
#define EINT2_DIS()        (IE &= ~(0X04))

#define EINT1_EN()         (IE |=0X02)
#define EINT1_DIS()        (IE &= ~(0X02))

#define EINT0_EN()         (IE |=0X01)
#define EINT0_DIS()        (IE &= ~(0X01))

#define GLOBAL_INTERRUPT_EN()       (EA = 1)
#define GLOBAL_INTERRUPT_DIS()      (EA = 0)

#define EI()                (EA = 1)
#define DI()                (EA = 0)

// IE1 REG
#define ADC_INT_EN()      	(IE1 |=0X01)
#define ADC_INT_DIS()     	(IE1 &= ~(0X01))

#define I2C_EN()       (IE1 |=0X02)
#define I2C_DIS()      (IE1 &= ~(0X02))

#define EINT12_INT_EN()     (IE1 |=0X20)
#define EINT12_INT_DIS()    (IE1 &= ~(0X20))

// IE2 REG
#define TIMER0_MATCH_EN()    (IE2 |=0X01)
#define TIMER0_MATCH_DIS()   (IE2 &= ~(0X01))

#define TIMER1_MATCH_EN()   (IE2 |=0X02)
#define TIMER1_MATCH_DIS()  (IE2 &= ~(0X02))

#define TIMER2_MATCH_EN()   (IE2 |=0X04)
#define TIMER2_MATCH_DIS()  (IE2 &= ~(0X04))

#define USART_RX_EN()   (IE2 |=0X10)
#define USART_RX_DIS()  (IE2 &= ~(0X10))

#define USART_TX_EN()   (IE2 |=0X20)
#define USART_TX_DIS()  (IE2 &= ~(0X20))

// IE3 REG
#define WDT_INT_EN()        (IE3 |=0X08)
#define WDT_INT_DIS()       (IE3 &= ~(0X08))

#define BIT_INT_EN()        (IE3 |=0X10)
#define BIT_INT_DIS()       (IE3 &= ~(0X10))

//------------------------------------------------------------------------------
/* CLOCK Control Register bits */
//------------------------------------------------------------------------------
//	SCCR (System and clock control Register : 0x8A, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define SCCR_IRC_4MHZ      ((INT8U)0x00)   /*!< Internal 4Mhz RC OSC for system clock */
#define SCCR_IRC_32KHZ     ((INT8U)0x03)   /*!< Internal 32Khz RC OSC for system clock  */
//------------------------------------------------------------------------------
//	OSCCR (Oscillator Control Register : 0xC8, Initial Value : 0x28 )
//------------------------------------------------------------------------------
#define OSCCR_LFIRCE_EN     ((INT8U)0x80)   /*!< OSCCR[7]
                                            Control the Operation the Low Frequency (32Khz)
                                            internal RC Oscillator at stop mode.
                                            1 : Enable
                                            */
#define OSCCR_HFIRCS_DIV8     ((INT8U)0x00)   /*!< OSCCR[4:3]
                                           High Frequency Internal RC Oscillator Post-devider Selection
                                            HFIRC/8 = 0.5Mhz
                                            */
#define OSCCR_HFIRCS_DIV4     ((INT8U)0x08)   /*!< OSCCR[4:3]
                                            High Frequency Internal RC Oscillator Post-devider Selection
                                            HFIRC/4 = 1Mhz
                                            */
#define OSCCR_HFIRCS_DIV2     ((INT8U)0x10)   /*!< OSCCR[4:3]
                                            High Frequency Internal RC Oscillator Post-devider Selection
                                            HFIRC/2 = 2Mhz
                                            */               
#define OSCCR_HFIRCS_DIV1    ((INT8U)0x18)   /*!< OSCCR[4:3]
											High Frequency Internal RC Oscillator Post-devider Selection
											HFIRC/1 = 4Mhz
												*/
#define OSCCR_HIRCE_EN      ((INT8U)0x00)   /*!< OSCCR[2]
                                            Control the Operation of the High Frequency (32Mhz)
                                            Internal RC oscillator
                                            !! Caution 1 : Disable
                                            */                                                           
#define OSCCR_HIRCE_DIS     ((INT8U)0x04)   /*!< OSCCR[2]
                                            Control the Operation of the High Frequency (32Mhz)
                                            Internal RC oscillator
                                            !! Caution 0 : Enable
                                            */                                                
//------------------------------------------------------------------------------
//	BITCR (Basic Interval Timer Control Register : 0x8B, Initial Value : 0x45 )
//------------------------------------------------------------------------------
#define BITIFR          ((INT8U)0x80) 	/*!< BITCR[7]
                                        When BIT Interrupt occurs, this bit becomes '1'.
                                        For clearing bit, write '0' to this bit or auto clear by INT_ACK signal.
                                        Writing '1' has no effect.
                                        0 : BIT interrupt no generation
                                        1 : BIT interrupt generation
                                        */
#define BITCK_DIV4096   ((INT8U)0x00) 	/*!< BITCR[6:5]
                                        Select BIT clock source
										000 : fx/4096
                                        */
#define BITCK_DIV1024   ((INT8U)0x20) 	/*!< BITCR[6:5]
                                        Select BIT clock source
										001 : fx/1024
                                         */
#define BITCK_DIV128    ((INT8U)0x40) 	/*!< BITCR[6:5]
                                        Select BIT clock source
                                        010 : fx/128
                                        */
#define BITCK_DIV16     ((INT8U)0x60) 	/*!< BITCR[6:5]
                                        Select BIT clock source
                                        011 : fx/16
                                        */
#define BCLR            ((INT8U)0x08) 	/*!< BITCR[3]
                                        If this bit is written to '1', BIT Counter is cleared to '0'
                                        0 : Free Running
                                        1 : Clear Counter
                                        */
#define BCK_2         ((INT8U)0x00) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        000 : 8ms (BIT Clock * 2)
                                        */
#define BCK_4           ((INT8U)0x01) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        001 : 16ms (BIT Clock * 4)
                                        */
#define BCK_8           ((INT8U)0x02) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        010 : 32ms (BIT Clock * 8)
                                        */
#define BCK_16           ((INT8U)0x03) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        011 : 64ms (BIT Clock * 16)
                                        */
#define BCK_32           ((INT8U)0x04) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        100 : 128ms (BIT Clock * 32)
                                        */
#define BCK_64          ((INT8U)0x05) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        101 : 256ms (BIT Clock * 64) (default)
                                        */	
#define BCK_128          ((INT8U)0x06) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        110 : 512ms (BIT Clock * 128)
                                        */	
#define BCK_256          ((INT8U)0x07) 	/*!< BITCR[2:0]
                                        Select BIT overflow period
                                        111 : 1024ms (BIT Clock * 256)
                                        */												
//------------------------------------------------------------------------------
//	WDTCR (Watch Dog Timer Control Register : 0x8D, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define WDTEN           ((INT8U)0x80) 	/*!< WDTCR[7]
                                        Control WDT Operation
                                        0 : Disable, 1 : Enable
                                        */
#define WDTRSON         ((INT8U)0x40) 	/*!< WDTCR[6]
                                        Control WDT RESET Operation
                                        0 : Free Running 8-bit timer
                                        1 : Watch Dog Timer RESET ON
                                        */
#define WDTCL           ((INT8U)0x20) 	/*!< WDTCR[5]
                                        Control WDT Operation
                                        0 : Free Run
                                        1 : Clear WDT Counter (auto clear after 1 Cycle)
                                        */
#define WDTCK_BIT_OVF   ((INT8U)0x00) 	/*!< WDTCR[2:1]
                                        Control WDT Operation
                                        00 : BIT overflow for WDT clock
                                        */
#define WDTCK_LFIRC     ((INT8U)0x02) 	/*!< WDTCR[2:1]
										Control WDT Operation
										01 : LFIRC for WDT clock
										*/
#define WDTCK_WDTRC     ((INT8U)0x04) 	/*!< WDTCR[2:1]
										Control WDT Operation
										10 : WDTRC for WDT clock
										*/
#define WDTIFR          ((INT8U)0x01) 	/*!< WDTCR[0]
                                        Control WDT Operation
                                        0 : WDT Interrupt no generation
                                        1 : WDT Interrupt generation
                                        */		
//------------------------------------------------------------------------------
//	P0OD (P0 Open-drain Selection Register : 0x92, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P07OD   ((INT8U)0x80)   /*!< P0OD[7]
                                Configure Open-drain of P07 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P06OD   ((INT8U)0x40)   /*!< P0OD[6]
                                Configure Open-drain of P06 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P05OD   ((INT8U)0x20)   /*!< P0OD[5]
                                Configure Open-drain of P05 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P04OD   ((INT8U)0x10)   /*!< P0OD[4]
                                Configure Open-drain of P04 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P03OD   ((INT8U)0x08)   /*!< P0OD[3]
                                Configure Open-drain of P03 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P02OD   ((INT8U)0x04)   /*!< P0OD[2]
                                Configure Open-drain of P02 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P01OD   ((INT8U)0x02)   /*!< P0OD[1]
                                Configure Open-drain of P01 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P00OD   ((INT8U)0x01)   /*!< P0OD[0]
                                Configure Open-drain of P00 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
//------------------------------------------------------------------------------
//	P1OD (P1 Open-drain Selection Register : 0x9A, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P15OD   ((INT8U)0x20)   /*!< P1OD[5]
                                Configure Open-drain of P15 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P14OD   ((INT8U)0x10)   /*!< P1OD[4]
                                Configure Open-drain of P14 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P13OD   ((INT8U)0x08)   /*!< P1OD[3]
                                Configure Open-drain of P13 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P12OD   ((INT8U)0x04)   /*!< P1OD[2]
                                Configure Open-drain of P12 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P11OD   ((INT8U)0x02)   /*!< P1OD[1]
                                Configure Open-drain of P11 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
#define P10OD   ((INT8U)0x01)   /*!< P1OD[0]
                                Configure Open-drain of P10 Port
                                0 : Push-pull output
                                1 : Open-drain output
                                */
//------------------------------------------------------------------------------
//	P1FSRH (Port 1 Function Selection High Register : 0x9D, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P1FSRH_P15_IO           ((INT8U)0x00) 	/*!< P1FSRH[3:2]
												P15 Function Select
												00 : I/O Port
												*/
#define P1FSRH_P15_LDO23         ((INT8U)0x04) 	/*!< P1FSRH[3:2]
												P15 Function Select
												01 : LDO23 Function
												*/
#define P1FSRH_P15_AN6   		((INT8U)0x08) 	/*!< P1FSRH[3:2]
												P15 Function Select
												10 : AN6
												*/
#define P1FSRH_P15_SDA	  		((INT8U)0x0C) 	/*!< P1FSRH[3:2]
												P15 Function Select
												11 : (SDA)
												*/
#define P1FSRH_P14_IO           ((INT8U)0x00) 	/*!< P1FSRH[1:0]
												P14 Function Select
												00 : I/O Port
												*/
#define P1FSRH_P14_AN9          ((INT8U)0x01) 	/*!< P1FSRH[1:0]
												P14 Function Select
												10 : AN9 Function
												*/
#define P1FSRH_P14_MOSI2_TXD2   ((INT8U)0x02) 	/*!< P1FSRH[1:0]
												P14 Function Select
												11 : MOSI2/TXD2
												*/
//------------------------------------------------------------------------------
//	P0IO (P0 Direction Register : 0x91, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P07IO   ((INT8U)0x80) 	/*!< P0IO[7]
                                P07 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P06IO   ((INT8U)0x40) 	/*!< P0IO[6]
                                P06 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P05IO   ((INT8U)0x20) 	/*!< P0IO[5]
                                P05 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P04IO   ((INT8U)0x10) 	/*!< P0IO[4]
                                P04 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P03IO   ((INT8U)0x08) 	/*!< P0IO[3]
                                P03 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P02IO   ((INT8U)0x04) 	/*!< P0IO[2]
                                P02 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P01IO   ((INT8U)0x02) 	/*!< P0IO[1]
                                P01 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P00IO   ((INT8U)0x01) 	/*!< P0IO[0]
                                P00 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
//------------------------------------------------------------------------------
//	EIPOL0 (External Interrupt Polarity 0 Register : 0xA4, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define POL3_NO_INT     ((INT8U)0x00) 	/*!< EIPOL0[7:6]
                                        EINT3 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL3_RIS_ADGE   ((INT8U)0x40) 	/*!< EIPOL0[7:6]
                                        EINT3 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL3_FAL_ADGE   ((INT8U)0x80) 	/*!< EIPOL0[7:6]
                                        EINT3 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL3_BOTH_ADGE  ((INT8U)0xC0) 	/*!< EIPOL0[7:6]
                                        EINT3 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */
#define POL2_NO_INT     ((INT8U)0x00) 	/*!< EIPOL0[5:4]
                                        EINT2 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL2_RIS_ADGE   ((INT8U)0x10) 	/*!< EIPOL0[5:4]
                                        EINT2 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL2_FAL_ADGE   ((INT8U)0x20) 	/*!< EIPOL0[5:4]
                                        EINT2 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL2_BOTH_ADGE  ((INT8U)0x30) 	/*!< EIPOL0[5:4]
                                        EINT2 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */
#define POL1_NO_INT     ((INT8U)0x00) 	/*!< EIPOL0[3:2]
                                        EINT1 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL1_RIS_ADGE   ((INT8U)0x04) 	/*!< EIPOL0[3:2]
                                        EINT1 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL1_FAL_ADGE   ((INT8U)0x08) 	/*!< EIPOL0[3:2]
                                        EINT1 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL1_BOTH_ADGE  ((INT8U)0x0C) 	/*!< EIPOL0[3:2]
                                        EINT1 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */
#define POL0_NO_INT     ((INT8U)0x00) 	/*!< EIPOL0[1:0]
                                        EINT0 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL0_RIS_ADGE   ((INT8U)0x40) 	/*!< EIPOL0[1:0]
                                        EINT0 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL0_FAL_ADGE   ((INT8U)0x80) 	/*!< EIPOL0[1:0]
                                        EINT0 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL0_BOTH_ADGE  ((INT8U)0xC0) 	/*!< EIPOL0[1:0]
                                        EINT0 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */                                
//------------------------------------------------------------------------------
//	EIPOL1 (External Interrupt Polarity 1 Register : 0xA5, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define POL12_NO_INT     ((INT8U)0x00) 	/*!< EIPOL1[5:4]
                                        EINT12 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL12_RIS_ADGE   ((INT8U)0x10) 	/*!< EIPOL1[5:4]
                                        EINT12 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL12_FAL_ADGE   ((INT8U)0x20) 	/*!< EIPOL1[5:4]
                                        EINT12 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL12_BOTH_ADGE  ((INT8U)0x30) 	/*!< EIPOL1[5:4]
                                        EINT12 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */
#define POL11_NO_INT     ((INT8U)0x00) 	/*!< EIPOL1[3:2]
                                        EINT11 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL11_RIS_ADGE   ((INT8U)0x04) 	/*!< EIPOL1[3:2]
                                        EINT11 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL11_FAL_ADGE   ((INT8U)0x08) 	/*!< EIPOL1[3:2]
                                        EINT11 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL11_BOTH_ADGE  ((INT8U)0x0C) 	/*!< EIPOL1[3:2]
                                        EINT11 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */
#define POL10_NO_INT     ((INT8U)0x00) 	/*!< EIPOL1[1:0]
                                        EINT10 polarity selection
                                        00 : No interrupt at any edge
                                        */
#define POL10_RIS_ADGE   ((INT8U)0x40) 	/*!< EIPOL1[1:0]
                                        EINT10 polarity selection
                                        01 : Interrupt on rising edge
                                        */    
#define POL10_FAL_ADGE   ((INT8U)0x80) 	/*!< EIPOL1[1:0]
                                        EINT10 polarity selection
                                        10 : Interrupt on falling edge
                                        */    
#define POL10_BOTH_ADGE  ((INT8U)0xC0) 	/*!< EIPOL1[1:0]
                                        EINT10 polarity selection
                                        11 : Interrupt on both of rising and falling edge
                                        */  
//------------------------------------------------------------------------------
//	EIFLAG (External Interrupt Flag Register : 0xA0, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define IICIFR         ((INT8U)0x80) 	/*!< EIFLAG1[7]
                                        When T0 overflow interrupt occurs, this bit becomes '1'.
                                        For clearing bit, write '0' to this bit or automatically clear by INT_ACK signal.
                                        Writing '1' has no effect.
                                        0 : I2C Interrupt no generation
                                        1 : I2C Interrupt generation
                                        */
#define EIFLAG_FLAG12   ((INT8U)0x40) 	/*!< EIFLAG1[6]
                                        When T0 interrupt occurs, this bit becomes '1'.
                                        For clearing bit, write '0' to this bit or automatically clear by INT_ACK signal.
                                        Writing '1' has no effect.
                                        0 : External Interrupt 12 not occurred
                                        1 : External Interrupt 12 occurred
                                        */
#define EIFLAG_FLAG11  ((INT8U)0x20) 	/*!< EIFLAG1[5]
										When an External Interrupt 12 is occurred, the flag becomes '1'.
										The flag is cleared by writing '0' to the bit or automatically cleared by INT_ACK signal.
										Writing '1' has no effect.
										0 : External Interrupt 11 not occurred
										1 : External Interrupt 11 occurred
										*/
#define EIFLAG_FLAG10  ((INT8U)0x10) 	/*!< EIFLAG1[4]
										When an External Interrupt 11 is occurred, the flag becomes '1'.
										The flag is cleared by writing '0' to the bit or automatically cleared by INT_ACK signal.
										Writing '1' has no effect.
										0 : External Interrupt 10 not occurred
										1 : External Interrupt 10 occurred
										*/
#define EIFLAG_FLAG3  ((INT8U)0x08) 	/*!< EIFLAG1[3]
										When an External Interrupt 10 is occurred, the flag becomes '1'.
										The flag is cleared by writing '0' to the bit or automatically cleared by INT_ACK signal.
										Writing '1' has no effect.
										0 : External Interrupt 3 not occurred
										1 : External Interrupt 3 occurred
										*/
#define EIFLAG_FLAG2  ((INT8U)0x04) 	/*!< EIFLAG1[2]
										When an External Interrupt 10 is occurred, the flag becomes '1'.
										The flag is cleared by writing '0' to the bit or automatically cleared by INT_ACK signal.
										Writing '1' has no effect.
										0 : External Interrupt 2 not occurred
										1 : External Interrupt 2 occurred
										*/
#define EIFLAG_FLAG1  ((INT8U)0x02) 	/*!< EIFLAG1[1]
										When an External Interrupt 10 is occurred, the flag becomes '1'.
										The flag is cleared by writing '0' to the bit or automatically cleared by INT_ACK signal.
										Writing '1' has no effect.
										0 : External Interrupt 1 not occurred
										1 : External Interrupt 1 occurred
										*/
#define EIFLAG1_FLAG0   ((INT8U)0x01) 	/*!< EIFLAG1[0]
										When an External Interrupt 8 is occurred, the flag becomes '1'.
										The flag is cleared by writing '0' to the bit or automatically cleared by INT_ACK signal.
										Writing '1' has no effect.
										0 : External Interrupt 0 not occurred
										1 : External Interrupt 0 occurred
										*/

//------------------------------------------------------------------------------
//	IE1 (Interrupt Enable Register 1 : 0xA9, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define INT11E  ((INT8U)0x20) 	/*!< IE1[5]
                                Enable or Disable External Interrupt 12 (EINT12)
								0 : Disable, 1 : Enable
                                */
#define INT7E   ((INT8U)0x02) 	/*!< IE1[1]
                                Enable or Disable USART2 TX Interrupt
								0 : Disable, 1 : Enable
                                */
#define INT6E   ((INT8U)0x01) 	/*!< IE1[0]
                                Enable or Disable External Interrupt 8 (EINT8)
								0 : Disable, 1 : Enable
                                */
//------------------------------------------------------------------------------
//	IE2 (Interrupt Enable Register 2 : 0xAA, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define INT17E ((INT8U)0x20) 	/*!< IE2[5]
                                Enable or Disable Timer 4/5 Match Interrupt
								0 : Disable, 1 : Enable
                                */
#define INT16E ((INT8U)0x10) 	/*!< IE2[4]
                                Enable or Disable Timer 3 Match Interrupt
								0 : Disable, 1 : Enable
                                */
#define INT14E ((INT8U)0x04) 	/*!< IE2[2]
                                Enable or Disable Timer 1 Match Interrupt
								0 : Disable, 1 : Enable
                                */
#define INT13E ((INT8U)0x02) 	/*!< IE2[1]
                                Enable or Disable Timer 0 I Match Interrupt
								0 : Disable, 1 : Enable
                                */
#define INT12E ((INT8U)0x01) 	/*!< IE2[0]
                                Enable or Disable Timer 0 Overflow Interrupt
								0 : Disable, 1 : Enable
                                */
//------------------------------------------------------------------------------
//	P0PU (P0 Pull-up Resistor Selection Register : 0x93, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P07PU   ((INT8U)0x80)   /*!< P0PU[7]
                                Configure Pull-up Resistor of P07 Port
                                0 : Disable, 1 : Enable
                                */
#define P06PU   ((INT8U)0x40)   /*!< P0PU[6]
                                Configure Pull-up Resistor of P06 Port
                                0 : Disable, 1 : Enable
                                */
#define P05PU   ((INT8U)0x20)   /*!< P0PU[5]
                                Configure Pull-up Resistor of P05 Port
                                0 : Disable, 1 : Enable
                                */
#define P04PU   ((INT8U)0x10)   /*!< P0PU[4]
                                Configure Pull-up Resistor of P04 Port
                                0 : Disable, 1 : Enable
                                */
#define P03PU   ((INT8U)0x08)   /*!< P0PU[3]
                                Configure Pull-up Resistor of P03 Port
                                0 : Disable, 1 : Enable
                                */
#define P02PU   ((INT8U)0x04)   /*!< P0PU[2]
                                Configure Pull-up Resistor of P02 Port
                                0 : Disable, 1 : Enable
                                */
#define P01PU   ((INT8U)0x02)   /*!< P0PU[1]
                                Configure Pull-up Resistor of P01 Port
                                0 : Disable, 1 : Enable
                                */
#define P00PU   ((INT8U)0x01)   /*!< P0PU[0]
                                Configure Pull-up Resistor of P00 Port
                                0 : Disable, 1 : Enable
                                */ 
//------------------------------------------------------------------------------
//	P0FSRL (Port 0 Function Selection Low Register : 0x94, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P0FSRL_P03_IO           ((INT8U)0x00) 	/*!< P0FSRL[7:6]
												P03 Function Select
												00 : I/O Port (EC1 function possible when input)
												*/	
#define P0FSRL_P03_OP0OUT        ((INT8U)0x40) 	/*!< P0FSRL[7:6]
												P03 Function Select
												01 : OP0OUT Function
												*/	
#define P0FSRL_P03_AN3         ((INT8U)0x80) 	/*!< P0FSRL[7:6]
												P03 Function Select
												10 : AN3 Function
												*/	
#define P0FSRL_P02_IO           ((INT8U)0x00) 	/*!< P0FSRL[5:4]
												P02 Function Select
												00 : I/O Port (EINT11 function possible when input)
												*/	
#define P0FSRL_P02_OP1P         ((INT8U)0x10) 	/*!< P0FSRL[5:4]
												P02 Function Select
												01 : OP1P Function
												*/	
#define P0FSRL_P02_AN2  		 ((INT8U)0x20) 	/*!< P0FSRL[5:4]
												P02 Function Select
												10 : AN2 Function
												*/	
#define P0FSRL_P01_IO           ((INT8U)0x00) 	/*!< P0FSRL[3:2]
												P01 Function Select
												00 : I/O Port (EINT12 function possible when input)
												*/	
#define P0FSRL_P01_OP1N         ((INT8U)0x04) 	/*!< P0FSRL[3:2]
												P01 Function Select
												01 : OP1N Function
												*/	
#define P0FSRL_P01_AN1    		((INT8U)0x08) 	/*!< P0FSRL[3:2]
												P01 Function Select
												10 : AN1 Function
												*/	
#define P0FSRL_P00_IO           ((INT8U)0x00) 	/*!< P0FSRL[1:0]
												P00 Function Select
												00 : I/O Port
												*/	
#define P0FSRL_P00_OP1OUT        ((INT8U)0x01) 	/*!< P0FSRL[1:0]
												P00 Function Select
												01 : OP1OUT Function
												*/	
#define P0SRL_P00_AN0 			((INT8U)0x02)	/*!< P0FSRL[1:0]
												P00 Function Select
												10 : AN0 Function
												*/	
//------------------------------------------------------------------------------
//	P0FSRH (Port 0 Function Selection High Register : 0x95, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P0FSRH_P07_IO           ((INT8U)0x00) 	/*!< P0FSRH[6]
												P07 Function Select
												0 : I/O Port (EINT3 function possible when input)
												*/	
#define P0FSRH_P07_ICS1          ((INT8U)0x80) 	/*!< P0FSRH[6]
												P07 Function Select
												1 : AN5 Function
												*/	
#define P0FSRH_P06_IO           ((INT8U)0x00) 	/*!< P0FSRH[4]
												P06 Function Select
												0 : I/O Port (EINT2 function possible when input)
												*/	
#define P0FSRH_P06_ICS0          ((INT8U)0x10) 	/*!< P0FSRH[4]
												P06 Function Select
												1 : ICS0 Function
												*/	
#define P0FSRH_P05_IO           ((INT8U)0x00) 	/*!< P0FSRH[2]
												P05 Function Select
												0 : I/O Port
												*/	
#define P0FSRH_P05_OP0P         ((INT8U)0x04) 	/*!< P0FSRH[2]
												P05 Function Select
												1 : OP0P Function
												*/	
#define P0FSRH_P04_IO           ((INT8U)0x00) 	/*!< P0FSRH[0]
												P04 Function Select
												0 : I/O Port
												*/	
#define P0FSRH_P04_OP0N          ((INT8U)0x01) 	/*!< P0FSRH[0]
												P04 Function Select
												1 : OP0N Function
												*/	
//------------------------------------------------------------------------------
//	P0DB (P0 De-bounce Enable Register : 0x96, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define DBCLK_DIV1      ((INT8U)0x00) 	/*!< P0DB[7:6]
										Configure De-bounce Clock of Port
										00 : fx/1
										*/
#define DBCLK_DIV4      ((INT8U)0x40) 	/*!< P0DB[7:6]
										Configure De-bounce Clock of Port
										01 : fx/4
										*/
#define DBCLK_DIV16   ((INT8U)0x80)   /*!< P0DB[7:6]
										Configure De-bounce Clock of Port
										10 : fx/16
										*/
#define DBCLK_DIV64     ((INT8U)0xC0) 	/*!< P0DB[7:6]
										Configure De-bounce Clock of Port
										11 : fx/64
										*/
#define P07DB           ((INT8U)0x08) 	/*!< P0DB[3]
										Configure De-bounce of P07 Port
										0 : Disable, 1 Enable
										*/
#define P06DB           ((INT8U)0x04) 	/*!< P0DB[2]
										Configure De-bounce of P06 Port
										0 : Disable, 1 Enable
										*/
#define P01DB           ((INT8U)0x02) 	/*!< P0DB[1]
										Configure De-bounce of P01 Port
										0 : Disable, 1 Enable
										*/
#define P00DB           ((INT8U)0x01) 	/*!< P0DB[0]
										Configure De-bounce of P00 Port
										0 : Disable, 1 Enable
										*/
//------------------------------------------------------------------------------
//	P1IO (P1 Direction Register : 0x99, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P15IO   ((INT8U)0x20) 	/*!< P1IO[5]
                                P15 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P14IO   ((INT8U)0x10) 	/*!< P1IO[4]
                                P14 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P13IO   ((INT8U)0x08) 	/*!< P1IO[3]
                                P13 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P12IO   ((INT8U)0x04) 	/*!< P1IO[2]
                                P12 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P11IO   ((INT8U)0x02) 	/*!< P1IO[1]
                                P11 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
#define P10IO   ((INT8U)0x01) 	/*!< P1IO[0]
                                P10 Data I/O Direction.
                                0 : Input, 1 : Output
                                */ 
//------------------------------------------------------------------------------
//	P1PU (P1 Pull-up Resistor Selection Register : 0x9B, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P17PU   ((INT8U)0x80)   /*!< P1PU[7]
								Configure Pull-up Resistor of P17 Port
								0 : Disable, 1 : Enable
								*/
#define P16PU   ((INT8U)0x40)   /*!< P1PU[6]
								Configure Pull-up Resistor of P16 Port
								0 : Disable, 1 : Enable
								*/
#define P15PU   ((INT8U)0x20)   /*!< P1PU[5]
								Configure Pull-up Resistor of P15 Port
								0 : Disable, 1 : Enable
								*/
#define P14PU   ((INT8U)0x10)   /*!< P1PU[4]
								Configure Pull-up Resistor of P14 Port
								0 : Disable, 1 : Enable
								*/
#define P13PU   ((INT8U)0x08)   /*!< P1PU[3]
								Configure Pull-up Resistor of P13 Port
								0 : Disable, 1 : Enable
								*/
#define P12PU   ((INT8U)0x04)   /*!< P1PU[2]
								Configure Pull-up Resistor of P12 Port
								0 : Disable, 1 : Enable
								*/
#define P11PU   ((INT8U)0x02)   /*!< P1PU[1]
								Configure Pull-up Resistor of P11 Port
								0 : Disable, 1 : Enable
								*/
#define P10PU   ((INT8U)0x01)   /*!< P1PU[0]
								Configure Pull-up Resistor of P10 Port
								0 : Disable, 1 : Enable
								*/ 
//------------------------------------------------------------------------------
//	P1FSRL (Port 1 Function Selection Low Register : 0x9C, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P1FSRL_P13_IO           ((INT8U)0x00) 	/*!< P1FSRL[6]
												P13 Function Select
												0 : I/O Port (EINT6 function possible when input)
												*/
#define P1FSRL_P13_RXD_MISO      ((INT8U)0x80) 	/*!< P1FSRL[6]
												P13 Function Select
												1 : (RXD/MISO) Function
												*/
#define P1FSRL_P12_IO           ((INT8U)0x00) 	/*!< P1FSRL[4]
												P12 Function Select
												0 : I/O Port (EINT7 function possible when input)
												*/
#define P1FSRL_P12_TXD_MOSI     ((INT8U)0x10) 	/*!< P1FSRL[4]
												P12 Function Select
												1 : (TXD/MOSI) Function
												*/
#define P1FSRL_P11_IO           ((INT8U)0x00) 	/*!< P1FSRL[2]
												P11 Function Select
												0 : I/O Port
												*/
#define P1FSRL_P11_T0O_PWM0O    ((INT8U)0x08) 	/*!< P1FSRL[2]
												P11 Function Select
												1 : T0O/PWM0O Function
												*/
#define P1FSRL_P10_IO           ((INT8U)0x00) 	/*!< P1FSRL[0]
												P10 Function Select
												0 : I/O Port
												*/
#define P1FSRL_P10_SCK          ((INT8U)0x01) 	/*!< P1FSRL[0]
												P10 Function Select
												1 : (SCK) Function
												*/
//------------------------------------------------------------------------------
//	P12DB (P1/P2 De-bounce Enable Register : 0x9E, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P21DB ((INT8U)0x20) 	/*!< P21DB[5]
								Configure De-bounce of P21 Port
								0 : Disable, 1 Enable
								*/
#define P20DB ((INT8U)0x10) 	/*!< P20DB[4]
								Configure De-bounce of P20 Port
								0 : Disable, 1 Enable
								*/
#define P14DB ((INT8U)0x04) 	/*!< P14DB[2]
								Configure De-bounce of P14 Port
								0 : Disable, 1 Enable
								*/
#define P11DB ((INT8U)0x02) 	/*!< P11DB[1]
								Configure De-bounce of P11 Port
								0 : Disable, 1 Enable
								*/
#define P10DB ((INT8U)0x01) 	/*!< P10DB[0]
								Configure De-bounce of P10 Port
								0 : Disable, 1 Enable
								*/
//------------------------------------------------------------------------------
//	P2IO (P2 Direction Register : 0xA1, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P23IO   ((INT8U)0x08) 	/*!< P2IO[3]
								P23 Data I/O Direction.
								0 : Input, 1 : Output
								*/ 
#define P22IO   ((INT8U)0x04) 	/*!< P2IO[2]
								P22 Data I/O Direction.
								0 : Input, 1 : Output
								*/ 
#define P21IO   ((INT8U)0x02) 	/*!< P2IO[1]
								P21 Data I/O Direction.
								0 : Input, 1 : Output
								*/ 
#define P20IO   ((INT8U)0x01) 	/*!< P2IO[0]
								P20 Data I/O Direction.
								0 : Input, 1 : Output
								*/ 
//------------------------------------------------------------------------------
//	IE3 (Interrupt Enable Register 3 : 0xAB, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define INT22E ((INT8U)0x10) 	/*!< IE3[4]
								Enable or Disable BIT Interrupt
								0 : Disable, 1 : Enable
								*/
#define INT21E ((INT8U)0x08) 	/*!< IE3[3]
								Enable or Disable WDT Interrupt
								0 : Disable, 1 : Enable
								*/
//------------------------------------------------------------------------------
//	T0CRL (Timer 0 Control Low Register : 0xB2, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define T0CK_DIV2048    ((INT8U)0x00) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        000 : fx/2048
                                        */
#define T0CK_DIV512     ((INT8U)0x20) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        001 : fx/512
                                        */
#define T0CK_DIV64      ((INT8U)0x40) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        010 : fx/64
                                        */
#define T0CK_DIV8       ((INT8U)0x60) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        011 : fx/8
                                        */
#define T0CK_DIV4       ((INT8U)0x80) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        100 : fx/4
                                        */
#define T0CK_DIV2       ((INT8U)0xA0) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        101 : fx/2
                                        */
#define T0CK_DIV1       ((INT8U)0xC0) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        110 : fx/1
                                        */
#define T0CK_EC         ((INT8U)0xE0) 	/*!< T0CRL[7:5]
                                        Select Timer 0 clock source.
                                        111 : External clock (EC0)
                                        */
#define T0IFR           ((INT8U)0x10) 	/*!< T0CRL[4]
                                        When T0 Interrupt occurs, this bit becomes '1'.
                                        For clearing bit, write '0' to this bit or auto clear by INT_ACK signal.
                                        Writing '1' has no effect.
                                        0 : T0 Interrupt no generation
                                        1 : T0 Interrupt generation
                                        */
#define T0POL           ((INT8U)0x04) 	/*!< T0CRL[2]
                                        T0O/PWM0O Polarity Selection
                                        0 : Start High (T0O/PWM0O is low level at disable)
                                        1 : Start Low (T0O/PWM0O is high level at disable)
                                        */											
#define T0ECE           ((INT8U)0x02) 	/*!< T0CRL[1]
                                        Timer 0 External Clock Edge Selection
                                        0 : External clock falling edge
                                        1 : External clock rising edge
                                        */
#define T0CNTR          ((INT8U)0x01) 	/*!< T0CRL[0]
                                        Timer 0 Counter Read Control
                                        0 : No effect
                                        1 : Load the counter value to the B data register
                                        */											
//------------------------------------------------------------------------------
//	T0CRH (Timer 1 Control High Register : 0xB3, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define T0EN             ((INT8U)0x80) 	/*!< T0CRH[7]
										Control Timer 0
										0 : Timer 0 disable
										1 : Timer 0 enable (Counter clear and start)
										*/
#define T0MS_TC          ((INT8U)0x00) 	/*!< T0CRH[5:4]
										Control Timer 0 Operation Mode
										00 : Timer/counter mode (T0O: toggle at A match)
										*/
#define T0MS_CAP         ((INT8U)0x10) 	/*!< T0CRH[5:4]
										Control Timer 0 Operation Mode
										01 : Capture mode (The A match interrupt can occur)
										*/
#define T0MS_PPG_OS      ((INT8U)0x20) 	/*!< T0CRH[5:4]
										Control Timer 0 Operation Mode
										10 : PPG one-shot mode (PWM0O)
										*/
#define T0MS_PPG_RP      ((INT8U)0x30) 	/*!< T0CRH[5:4]
										Control Timer 0 Operation Mode
										11 : PPG repeat mode (PWM0O)
										*/
#define T0CC             ((INT8U)0x01) 	/*!< T1CRH[0]
										Clear Timer 0 Counter
										0 : No effect
										1 : Clear the Timer 0 counter
										*/
//------------------------------------------------------------------------------
//	T1CRL (Timer 1 Control Low Register : 0xBA, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define T1CK_DIV2048    ((INT8U)0x00) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        000 : fx/2048
                                        */
#define T1CK_DIV512     ((INT8U)0x20) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        001 : fx/512
                                        */
#define T1CK_DIV64      ((INT8U)0x40) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        010 : fx/64
                                        */
#define T1CK_DIV8       ((INT8U)0x60) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        011 : fx/8
                                        */
#define T1CK_DIV4       ((INT8U)0x80) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        100 : fx/4
                                        */
#define T1CK_DIV2       ((INT8U)0xA0) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        101 : fx/2
                                        */
#define T1CK_DIV1       ((INT8U)0xC0) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        110 : fx/1
                                        */
#define T1CK_EC         ((INT8U)0xE0) 	/*!< T1CRL[7:5]
                                        Select Timer 1 clock source.
                                        111 : External clock (EC1)
                                        */
#define T1IFR           ((INT8U)0x10) 	/*!< T1CRL[4]
                                        When T1 Interrupt occurs, this bit becomes '1'.
                                        For clearing bit, write '0' to this bit or auto clear by INT_ACK signal.
                                        Writing '1' has no effect.
                                        0 : T1 Interrupt no generation
                                        1 : T1 Interrupt generation
                                        */
#define T1POL           ((INT8U)0x04) 	/*!< T1CRL[2]
                                        T1O/PWM1O Polarity Selection
                                        0 : Start High (T1O/PWM1O is low level at disable)
                                        1 : Start Low (T1O/PWM1O is high level at disable)
                                        */											
#define T1ECE           ((INT8U)0x02) 	/*!< T1CRL[1]
                                        Timer 1 External Clock Edge Selection
                                        0 : External clock falling edge
                                        1 : External clock rising edge
                                        */
#define T1CNTR          ((INT8U)0x01) 	/*!< T1CRL[0]
                                        Timer 1 Counter Read Control
                                        0 : No effect
                                        1 : Load the counter value to the B data register
                                        */											
//------------------------------------------------------------------------------
//	T1CRH (Timer 1 Control High Register : 0xBB, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define T1EN             ((INT8U)0x80) 	/*!< T1CRH[7]
                                        Control Timer 1
                                        0 : Timer 1 disable
                                        1 : Timer 1 enable (Counter clear and start)
                                        */
#define T1MS_TC          ((INT8U)0x00) 	/*!< T1CRH[5:4]
                                        Control Timer 1 Operation Mode
                                        00 : Timer/counter mode (T1O: toggle at A match)
                                        */
#define T1MS_CAP         ((INT8U)0x10) 	/*!< T1CRH[5:4]
                                        Control Timer 1 Operation Mode
										01 : Capture mode (The A match interrupt can occur)
                                        */
#define T1MS_PPG_OS      ((INT8U)0x20) 	/*!< T1CRH[5:4]
                                        Control Timer 1 Operation Mode
										10 : PPG one-shot mode (PWM1O)
                                        */
#define T1MS_PPG_RP      ((INT8U)0x30) 	/*!< T1CRH[5:4]
                                        Control Timer 1 Operation Mode
										11 : PPG repeat mode (PWM1O)
                                        */
#define T1CC             ((INT8U)0x01) 	/*!< T1CRH[0]
                                        Clear Timer 1 Counter
                                        0 : No effect
                                        1 : Clear the Timer 1 counter
                                        */
//------------------------------------------------------------------------------
//	T2CRL (Timer 2 Control Low Register : 0xC2, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define T2CK_DIV2048    ((INT8U)0x00) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        000 : fx/2048
                                        */
#define T2CK_DIV512     ((INT8U)0x20) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        001 : fx/512
                                        */
#define T2CK_DIV64      ((INT8U)0x40) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        010 : fx/64
                                        */
#define T2CK_DIV8       ((INT8U)0x60) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        011 : fx/8
                                        */
#define T2CK_DIV4       ((INT8U)0x80) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        100 : fx/4
                                        */
#define T2CK_DIV2       ((INT8U)0xA0) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        101 : fx/2
                                        */
#define T2CK_DIV1       ((INT8U)0xC0) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        110 : fx/1
                                        */
#define T2CK_EC         ((INT8U)0xE0) 	/*!< T2CRL[7:5]
                                        Select Timer 2 clock source.
                                        111 : External clock (EC2)
                                        */
#define T2IFR           ((INT8U)0x10) 	/*!< T2CRL[4]
                                        When T2 Interrupt occurs, this bit becomes '1'.
                                        For clearing bit, write '0' to this bit or auto clear by INT_ACK signal.
                                        Writing '1' has no effect.
                                        0 : T2 Interrupt no generation
                                        1 : T2 Interrupt generation
                                        */
#define T2POL           ((INT8U)0x04) 	/*!< T2CRL[2]
                                        T2O/PWM2O Polarity Selection
                                        0 : Start High (T2O/PWM2O is low level at disable)
                                        1 : Start Low (T2O/PWM2O is high level at disable)
                                        */											
#define T2ECE           ((INT8U)0x02) 	/*!< T2CRL[1]
                                        Timer 2 External Clock Edge Selection
                                        0 : External clock falling edge
                                        1 : External clock rising edge
                                        */
#define T2CNTR          ((INT8U)0x01) 	/*!< T2CRL[0]
                                        Timer 2 Counter Read Control
                                        0 : No effect
                                        1 : Load the counter value to the B data register
                                        */											
//------------------------------------------------------------------------------
//	T2CRH (Timer 2 Control High Register : 0xC3, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define T2EN             ((INT8U)0x80) 	/*!< T2CRH[7]
                                        Control Timer 2
                                        0 : Timer 2 disable
                                        1 : Timer 2 enable (Counter clear and start)
                                        */
#define T2MS_TC          ((INT8U)0x00) 	/*!< T2CRH[5:4]
                                        Control Timer 2 Operation Mode
                                        00 : Timer/counter mode (T2O: toggle at A match)
                                        */
#define T2MS_CAP         ((INT8U)0x10) 	/*!< T2CRH[5:4]
                                        Control Timer 2 Operation Mode
										01 : Capture mode (The A match interrupt can occur)
                                        */
#define T2MS_PPG_OS      ((INT8U)0x20) 	/*!< T2CRH[5:4]
                                        Control Timer 2 Operation Mode
										10 : PPG one-shot mode (PWM2O)
                                        */
#define T2MS_PPG_RP      ((INT8U)0x30) 	/*!< T2CRH[5:4]
                                        Control Timer 2 Operation Mode
										11 : PPG repeat mode (PWM2O)
                                        */
#define T2CC             ((INT8U)0x01) 	/*!< T2CRH[0]
                                        Clear Timer 2 Counter
                                        0 : No effect
                                        1 : Clear the Timer 2 counter
                                        */
//------------------------------------------------------------------------------
//	ADCCRL (A/D Converter Counter Low Register : 0xCA, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define STBY      ((INT8U)0x80) 	/*!< ADCCRL[7]
									Control Operation of A/D
									(The ADC module is automatically disabled at stop mode)
									0 : ADC module disable
									1 : ADC module enable
									*/
#define ADST      ((INT8U)0x40) 	/*!< ADCCRL[6]
									Control A/D Conversion stop/start.
									0 : No effect
									1 : ADC Conversion Start and auto clear
									*/
#define REFSEL    ((INT8U)0x20) 	/*!< ADCCRL[5]
									A/D Converter Reference Selection
									0 : Internal Reference (VDD)
									1 : External Reference (AVREF)
									*/
#define AFLAG     ((INT8U)0x10) 	/*!< ADCCRL[4]
									A/D Converter Operation State
									(This bit is cleared to '0' when the STBY bit
									is set to '0' or when the CPU is at STOP mode)
									0 : During A/D Conversion
									1 : A/D Conversion finished
									*/
#define ADSEL_AN0 ((INT8U)0x00) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0000 : AN0
									*/
#define ADSEL_AN1 ((INT8U)0x01) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0001 : AN1
									*/
#define ADSEL_AN2 ((INT8U)0x02) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0010 : AN2
									*/
#define ADSEL_AN3 ((INT8U)0x03) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0011 : AN3
									*/
#define ADSEL_AN4 ((INT8U)0x04) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0100 : AN4
									*/
#define ADSEL_AN5 ((INT8U)0x05) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0101 : AN5
									*/
#define ADSEL_AN6 ((INT8U)0x06) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									0110 : AN6
									*/
#define ADSEL_OP0OUT ((INT8U)0x09) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									1001 : OP0OUT
									*/
#define ADSEL_OP1OUT ((INT8U)0x0A) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									1010 : OP1OUT
									*/
#define ADSEL_VBGR ((INT8U)0x0F) 	/*!< ADCCRL[3:0]
									A/D Converter input selection
									1111 : VBGR
									*/
//------------------------------------------------------------------------------
//	ADCCRH (A/D Converter Counter High Register : 0xCB, Initial Value : 0x01 )
//------------------------------------------------------------------------------
#define ADCIFR              ((INT8U)0x80) 	/*!< ADCCRH[7]
											When ADC interrupt occurs, this bit becomes '1'.
											For clearing bit, write '0' to this bit
											or auto clear by INT_ACK signal.
											Writing '1' has no effect.
											0 : ADC Interrupt no generation
											1 : ADC Interrupt generation
											*/
#define TRIG_ADST           ((INT8U)0x00) 	/*!< ADCCRH[3]
											A/D Trigger Signal Selection
											0 : ADST
											*/
#define TRIG_T0_A_MATCH_SIG ((INT8U)0x08) 	/*!< ADCCRH[3]
											A/D Trigger Signal Selection
											1 : Timer 1 A match signal
											*/
#define ALIGN               ((INT8U)0x20) 	/*!< ADCCRH[2]
											A/D Converter data align selection.
											0 : MSB align (ADCDRH[7:0], ADCDRL[7:6])
											1 : LSB align (ADCRDH[1:0], ADCDRL[7:0])
											*/
#define CKSEL_DIV1          ((INT8U)0x00) 	/*!< ADCCRH[1:0]
											A/D Converter Clock selection
											00 : 4MHz
											*/
#define CKSEL_DIV2          ((INT8U)0x01) 	/*!< ADCCRH[1:0]
											A/D Converter Clock selection
											01 : 2MHz
											*/
#define CKSEL_DIV4          ((INT8U)0x02) 	/*!< ADCCRH[1:0]
											A/D Converter Clock selection
											10 : 1MHz
											*/
#define CKSEL_DIV8          ((INT8U)0x03) 	/*!< ADCCRH[1:0]
											A/D Converter Clock selection
											11 : 0.5MHz
											*/
//------------------------------------------------------------------------------
//	P2OD (P2 Open-drain Selection Register : 0xD2, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P23OD   ((INT8U)0x08)   /*!< P2OD[3]
								Configure Open-drain of P23 Port
								0 : Push-pull output
								1 : Open-drain output
								*/
#define P22OD   ((INT8U)0x04)   /*!< P2OD[2]
								Configure Open-drain of P22 Port
								0 : Push-pull output
								1 : Open-drain output
								*/
#define P21OD   ((INT8U)0x02)   /*!< P2OD[1]
								Configure Open-drain of P21 Port
								0 : Push-pull output
								1 : Open-drain output
								*/
#define P20OD   ((INT8U)0x01)   /*!< P2OD[0]
								Configure Open-drain of P20 Port
								0 : Push-pull output
								1 : Open-drain output
								*/
//------------------------------------------------------------------------------
//	P2PU (P2 Pull-up Resistor Selection Register : 0xD3, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P23PU   ((INT8U)0x08)   /*!< P2PU[3]
								Configure Pull-up Resistor of P23 Port
								0 : Disable, 1 : Enable
								*/
#define P22PU   ((INT8U)0x04)   /*!< P2PU[2]
								Configure Pull-up Resistor of P22 Port
								0 : Disable, 1 : Enable
								*/
#define P21PU   ((INT8U)0x02)   /*!< P2PU[1]
								Configure Pull-up Resistor of P21 Port
								0 : Disable, 1 : Enable
								*/
#define P20PU   ((INT8U)0x01)   /*!< P2PU[0]
								Configure Pull-up Resistor of P20 Port
								0 : Disable, 1 : Enable
								*/ 
//------------------------------------------------------------------------------
//	P2FSR (Port 2 Function Selection Register : 0xD4, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define P2FSR_P23_IO                ((INT8U)0x00)   /*!< P2FSR[7:6]
													P23 Function Select
													00 : I/O Port
													*/
#define P2FSR_P23_TXD_MOSI         ((INT8U)0x40)   /*!< P2FSR[7:6]
													P23 Function Select
													01 : TXD/MOSI Function
													*/
#define P2FSR_P23_AN5              ((INT8U)0x80)   /*!< P2FSR[7:6]
													P23 Function Select
													10 : AN5 Function
													*/
#define P2FSR_P22_IO                ((INT8U)0x00)   /*!< P2FSR[5:4]
													P22 Function Select
													00 : I/O Port
													*/
#define P2FSR_P22_RXD_MISO          ((INT8U)0x10)   /*!< P2FSR[5:4]
													P22 Function Select
													01 : RXD/MISO Function
													*/
#define P2FSR_P22_AN4              ((INT8U)0x20)   /*!< P2FSR[5:4]
													P22 Function Select
													10 : AN4 Function
													*/
#define P2FSR_P21_IO                ((INT8U)0x00)   /*!< P2FSR[3:2]
													P21 Function Select
													00 : I/O Port
													*/
#define P2FSR_P21_SCK              ((INT8U)0x04)   /*!< P2FSR[3:2]
													P21 Function Select
													01 : SCK Function
													*/
#define P2FSR_P21_SCL              ((INT8U)0x08)   /*!< P2FSR[3:2]
													P21 Function Select
													10 : SCL Function
													*/
#define P2FSR_P20_IO                ((INT8U)0x00)   /*!< P2FSR[1:0]
													P20 Function Select
													00 : I/O Port
													*/
#define P2FSR_P20_T2O_PWM2O         ((INT8U)0x01)   /*!< P2FSR[1:0]
													P20 Function Select
													01 : T2O/PWM2O Function
													*/
#define P2FSR_P20_SDA   			((INT8U)0x02)   /*!< P2FSR[1:0]
													P20 Function Select
													10 : SDA Function
													*/					  
//------------------------------------------------------------------------------
//	LVRCR (Low Voltage Reset Control Register : 0xD8, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define LVRVS_1P60 ((INT8U)0x00) 	/*!< LVRCR[2:1]
									LVR Voltage Select
									00 : 1.60V
									*/	
#define LVRVS_2P20 ((INT8U)0x02) 	/*!< LVRCR[2:1]
									LVR Voltage Select
									01 : 2.20V
									*/	
#define LVRVS_2P40 ((INT8U)0x04) 	/*!< LVRCR[2:1]
									LVR Voltage Select
									10 : 2.40V
									*/	
#define LVRVS_2P70 ((INT8U)0x06) 	/*!< LVRCR[2:1]
									LVR Voltage Select
									11 : 2.70V
									*/	

#define LVREN      ((INT8U)0x01) 	/*!< LVRCR[0]
									LVR Operation
									0 : LVR Enable
									1 : LVR Disable
									*/

//------------------------------------------------------------------------------
//	USTCR1 (USART Control 1 Register : 0xDA, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define USTMS_ASYN          ((INT8U)0x00) 	/*!< USTMS[7:6]
                                            Selects operation mode of USART
                                            00 : Asynchronous Mode (Normal Uart)
                                            */ 
#define USTMS_SYN           ((INT8U)0x40) 	/*!< USTMS[7:6]
                                            Selects operation mode of USART
                                            01 : Synchronous Mode (Synchronous Uart)
                                            */ 
#define USTMS_SPI           ((INT8U)0xC0) 	/*!< USTMS[7:6]
                                            Selects operation mode of USART
                                            11 : SPI Mode
                                            */ 
#define USTP_NO              ((INT8U)0x00) 	/*!< USTP[5:4]
                                            Selects Parity Generation and Check methods
                                            00 : No Parity
                                            */ 
#define USTP_EVEN            ((INT8U)0x20) 	/*!< USTP[5:4]
                                            Selects Parity Generation and Check methods
                                            10 : Even Parity
                                            */ 
#define USTP_ODD             ((INT8U)0x30) 	/*!< USTP[5:4]
                                            Selects Parity Generation and Check methods
                                            11 : Odd Parity
                                            */ 
#define USTS_5             ((INT8U)0x00) 	/*!< USTS[3:1]
                                            When in asynchronous or synchronous mode of operation,
                                            selects the length of data bits in frame.
                                            000 : 5-bit
                                            */ 
#define USTS_6             ((INT8U)0x02) 	/*!< USTS[3:1]
                                            When in asynchronous or synchronous mode of operation,
                                            selects the length of data bits in frame.
                                            001 : 6-bit
                                            */ 
#define USTS_7             ((INT8U)0x04) 	/*!< USTS[3:1]
                                            When in asynchronous or synchronous mode of operation,
                                            selects the length of data bits in frame.
                                            010 : 7-bit
                                            */ 
#define USTS_8             ((INT8U)0x06) 	/*!< USTS[3:1]
                                            When in asynchronous or synchronous mode of operation,
                                            selects the length of data bits in frame.
                                            011 : 8-bit
                                            */ 
#define USTS_9             ((INT8U)0x0E) 	/*!< USTS[3:1]
                                            When in asynchronous or synchronous mode of operation,
                                            selects the length of data bits in frame.
                                            111 : 9-bit
                                            */ 
#define ORD               ((INT8U)0x04) 	/*!< ORD
                                            This bit is in the same bit position with USTS0.
                                            In SPI mode, when set to one the MSB of the data byte is transmitted first.
                                            When set to zero the LSB of the data byte is transmitted first.
                                            0 : LSB First
                                            1 : MSB First
                                            */ 
#define CPOL               ((INT8U)0x01) 	/*!< CPOL
                                            Selects polarity of XCK in synchronous or SPI mode
                                            0 : TXD2 change @ Rising Edge, RXD2 change @ Falling Edge
                                            1 : TXD2 change @ Falling Edge, RXD2 change @ Rising Edge
                                            */ 
#define CPHA               ((INT8U)0x02) 	/*!< CPHA
                                            This bit is in the same bit position with USTS[0].
                                            In SPI mode, along with UCPOL bit, selects one of two clock formats for different kinds of synchronous serial peripherals.
                                            Leading edge means first XCK edge and trailing edge means 2nd or last clock edge of XCK in one XCK pulse.
                                            And Sample means detecting of incoming receive bit, Setup means preparing transmit data.
                                            CPOL   CPHA     Leading Edge        Trailing Edge
                                             0      0   :   Sample (Rising)     Setup (Falling)
                                             0      1   :   Setup (Rising)      Sample (Falling)
                                             1      0   :   Sample (Falling)    Setup (Rising)
                                             1      1   :   Setup (Falling)     Sample (Rising)
                                            */
//------------------------------------------------------------------------------
//	USTCR2 (USART Control 2 Register : 0xDB, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define DRIE       ((INT8U)0x80)   /*!< USTCR2[7]
                                    Interrupt enable bit for USART Data Register Empty.
                                    0 : Interrupt from DRE is inhibited (use polling)
                                    1 : When DRE is set, request an interrupt
                                    */
#define TXCIE       ((INT8U)0x40)   /*!< USTCR2[6]
                                    Interrupt enable bit for Transmit Complete.
                                    0 : Interrupt from TXC is inhibited (use polling)
                                    1 : When TXC is set, request an interrupt
                                    */
#define RXCIE       ((INT8U)0x20)   /*!< USTCR2[5]
                                    Interrupt enable bit for Receive Complete
                                    0 : Interrupt from RXC is inhibited (use polling)
                                    1 : When RXC is set, request an interrupt
                                    */
#define WAKEIE       ((INT8U)0x10)   /*!< USTCR2[4]
                                    Interrupt enable bit for Asynchronous Wake in STOP mode.
                                    When device is in stop mode,
                                    if RXD goes to LOW level an interrupt can be requested to wake-up system.
                                    0 : Interrupt from Wake is inhibited
                                    1 : When WAKE is set, request an interrupt
                                    */
#define TXE         ((INT8U)0x08)   /*!< USTCR2[3]
                                    Enables the transmitter unit.
                                    0 : Transmitter is disabled
                                    1 : Transmitter is enabled
                                    */
#define RXE         ((INT8U)0x04)   /*!< USTCR2[2]
                                    Interrupt enable bit for Receive Complete
                                    0 : Receiver is disabled
                                    1 : Receiver is enabled
                                    */
#define USTEN     ((INT8U)0x02)   /*!< USTCR2[1]
                                    Activate USART module by supplying clock.
                                    0 : USART is disabled
                                    1 : USART is enabled
                                    */
#define DBLS         ((INT8U)0x01)   /*!< USTCR2[0]
                                    This bit selects receiver sampling rate (only UART mode)
                                    0 : Normal asynchronous operation
                                    1 : Double Speed asynchronous operation
                                    */
//------------------------------------------------------------------------------
//	USTCR3 (USART Control 3 Register : 0xDC, Initial Value : 0x00 )
//------------------------------------------------------------------------------
#define MASTER      ((INT8U)0x80)   /*!< USTCR3[7]
                                    Selects master or slave in SPI or Synchronous mode operation and controls the direction of XCK pin.
                                    0 : Slave mode operation and XCK is input pin.
                                    1 : Master mode operation and XCK is output pin
                                    */
#define LOOPS       ((INT8U)0x40)   /*!< USTCR3[6]
                                    Controls the Loop Back mode of USART2, for test mode
                                    0 : Normal operation
                                    1 : Loop Back mode
                                    */
#define DISSCK      ((INT8U)0x20)   /*!< USTCR3[5]
                                    In Synchronous mode of operation, selects the waveform of XCK output.
                                    0 : XCK is free-running while USART is enabled in synchronous master mode.
                                    1 : XCK is active while any frame is on transferring.
                                    */
#define USTSSEN       ((INT8U)0x10)   /*!< USTCR3[4]
                                    Controls the functionality of SS2 pin in master SPI mode.
                                    0 : SS2 pin is normal GPIO or other primary function
                                    1 : SS2 output to other slave device
                                    */
#define FXCH       ((INT8U)0x10)   /*!< USTCR3[3]
									SPI port function exchange control bit (only SPI mode)
									0 : No effect
									1 : Exchange MOSI and MISO function
									*/
#define USTSB        ((INT8U)0x04)   /*!< USTCR3[2]
                                    Selects the length of stop bit in Asynchronous or Synchronous mode of operation.
                                    0 : 1 Stop bit
                                    1 : 2 Stop bit
                                    */
#define USTTX8         ((INT8U)0x02)   /*!< USTCR3[1]
                                    The ninth bit of data frame in Asynchronous or Synchronous mode of operation.
                                    Write this bit first before loading the UDATA register.
                                    0 : MSB (9th bit) to be transmitted is '0'
                                    1 : MSB (9th bit) to be transmitted is '1'
                                    */
#define USTRX8         ((INT8U)0x01)   /*!< USTCR3[0]
                                    The ninth bit of data frame in Asynchronous or Synchronous mode of operation.
                                    Read this bit first before reading the receive buffer.
                                    0 : MSB (9th bit) received is '0'
                                    1 : MSB (9th bit) received is '1'
                                    */
//------------------------------------------------------------------------------
//	USTST (USART2 Status Register : 0xDD, Initial Value : 0x80 )
//------------------------------------------------------------------------------
#define DRE   ((INT8U)0x80) 	/*!< USTST[7]
                                The DRE flag indicates if the transmit buffer (UDATA) is ready to be loaded with new data.
                                If DRE is '1', it means the transmit buffer is empty and can hold one or two new data.
                                This flag can generate an UDRE interrupt.
                                Writing '0' to this bit position will clear UDRE flag.
                                0 : Transmit buffer is not empty.
                                1 : Transmit buffer is empty.
                                */
#define TXC     ((INT8U)0x40) 	/*!< USTST[6]
                                This flag is set when the entire frame in the transmit shift register has been shifted out
                                and there is no new data currently present in the transmit buffer.
                                This flag is automatically cleared when the interrupt service routine of a TXC interrupt is executed.
                                It is also cleared by writing ¡®0¡¯ to this bit position. This flag can generate a TXC interrupt.
                                0 : Transmit buffer is not empty.
                                1 : Transmit buffer is empty.
                                */
#define RXC     ((INT8U)0x20) 	/*!< USTST[5]
                                This flag is set when there are unread data in the receive buffer and cleared when all the data in the receive buffer are read.
                                The RXC flag can be used to generate a RXC interrupt.
                                0 : There is no data unread in the receive buffer
                                1 : There are more than 1 data in the receive buffer
                                */
#define WAKE     ((INT8U)0x10) 	/*!< USTST[4]
                                This flag is set when the RX pin is detected low while the CPU is in stop mode.
                                This flag can be used to generate a WAKE interrupt.
                                This bit is set only when in asynchronous mode of operation.
                                0 : No WAKE interrupt is generated.
                                1 : WAKE interrupt is generated.
                                */
#define SOFTRST  ((INT8U)0x08) 	/*!< USTST[3]
                                This is an internal reset and only has effect on USART.
                                Writing '1' to this bit initializes the internal logic of USART and is auto cleared.
                                0 : No operation
                                1 : Reset USART
                                */
#define DOR      ((INT8U)0x04) 	/*!< USTST[2]
                                This bit is set if a Data OverRun occurs.
                                While this bit is set, the incoming data frame is ignored.
                                This flag is valid until the receive buffer is read.
                                0 : No Data OverRun
                                1 : Data OverRun detected
                                */
#define FE       ((INT8U)0x02) 	/*!< USTST[1]
                                This bit is set if the first stop bit of next character in the receive buffer is detected as '0'.
                                This bit is valid until the receive buffer is read.
                                0 : No Frame Error
                                1 : Frame Error detected
                                */
#define PE       ((INT8U)0x01) 	/*!< USTST[0]
                                This bit is set if the next character in the receive buffer has a Parity Error when received while Parity Checking is enabled.
                                This bit is valid until the receive buffer is read.
                                0 : No Parity Error
                                1 : Parity Error detected
                                */
//------------------------------------------------------------------------------
//	RSTFR (Reset Flag Register : 0xE8, Initial Value : 0x80 )
//------------------------------------------------------------------------------
#define PORF    ((INT8U)0x80)   /*!< RSTFR[7]
                                Power-On Reset flag bit.
                                The bit is reset by writing '0' to this bit.
                                0 : No detection, 1 Detection
                                */
#define EXTRF   ((INT8U)0x40)   /*!< RSTFR[6]
                                External Reset (RESETB) flag bit.
                                The bit is reset by writing '0' to this bit or by Power-On Reset.
                                0 : No detection, 1 Detection
                                */
#define WDTRF   ((INT8U)0x20)   /*!< RSTFR[5]
                                Watch Dog Reset flag bit.
                                The bit is reset by writing '0' to this bit or by Power-On Reset.
                                0 : No detection, 1 Detection
                                */
#define OCDRF   ((INT8U)0x10)   /*!< RSTFR[4]
                                On-chip debugger reset flag bit.
                                The bit is reset by writing '0' to this bit or by Power-On Reset.
                                0 : No detection, 1 Detection
                                */
#define LVRF    ((INT8U)0x08)   /*!< RSTFR[3]
                                Low Voltage Reset flag bit.
                                The bit is reset by writing '0' to this bit or by Power-On Reset.
                                0 : No detection, 1 Detection
                                */

#endif
/* --------------------------------- End Of File ------------------------------ */
