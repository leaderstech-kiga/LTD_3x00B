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
* @brief		Initialize the clock of usart peripheral(For only USART)
* @param   clock_en	This parameter contains the selection of USART mode.
*
*					- USART_SYNC_DISABLE = 0
*					- USART_SYNC_ENABLE  = 1
* @param   master_en	This parameter contains the enable of transmit or receive mode(uart/usart). 
*
*					- USART_SLAVE_MODE    = 0
*					- USART_MASTER_MODE = 1
* @return		None
*/
void USART_Clock_Initial(uint32_t clock_en, uint8_t master_en)
{
	uint8_t temp_reg, temp_reg2;
	
	temp_reg = USTCR1;
	temp_reg &= ~(0x3 << 6);
	temp_reg |= (clock_en << 6);
	
	temp_reg2 = USTCR3;
	temp_reg2 &= ~(0x1 << 7);
	temp_reg2 |= (master_en << 7);
	
	USTST	|= (1 << 3);			// USART block reset
	
	USTCR1 = temp_reg;
	USTCR3 = temp_reg2;
}

/**
* @brief		Enables or disables the USART peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void USART_Enable(uint8_t enable)
{
		if(enable == TRUE)
		{
			USTCR2 |= (1 << 1);
		}
		else
		{
			USTCR2 &= ~(1 << 1);
		}
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

/**
* @brief		Transmit data through the USART peripheral for interrupt mode.
* @param   send_data   This parameter contains the data to transmit. 
* @return		None
*/
void USART_SendDataWithInterrupt(uint8_t send_data)
{
	if( (USTCR2 & ((1 << 7))) || (USTCR2 & ((1 << 6))))           
	{
		if ( USART_tx_queue_is_empty() == FALSE )			
		{
			while ( USART_tx_queue_is_full() == TRUE );	
													 
													
		}

		USART_ConfigureInterrupt(USART_TX_COMPLETE_INT, FALSE);
		
		USART_tx_queue [ USART_tx_rear ] = send_data;				
		USART_tx_rear = ++USART_tx_rear % USART_QUEUE_SIZE;						
		USART_ConfigureInterrupt(USART_TX_COMPLETE_INT, TRUE);
		
		USTCR2 |= ((1 << 7));	
	}	
}

/**
* @brief		Receive data through the USART peripheral for polling mode.
* @param   receive_data   This parameter contains the data to receive. 
* @param   count		This parameter contains the number of receive length.
* @return		received data
*/
void USART_ReceiveDataWithPolling(uint8_t *receive_data, uint8_t count)
{
	uint8_t i;
	
	for(i = 0; i < count; i++)
	{
		while(( USTST & (1 << 5))!= (1 << 5));		// Wait Data in
		receive_data[i] = USTDR;
	}

}

/**
* @brief		Receive data through the USART peripheral for interrupt mode.
* @return		received data
*/
uint8_t USART_ReceiveDataWithInterrupt(void)
{
	uint8_t temp;
	
	while ( USART_rx_queue_is_empty() == TRUE ); 

	USART_ConfigureInterrupt(USART_RX_COMPLETE_INT, FALSE);
	temp = USART_rx_queue [ USART_rx_front ];

	USART_rx_front = ++USART_rx_front % USART_QUEUE_SIZE;
	USART_ConfigureInterrupt(USART_RX_COMPLETE_INT, TRUE);

	return temp;
}

