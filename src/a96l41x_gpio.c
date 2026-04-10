/**
*   @file       a96l41x_gpio.c
*   @brief      A96l41x gpio peripheral function 
*   @author     Application Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       29. MAY. 2020
*
* Copyright(C) 2020, ABOV Semiconductor
* All rights reserved.
*
********************************************************************************
* ABOV Disclaimer
*
*IMPORTANT NOTICE - PLEASE READ CAREFULLY
*ABOV Semiconductor ("ABOV") reserves the right to make changes, correction2s, enhancements, 
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include "a96l41x_gpio.h"

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/

/*******************************************************************************
* Private Function Prototype
*******************************************************************************/

/*******************************************************************************
* Public Function
*******************************************************************************/

/**
* @brief	Initialize GPIO(PCU) peripheral
* @param    	None
* @return		None
*/
void Port_Initial(void)
{
		// P0 setting
		// P00:   C_VDD
		
		P0		= _1100_0000;		// 0 : Low,	1 : High
		P0OD	= _0000_0000;		// 0 : Disable,	1 : Enable (Open-drain)
		P0PU	= _0000_0000;		// 0 : Disable,	1 : Enable (Pull-up)
		P0IO	= _1100_0001;		// 0 : Input,	1 : Output
		P0FSRH  = 0
		| ( 0 << 6)       //P07     // 0 : I/O (EINT3), 1 : ICS1									// LED_R				
		| ( 0 << 4)       //P06     // 0 : I/O (EINT2), 1 : ICS0									// LED_G
		| ( 1 << 2)       //P05     // 0 : I/O, 1 : OP0P,    										// SENISNG OP0P
		| ( 1 << 0);      //P04     // 0 : I/O, 1 : OP0N										// SENSING OP0N
		
		P0FSRL  = 0
		| ( 1 << 6)       //P03     // 0 : I/O, 1 : OP0OUT,   2 : AN3,      3 : Not used					// SENSING IN ADC 
		| ( 2 << 4)       //P02     // 0 : I/O, 1 : OP1P,   2 : AN2,      3 : Not used					// BAT_ADC
		| ( 0 << 2)       //P01     // 0 : I/O (EINT1),	1 : OP1N,  2 : AN1,	3 : Not used				// EMR_IO
		| ( 0 << 0);      //P00     // 0 : I/O (EINT0),	1 : OP1OUT,   2 : AN0,	3 : Not used			// C_VDD
		
		P0DB	= 0
		| ( 0 << 6) 	  //DBCLK[1:0] 	// 00 : fx (SCLK), 	01 : fx/4,	10 : fx/16,	11 : fx/64
		| ( 0 << 3) 	  //P07DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 2) 	  //P06DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 1) 	  //P01DB 	// 0 : Disable, 1 : Enable
		| ( 0 << 0);	  //P00DB 	// 0 : Disable, 1 : Enable
		
		// P1 setting    
		// P14: /AUDIO_ON		
		P1		= _0001_0000;		// 0 : Low,	1 : High								// audio off
		P1OD	= _0000_0000;		// 0 : Disable,	1 : Enable (Open-drain)
		P1PU	= _0000_0100;		// 0 : Disable,	1 : Enable (Pull-up)
		P1IO	= _0011_0011;		// 0 : Input,	1 : Output
		
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

		// P2 setting    
		//P20:	/CS		
		P2	= _0000_1111;		// 0 : Low,	1 : High
		P2OD	= _0000_0000;		// 0 : Disable,	1 : Enable (Open-drain)
		P2PU	= _0000_0000;		// 0 : Disable,	1 : Enable (Pull-up)
		P2IO	= _0000_1111;		// 0 : Input,	1 : Output
		
		P2FSR   = 0
		| ( 1 << 6) 		// 0 : P23,  1 : TXD/MOSI,	2 : AN5,     3 : Not used					//mosi
		| ( 1 << 4) 		// 0 : P22,  1 : RXD/MISO,	2 : AN4,     3 : Not used					// miso
		| ( 1 << 2)         // 0 : P21 (EC2),  1 : SCK,   2 : (SCL),     3 : Not used						// sck
		| ( 0 << 0);        // 0 : P20 (EINT12),  1 : T2O/PWM2O,   2 : SDA,     3 : Not used			// cs 

}

