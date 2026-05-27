/**
*   @file       a96l41x_i2c.h
*   @brief      A96L41x i2c peripheral function 
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
#ifndef __I2C_H_
#define __I2C_H_
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
#define I2C_DEVICE_ADDRESS			0xA0
#define I2C_SLAVE_OWN_ADDR		0xA0
#define I2C_SPEED											10000

#define I2C_MAX_BUFFER_SIZE		10
#define I2C_MAX_CHANNEL					1

#define I2C_CH0														0
//#define I2C_CH1														1

#define I2C_ACK_DISABLE						0
#define I2C_ACK_ENABLE							1

#define I2C_WRITE_MODE						0
#define I2C_READ_MODE							1

#define I2C_IDLE													0
#define I2C_BUSY												1
#define I2C_MATCH_CMD							2

#define I2C_TRUE												1
#define I2C_FALSE												0
/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum  i2c_interrupt_status{ 
	GCALL	 	= 1 << 7,
	TEND			= 1 << 6,
	STOPD 	= 1 << 5,
	SSEL 			= 1 << 4,
	MLOST 	= 1 << 3,
	BUSY 		= 1 << 2,
	TMODE 	= 1 << 1,
	RXACK 	= 1 << 0,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void I2C_Initial(uint8_t ch, uint32_t speed, uint8_t addr, uint8_t ack);
void I2C_Enable(uint8_t ch, uint8_t enable);
void I2C_ConfigureGerneralCall(uint8_t ch, uint8_t enable);
void I2C_ConfigureAcknowledge(uint8_t ch, uint8_t enable);
void I2C_SoftwareReset(uint8_t ch, uint8_t enable);

void I2C_MasterTransferData(uint8_t ch, uint8_t dev_addr, uint8_t *write_data, uint8_t write_len, uint8_t *read_data, uint8_t read_len);
void I2C_ConfigureInterrupt(uint8_t ch, uint8_t enable);
uint8_t I2C_GetInterruptStatus(uint8_t ch);
void I2C_InterruptHandler(uint8_t ch);
#endif  /* End of __USI_I2C_H_ */
/* --------------------------------- End Of File ------------------------------ */