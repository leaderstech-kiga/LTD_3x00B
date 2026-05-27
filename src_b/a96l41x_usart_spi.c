/**
*   @file       a96l41x_usart_spi.c
*   @brief      A96L41x usart spi peripheral function 
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
#include "a96l41x_usart_spi.h"

/*******************************************************************************
* Private Pre-processor Definition & Macro
*******************************************************************************/

/*******************************************************************************
* Private Typedef
*******************************************************************************/

/*******************************************************************************
* Private Variable
*******************************************************************************/
uint8_t xdata spi_writeBuff0[SPI_MAX_BUFFER_SIZE] _at_ 0x100;
uint8_t spi_writeNum0 = 0;

uint8_t xdata temp_spi_readBuff0[SPI_MAX_BUFFER_SIZE] _at_ 0x140;
uint8_t *spi_readBuff0;
uint8_t spi_readNum0 = 0;

/*******************************************************************************
* Private Function Prototype
*******************************************************************************/

/*******************************************************************************
* Public Function
*******************************************************************************/

/**
* @brief		Initialize usart(spi) peripheral
* @param   mode	This parameter contains the enable of master or slave mode(SPI). 
*
* 					- SPI_SLAVE_MODE		= 0
* 					- SPI_MASTER_MODE 		= 1
* @param   speed			This parameter contains the buadrate of SPI.
* @param   first_bit	This parameter contains the transmitted first as SPI.
*
* 					- SPI_LSB 		= 0
*					- SPI_MSB		= 1
* @param   cpol	This parameter contains the polarity(CPOL) as SPI. 
*
* 					- SPI_CPOL_LOW 	= 0
*					- SPI_CPOL_HIGH = 1
* @param   cpha	This parameter contains the phase(CPHA) as SPI. 
*
* 					- SPI_CPHA_1EDGE = 0
*					- SPI_CPHA_2EDGE = 1
* @param   direction	This parameter contains the enable of transmit or receive mode(SPI). 
*
* 					- SPI_TX_RX_MODE		= 0
* 					- SPI_RX_MODE			= 1
*					- SPI_TX_MODE 			= 2
* @param   ss_mode	This parameter contains the enable of SS function(SPI).
*
* 					- SPI_SS_HW_DISABLE			= 0
*					- SPI_SS_HW_ENABLE 			= 1
* @return		None
*/
void USART_SPI_Initial(uint8_t mode, uint32_t speed, uint8_t first_bit, uint8_t cpol, uint8_t cpha, uint8_t direction, uint8_t ss_mode)
{
	uint32_t buadrate_val = 0;
	uint8_t temp_reg;
	
	spi_writeNum0 = 0;
	spi_readNum0 = 0;
	
	/*calculate baudrate speed*/
	buadrate_val = ((Clock_GetSystemFreq() / speed )  >> 1) * 10;
	
	buadrate_val = ((buadrate_val + 5) / 10) - 1; //round
		
	temp_reg = USTCR2;
	if(direction == SPI_RX_MODE)
		temp_reg |= RXE;	
	else if(direction == SPI_TX_MODE)
		temp_reg |= TXE;
	else
		temp_reg |= (RXE | TXE);
		
	temp_reg |= USTEN;
		
	USTST	|= SOFTRST;			// USART block reset
	USTCR1 |= ((0x3 << 6) | (first_bit << 2) | (cpha << 1) | (cpol << 0));  //SPI mode
	USTCR3 |= ((mode << 7) | (ss_mode << 4));
	USTBD = buadrate_val;
	USTCR2 = temp_reg;
	
}


/**
* @brief		Enables or disables the SPI peripheral.
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void USART_SPI_Enable(uint8_t enable)
{
	if(enable == TRUE)
	{
		USTCR2 |= USTEN;
	}
	else
	{
		USTCR2 &= ~USTEN;
	}
}


/**
* @brief		Transmit data through the SPI peripheral for polling mode.
* @param   send_data   This parameter contains the data to transmit. 
* @param   count   This parameter contains the number of data. 
* @return		None
*/
void USART_SPI_SendDataWithPolling(uint8_t *send_data, uint8_t count)
{
	uint8_t i;
	
	for(i = 0; i < count ; i++)
	{
		USTCR2 |= (DRIE);
		while(!(USTST & (DRE)));
			USTDR = send_data[i];
	}

}

/**
* @brief		Transmit data through the SPI peripheral for interrupt mode.
* @param   send_data   This parameter contains the data to transmit. 
* @param   count   This parameter contains the number of data. 
* @return		None
*/
void USART_SPI_SendDataWithInterrupt(uint8_t *send_data, uint8_t count)
{
	uint8_t i;
	
	USART_SPI_ConfigureInterrupt(SPI_TX_COMPLETE_INT, FALSE);
		
	spi_writeNum0 = count;
	for(i = 0; i < count ; i++)
	{
	spi_writeBuff0[i] = send_data[i];
	}
	
	USART_SPI_ConfigureInterrupt(SPI_TX_COMPLETE_INT, TRUE);
	
	USTCR2 |= (DRIE);	
}