/**
* @brief	Configures the selected port(x) pins as output pin.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @param    mode	This parameter contains the pin mode(Push-Pull : 0, Open-Drain :1)
* @return		None
*/
void Port_SetOutputpin(uint8_t port, uint8_t pin, uint8_t mode)
{

		switch(port)
		{
			case 0:
				P0IO |= (1<<pin);
				P0OD |= (mode<<pin);
			break;
			case 1:
				P1IO |= (1<<pin);
				P1OD |= (mode<<pin);
			break;
			case 2:
				P2IO |= (1<<pin);
				P2OD |= (mode<<pin);
			break;
			default:
			break;
		}
}

/**
* @brief	Configures the selected port(x) pins as input pin.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @param    mode	This parameter contains the pin mode(Disable : 0, Pull-UP : 1)
* @return	None
*/
void Port_SetInputpin(uint8_t port, uint8_t pin, uint8_t mode)
{
		switch(port)
		{
			case 0:
				P0IO &= ~(1<<pin);
				P0PU |= (mode<<pin);
			break;
			case 1:
				P1IO &= ~(1<<pin);
				P1PU |= (mode<<pin);
			break;
			case 2:
				P2IO &= ~(1<<pin);
				P2PU |= (mode<<pin);
			break;
			default:
			break;			
		}
}

/**
* @brief	Configures the selected port(x) pins as several kinds alternative function pin.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @param    val		This parameter contains the value of alternative function bit
* @return	None
*/
void Port_SetAlterFunctionpin(uint8_t port, uint8_t pin, uint8_t val)
{
	uint8_t temp_reg;
	
		switch(port)
		{
			case 0:
				if(pin > 0x3)
				{
					temp_reg = P0FSRH;
					temp_reg &= ~(0x3<<((pin-4)<<1));
					temp_reg |= (val<<((pin-4)<<1));
					
					P0FSRH = temp_reg;
				}
				else
				{
					temp_reg = P0FSRL;
					temp_reg &= ~(0x3<<(pin<<1));
					temp_reg |= (val<<(pin<<1));
					
					P0FSRL = temp_reg;
				}
			break;
			case 1:
				if(pin > 0x3)
				{
					temp_reg = P1FSRH;
					temp_reg &= ~(0x3<<((pin-4)<<1));
					temp_reg |= (val<<((pin-4)<<1));
					
					P1FSRH = temp_reg;
				}
				else
				{
					temp_reg = P1FSRL;
					temp_reg &= ~(0x3<<(pin<<1));
					temp_reg |= (val<<(pin<<1));	

					P1FSRL = temp_reg;
				}	
			break;
			case 2:
				if(pin > 0x1)
				{
					temp_reg = P2FSR;
					temp_reg  &= ~(1<<pin);
					temp_reg  |= (val<<pin);
					
					P2FSR = temp_reg;
				}
				else
				{
					temp_reg = P2FSR;
					temp_reg  &= ~(0x3<<(pin<<1));
					temp_reg  |= (val<<(pin<<1));	

					P2FSR = temp_reg;
				}
			break;
			default:
			break;			
		}
}


/**
* @brief	Configures the selected port(x) pins as output high.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @return	None
*/
void Port_SetOutputHighpin(uint8_t port, uint8_t pin)
{

		switch(port)
		{
			case 0:
				P0 |= (1<<pin);
			break;
			case 1:
				P1 |= (1<<pin);
			break;
			case 2:
				P2 |= (1<<pin);
			break;
			default:
			break;			
		}
}


