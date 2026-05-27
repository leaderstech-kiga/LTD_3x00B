/**
*   @file       a96l41x_usart.c
*   @brief      A96L416 usart peripheral function 
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

/*******************************************************************************
* Included File
*******************************************************************************/
#include "a96l41x_usart.h"

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/
char USART_tx_queue[ USART_QUEUE_SIZE ];
char USART_tx_front, USART_tx_rear;

char USART_rx_queue[ USART_QUEUE_SIZE ];
char USART_rx_front, USART_rx_rear;
/*******************************************************************************
* Private Function Prototype
*******************************************************************************/
char USART_tx_queue_is_full()
{
	return ( (USART_tx_rear + 1 ) % USART_QUEUE_SIZE == USART_tx_front );
}

char USART_tx_queue_is_empty()
{
	return ( USART_tx_rear == USART_tx_front );
}

char USART_rx_queue_is_full()
{
	return ( (USART_rx_rear + 1 ) % USART_QUEUE_SIZE == USART_rx_front );
}

char USART_rx_queue_is_empty()
{
	return ( USART_rx_rear == USART_rx_front );
}

/*******************************************************************************
* Public Function
*******************************************************************************/

/**
* @brief		Initialize usart peripheral(For USART & UART)
* @param   speed			This parameter contains the buadrate of uart/usart.
* @param   data_length	This parameter contains the length of data as uart/usart
*
*					- USART_DATA_5BIT = 0
*					- USART_DATA_6BIT = 1
*					- USART_DATA_7BIT = 2
*					- USART_DATA_8BIT = 3
*					- USART_DATA_9BIT = 7
* @param   stop_bits	This parameter contains the bits of stop as uart/usart. 
*
*					- USART_STOP_1BIT = 0
*					- USART_STOP_2BIT = 1
* @param   parity	This parameter contains the bit of parity as uart. 
*
*					- USART_PARITY_NO = 0
*					- USART_PARITY_EVEN = 2
*					- USART_PARITY_ODD = 3
* @param   mode		This parameter contains the enable of transmit or receive mode(uart/usart). 
*
*					- USART_TX_RX_MODE = 0
* 					- USART_RX_MODE = 1
*					- USART_TX_MODE = 2
* @return		None
*/
void USART_Initial(uint32_t speed, uint8_t data_length, uint8_t stop_bits, uint8_t parity, uint8_t mode)
{
	uint32_t buadrate_val = 0;
	uint8_t temp_reg, temp_reg2;
	
	temp_reg = USTCR1;
	temp_reg &= ~(0x3 << 6);  
	//operation mode : UART(default), if used USART, set USART_Clock_Initial() + USART_Initial();
	temp_reg |= ((parity << 4) | (data_length <<  1));
	
	temp_reg2 = USTCR2;
	if(mode == USART_RX_MODE)
		temp_reg2 |= (1 << 2);	
	else if(mode == USART_TX_MODE)
		temp_reg2 |= (1 << 3);
	else
		temp_reg2 |= ((1 << 2) | (1 << 3));
	
	temp_reg2 |= (1 << 1);	
	
	//UCTRL2 |= (0x1 << 0); //if using Double Speed operation, Enable UCTRL2 |= (0x1 << 0)
	if(USTCR2 & 0x1) //Double Speed operation(only UART)
	{
		buadrate_val = ((Clock_GetSystemFreq() / speed )  >> 3) * 10;
	}
	else if(USTCR1 & 0x40) //Synchronous mode(USART)
	{
		buadrate_val = ((Clock_GetSystemFreq() / speed )  >> 1) * 10;
	}
	else  //Normal Speed operation
	{
		buadrate_val = ((Clock_GetSystemFreq() / speed )  >> 4) * 10;
	}
	
	buadrate_val = ((buadrate_val + 5) / 10) - 1; //round
	
	
	if(!(USTCR1 & 0x40)) //Not Synchronous mode(USART)
		USTST	|= (1 << 3);			// USART block reset
	
	
	USTCR1 = temp_reg;
	USTCR3 |= (stop_bits<< 2);
	
	USTBD = buadrate_val;
	
	USTCR2 = temp_reg2;	
}



/**
* @brief		Transmit data through the USART peripheral for polling mode.
* @param   send_data   This parameter contains the data to transmit. 
* @param   count		This parameter contains the number of write length.
* @return		None
*/
void USART_SendDataWithPolling(uint8_t *send_data, uint8_t count)
{
	uint8_t i;
	
	for(i = 0; i < count; i++)
	{
		while(!(USTST & ((1 << 7))));
		USTDR = send_data[i];
	}
}

/* --------------------------------- End Of File ------------------------------ */