/**
* @brief		Receive data through the SPI peripheral for polling mode.
* @param   receive_data   This parameter contains the data to receive. 
* @param   count   This parameter contains the number of data. 
* @return		None
*/
void USART_SPI_ReceiveDataWithPolling(uint8_t *receive_data, uint8_t count)
{
	uint8_t i;
	
	for(i = 0; i < count; i++)
	{
		USTCR2 |= (DRIE);
		while(!(USTST & (DRE)));
			USTDR = 0xFF; //Dummy data
		while(( USTST & RXC)!= RXC);		// Wait Data in
			receive_data[i] = USTDR;
	}
}

/**
* @brief		Receive data through the SPI peripheral for interrupt mode.
* @param   receive_data   This parameter contains the data to receive. 
* @param   count   This parameter contains the number of data. 
* @return		received data
*/
void USART_SPI_ReceiveDataWithInterrupt(uint8_t *receive_data, uint8_t count)
{
	
	USART_SPI_ConfigureInterrupt(SPI_RX_COMPLETE_INT, FALSE);

	spi_readNum0 = count;
	spi_readBuff0 = (uint8_t*)receive_data; 
	
	USART_SPI_ConfigureInterrupt(SPI_RX_COMPLETE_INT, TRUE);

}

/**
* @brief		Configure the enable or disable SPI interrupt.
* @param   flag   This parameter contains the selection of this function. 
*
*					- SPI_RX_COMPLETE_INT = 0
*					- SPI_TX_COMPLETE_INT = 1
* 					- SPI_DATA_EMPTY_INT = 2
* @param   enable   This parameter contains the enable of this function. 
*
*					- FLASE												= 0
*					- TRUE 												= 1
* @return		None
*/
void USART_SPI_ConfigureInterrupt(uint8_t flag, uint8_t enable)
{
	uint8_t temp_reg, temp_reg2;
	
	if(flag == SPI_TX_COMPLETE_INT)
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
		else if(flag == SPI_RX_COMPLETE_INT)
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
		else if(flag == SPI_DATA_EMPTY_INT)
		{
			temp_reg = USTCR2;
			temp_reg &= ~(0x1 << (5 + flag));
			temp_reg |= (enable << (5 + flag));
			
			USTCR2 = temp_reg;
		}
}

/**
* @brief		Get the USART interrupt status.
* @return		Value of status(interrupt flag)
*/
uint8_t USART_SPI_GetInterruptStatus()
{
	return USTST;
}


//==============================================================================
// interrupt routines(USI0)
//==============================================================================

void USART_RXInt_Handler() interrupt USART_RX_VECT
{
    uint8_t temp;

    // 마스터 모드
    if(USTCR3 & MASTER)
    {
        if(USTST & DOR)
        {
            // 데이터 오버런 처리
            NOP;
            temp = USTDR;
        }

        // ★ 핵심 변경: for 루프 제거, 1바이트씩 상태머신으로 처리
        if(spi_readNum0 > 0)
        {
            temp_spi_readBuff0[spi_readNum0 - 1] = USTDR;  // 수신 데이터 저장
            spi_readNum0--;

            if(spi_readNum0 > 0)
            {
                // 다음 바이트 요청: 더미 전송 → 다음 RXC 인터럽트 발생
                while(!(USTST & (DRE)));
                USTDR = 0xFF;
            }
            else
            {
                // 모든 바이트 수신 완료
                USART_SPI_ConfigureInterrupt(SPI_RX_COMPLETE_INT, FALSE);
            }
        }
    }
    // 슬레이브 모드
    else
    {
        // ★ 핵심 변경: RXC 대기 제거 (이미 RXC 인터럽트로 진입했으므로 불필요)
        if(USTST & DOR)
        {
            NOP;
            temp = USTDR;
        }

        temp = USTDR;
        if(temp == 0xFF)
        {
            while(!(USTST & (DRE)));
            USTDR = 0x05;
        }
    }
}


void USART_TXInt_Handler() interrupt USART_TX_VECT
{
    // ★ 핵심 변경: for 루프 제거, 1바이트씩 처리
    if(spi_writeNum0 > 0)
    {
        spi_writeNum0--;
        while(!(USTST & (DRE)));
        USTDR = spi_writeBuff0[spi_writeNum0];

        if(spi_writeNum0 == 0)
        {
            // 모든 바이트 전송 완료
            USART_SPI_ConfigureInterrupt(SPI_TX_COMPLETE_INT, FALSE);
            USTCR2 &= ~(DRIE);
        }
    }
}


/* --------------------------------- End Of File ------------------------------ */