/**
* @brief	Configures the selected port(x) pins as output low.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @return	None
*/
void Port_SetOutputLowpin(uint8_t port, uint8_t pin)
{
		
		switch(port)
		{
			case 0:
				P0 &= ~(1<<pin);
			break;
			case 1:
				P1 &= ~(1<<pin);
			break;
			case 2:
				P2 &= ~(1<<pin);
			break;
			default:
			break;			
		}
}


/**
* @brief	Configures the selected port(x) pins as output toggle.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @return	None
*/
void Port_SetOutputTogglepin(uint8_t port, uint8_t pin)
{
		
		switch(port)
		{
			case 0:
				P0 ^= (1<<pin);
			break;
			case 1:
				P1 ^= (1<<pin);
			break;
			case 2:
				P2 ^= (1<<pin);
			break;
			default:
			break;			
		}
}


/**
* @brief	Get the value of selected port(x) pins as input pin.
* @param    port	This parameter contains the pin port
* @param    pin		This parameter contains the pin number
* @return	Value of Input pin(High : 1, Low :0)
*/
uint8_t Port_GetInputpinValue(uint8_t port, uint8_t pin)
{
	 uint8_t result;
		
		switch(port)
		{
			case 0:
				result = (P0 & (1<<pin)) ? 1 : 0;
			break;
			case 1:
				result = (P1 & (1<<pin)) ? 1 : 0;
			break;
			case 2:
				result = (P2 & (1<<pin)) ? 1 : 0;
			break;
			default:
			break;			
		}
		
		return result;
}

/**
* @brief	Enable the channel of external interrupt pin as input.
* @param    channel	This parameter contains the channel of external interrupt.
* @return	None
*/
void Port_EnableInterrupt(uint8_t channel)
{	
		switch(channel)
		{
			case 0:
				IE |= (1<<0);
			break;
			case 1:
				IE |= (1<<1);
			break;
			case 2:
				IE |= (1<<2);
			break;
			case 3:
				IE |= (1<<3);
			break;
			case 4:
			case 5:
			case 6:
			case 7:				
			case 8:
			case 10:
				IE |= (1<<4);
			break;
			case 11:
				IE |= (1<<5);
			break;
			case 12:
				IE1 |= (1<<5);
			break;
			default:
			break;			
		}
}

/**
* @brief	Disable the channel of external interrupt pin as input.
* @param    channel		This parameter contains the channel of external interrupt.
* @return	None
*/
void Port_DisableInterrupt(uint8_t channel)
{
		switch(channel)
		{
			case 0:
				IE &= ~(1<<0);
			break;
			case 1:
				IE &= ~(1<<1);
			break;
			case 2:
				IE &= ~(1<<2);
			break;
			case 3:
				IE &= ~(1<<3);
			break;
			case 4:
			case 5:
			case 6:
			case 7:				
			case 8:
			case 10:
				IE &= ~(1<<4);
			break;
			case 11:
				IE &= ~(1<<5);
			break;
			case 12:
				IE1 &= ~(1<<5);
			break;
			default:
			break;			
		}
}


/**
* @brief	Configure the channel of external interrupt pin as input.
* @param    channel		This parameter contains the channel of external interrupt.
* @param    edge		This parameter contains the edge of external interrupt,
*
*				- NONE          = 0
*				- RISING_EDGE   = 1
*				- FALLING_EDGE  = 2
*				- BOTH_EDGE     = 3
* @return	None
*/