/**
* @brief		Configure the enable or disable USART interrupt.
* @param   flag   This parameter contains the selection of this function. 
*
*					- USART_RX_COMPLETE_INT = 0
*					- USART_TX_COMPLETE_INT = 1
* 					- USART_DATA_EMPTY_INT = 2
* 					- USART_WAKEUP_INT = 3
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void USART_ConfigureInterrupt(uint8_t flag, uint8_t enable)
{
	uint8_t temp_reg, temp_reg2;

	if(flag == USART_TX_COMPLETE_INT)
	{
		temp_reg = IE2;
		temp_reg &= ~(0x1 << 5);
		temp_reg |= (enable << 5);
		
		temp_reg2 = USTCR2;
		temp_reg2 &= ~(0x1 << (5 + flag));
		temp_reg2 |= (enable << (5 + flag));
		
		IE2 = temp_reg;
		USTCR2 = temp_reg2;
	}
	else if(flag == USART_RX_COMPLETE_INT)
	{
		temp_reg = IE2;
		temp_reg &= ~(0x1 << 4);
		temp_reg |= (enable << 4);
		
		temp_reg2 = USTCR2;
		temp_reg2 &= ~(0x1 << (5 + flag));
		temp_reg2 |= (enable << (5 + flag));
		
		IE2 = temp_reg;
		USTCR2 = temp_reg2;
	}
	else if(flag == USART_DATA_EMPTY_INT)
	{
		temp_reg = USTCR2;
		temp_reg &= ~(0x1 << (5 + flag));
		temp_reg |= (enable << (5 + flag));
		USTCR2 = temp_reg;
	}
	else if(flag == USART_WAKEUP_INT)
	{
		temp_reg = USTCR2;
		temp_reg &= ~(0x1 << 4);
		temp_reg |= (enable << 4);
		USTCR2 = temp_reg;
	}
}

/**
* @brief		Get the USART interrupt status.
* @return		Value of status(interrupt flag)
*/
uint8_t USART_GetInterruptStatus(void)
{
	uint8_t result;
	
	result = USTST;
	
	return result;
}

/**
* @brief		Set the function of wake up interrupt.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 		
* @return		None

*/
void USART_ConfigureWakeUp(uint8_t enable)
{
	uint8_t temp_reg;
	
	temp_reg = USTCR2;
	temp_reg &= ~(1 << 4);
	temp_reg |= (enable << 4);
	
	USTCR2 = temp_reg;
}

/**
* @brief		Clear the flag of wake up interrupt.
* @param   		None	
* @return		None

*/
void USART_ClearWakeUpFlag(void)
{
	USTST &= ~(1 << 4);
}

//==============================================================================
// interrupt routines
//==============================================================================
/*
void USART_RXInt_Handler() interrupt USART_RX_VECT
{	
	unsigned char temp, temp2;
	
	temp    = USTST;
	temp2   = USTCR2;
	
	if ( USART_rx_queue_is_full () )
	{
		if ((temp & (1 << 2) )||(temp & (1 << 1) )||(temp & (1 << 0) ))
		{
			USART_rx_queue [ USART_rx_rear ] = USTDR;
			USART_rx_rear = ++USART_rx_rear % USART_QUEUE_SIZE;
		}
		USART_rx_queue [ USART_rx_rear ] = USTDR;
		USART_rx_rear = ++USART_rx_rear % USART_QUEUE_SIZE;
			
		USART_rx_front = USART_rx_rear = 0;	
    USART_rx_queue[ USART_rx_front ] = 0;
	}
	else
	{
		USART_rx_queue [ USART_rx_rear ] = USTDR;
		USART_rx_rear = ++USART_rx_rear % USART_QUEUE_SIZE;
	}
}

void USART_TXInt_Handler() interrupt USART_TX_VECT
{
	if ( USART_tx_queue_is_empty() == TRUE )
	{
		if ( (USTST & ((1 << 6))) )				
		{
			USTCR2	&= ~( ((1 << 7)) | ((1 << 6)) );
			USTST	&= ~( (1 << 7) + (1 << 6) );
		}
		else								
		{
			USTCR2	&= ~( (1 << 7) );			
											
		}
	}
	else
	{
		USTDR = USART_tx_queue [ USART_tx_front ];
    USART_tx_queue [ USART_tx_front ] = 0;
		USART_tx_front = ++USART_tx_front % USART_QUEUE_SIZE;
	}
}
*/

/* --------------------------------- End Of File ------------------------------ */