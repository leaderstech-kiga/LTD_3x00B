/**
*   @file       a96l41x_usart.h
*   @brief      A96L41x usart peripheral function 
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
#ifndef __USART_H_
#define __USART_H_
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
#define	USART_QUEUE_SIZE					8

#define USART_STOP_1BIT					0
#define USART_STOP_2BIT					1

#define USART_PARITY_NO					0
#define USART_PARITY_EVEN			2
#define USART_PARITY_ODD				3

#define USART_TX_RX_MODE			0
#define USART_RX_MODE						1
#define USART_TX_MODE						2

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum  usart_data_length{ 
	USART_DATA_5BIT = 0,
	USART_DATA_6BIT = 1,
	USART_DATA_7BIT = 2,
	USART_DATA_8BIT = 3,
	USART_DATA_9BIT = 7,
};

enum usart_interrupt_flag{
	USART_RX_COMPLETE_INT = 0,
	USART_TX_COMPLETE_INT,
	USART_DATA_EMPTY_INT,    	
	USART_WAKEUP_INT,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void USART_Initial(uint32_t speed, uint8_t data_length, uint8_t stop_bits, uint8_t parity, uint8_t mode);
void USART_Clock_Initial(uint32_t clock_en, uint8_t master_en);
void USART_Enable(uint8_t enable);

void USART_SendDataWithPolling(uint8_t *send_data, uint8_t count);
void USART_SendDataWithInterrupt(uint8_t send_data);
void USART_ReceiveDataWithPolling(uint8_t *receive_data, uint8_t count);
uint8_t USART_ReceiveDataWithInterrupt(void);

void USART_ConfigureInterrupt(uint8_t flag, uint8_t enable);
uint8_t USART_GetInterruptStatus(void);

void USART_ConfigureWakeUp(uint8_t enable);
void USART_ClearWakeUpFlag(void);
void USART_ConfigureRTO(uint8_t enable);
uint8_t USART_GetRTOStatus(void);

void USART_CompensationBaudrate(uint16_t fpcg_val, uint8_t enable);

#endif  /* End of __USART_H_ */
/* --------------------------------- End Of File ------------------------------ */