void Port_ConfigureInterrupt(uint8_t channel, uint8_t edge)
{
	uint8_t temp_reg;
	
		switch(channel)
		{
			case 0:
					temp_reg = EIPOL0;
					temp_reg &= ~(0x3 << 0);   // 0*2 = 0
					temp_reg |= (edge  << 0);
					EIPOL0 = temp_reg;
					break;
			case 1:
					temp_reg = EIPOL0;
					temp_reg &= ~(0x3 << 2);   // 1*2 = 2
					temp_reg |= (edge  << 2);
					EIPOL0 = temp_reg;
					break;
			case 2:
					temp_reg = EIPOL0;
					temp_reg &= ~(0x3 << 4);   // 2*2 = 4
					temp_reg |= (edge  << 4);
					EIPOL0 = temp_reg;
					break;
			case 3:
					temp_reg = EIPOL0;
					temp_reg &= ~(0x3 << 6);   // 3*2 = 6
					temp_reg |= (edge  << 6);
					EIPOL0 = temp_reg;
					break;
			case 4:
			case 5:
			case 6:
			case 7:	
			case 8:
			break;
			case 10:
				temp_reg = EIPOL1;			
				temp_reg &= ~(0x3<<0);
				temp_reg |= (edge<<0);
				
				EIPOL1 = temp_reg;
			break;
			case 11:
				temp_reg = EIPOL1;
				temp_reg &= ~(0x3<<2);
				temp_reg |= (edge<<2);
				
				EIPOL1 = temp_reg;
			break;
			case 12:
				temp_reg = EIPOL1;
				temp_reg &= ~(0x3<<4);
				temp_reg |= (edge<<4);
				
				EIPOL1 = temp_reg;
			break;
			default:
			break;			
		}
}

/**
* @brief	Get the external interrupt status.
* @param    channel		This parameter contains the channel of external interrupt.
* @return	Value of status(interrupt occur : 1, not occur : 0)
*/
uint8_t Port_GetInterruptStatus(uint8_t channel)
{
		uint8_t result;
	
		switch(channel)
		{
			case 0:
				result = (EIFLAG & (1<<0)) ? 1 : 0;
			break;
			case 1:
				result = (EIFLAG & (1<<1)) ? 1 : 0;
			break;
			case 2:
				result = (EIFLAG & (1<<2)) ? 1 : 0;
			break;
			case 3:
				result = (EIFLAG & (1<<3)) ? 1 : 0;
			break;
			case 4:
			case 5:
			case 6:
			case 7:				
			case 8:
			case 10:
				result = (EIFLAG & (1<<4)) ? 1 : 0;
			break;
			case 11:
				result = (EIFLAG & (1<<5)) ? 1 : 0;
			break;
			case 12:
				result = (EIFLAG & (1<<6)) ? 1 : 0;
			break;
			default:
			break;			
		}
		
		return result;
}

/**
* @brief	Clear the external interrupt status.
* @param    channel	    This parameter contains the channel of external interrupt.
*
*					-  EXTINT_CH0 = 0       
*					-  EXTINT_CH1 = 1       
*					-  EXTINT_CH2 = 2       
*					-  EXTINT_CH3 = 3       
*					-  EXTINT_CH4 = 4       
*					-  EXTINT_CH5 = 5       
*					-  EXTINT_CH6 = 6       
*					-  EXTINT_CH7 = 7       
*					-  EXTINT_CH8 = 8       
*					-  EXTINT_CH9 = 9       
*					-  EXTINT_CH10 = 10       
*					-  EXTINT_CH11 = 11       
*					-  EXTINT_CH12 = 12       
* @return	None
*/
void Port_ClearInterruptStatus(uint8_t channel)
{
		switch(channel)
		{
			case 0:
				EIFLAG = ~(1<<0);
			break;
			case 1:
				EIFLAG = ~(1<<1);
			break;
			case 2:
				EIFLAG = ~(1<<2);
			break;
			case 3:
				EIFLAG = ~(1<<3);
			break;
			case 4:
			case 5:
			case 6:
			case 7:			
			case 8:
			case 10:
				EIFLAG	= ~(1<<4);
			break;
			case 11:
				EIFLAG	= ~(1<<5);
			break;
			case 12:
				EIFLAG	= ~(1<<6);
			break;
			default:
			break;			
		}
}

/* --------------------------------- End Of File ------------------------------ */
