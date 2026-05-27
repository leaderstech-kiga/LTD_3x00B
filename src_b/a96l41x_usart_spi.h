/**
*   @file       a96l41x_usart_spi.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_SPI_H_
#define __USART_SPI_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "a96l41x_clock.h"
#include "typedef.h"
#include "Intrins.h"

/*******************************************************************************
* Public Macro
*******************************************************************************/
#define SPI_SLAVE_MODE			0
#define SPI_MASTER_MODE		1

#define SPI_LSB					0
#define SPI_MSB					1

#define SPI_CPOL_LOW			0
#define SPI_CPOL_HIGH			1

#define SPI_CPHA_1EDGE			0
#define SPI_CPHA_2EDGE			1

#define SPI_TX_RX_MODE			0
#define SPI_RX_MODE				1
#define SPI_TX_MODE				2

#define SPI_SS_HW_DISABLE		0
#define SPI_SS_HW_ENABLE		1

#define SPI_MAX_BUFFER_SIZE	8
/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum spi_interrupt_flag{
	SPI_RX_COMPLETE_INT = 0,
	SPI_TX_COMPLETE_INT,
	SPI_DATA_EMPTY_INT,    	
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void USART_SPI_Initial(uint8_t mode, uint32_t speed, uint8_t first_bit, uint8_t cpol, uint8_t cpha, uint8_t direction, uint8_t ss_mode);
void USART_SPI_Enable(uint8_t enable);

void USART_SPI_SendDataWithPolling(uint8_t *send_data, uint8_t count);
void USART_SPI_SendDataWithInterrupt(uint8_t *send_data, uint8_t count);
void USART_SPI_ReceiveDataWithPolling(uint8_t *receive_data, uint8_t count);
void USART_SPI_ReceiveDataWithInterrupt(uint8_t *receive_data, uint8_t count);

void USART_SPI_ConfigureInterrupt(uint8_t flag, uint8_t enable);
uint8_t USART_SPI_GetInterruptStatus(void);

#endif  /* End of __USART_SPI_H_ */
/* --------------------------------- End Of File ------------------------------ */