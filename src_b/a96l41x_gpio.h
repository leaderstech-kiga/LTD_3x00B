/**
*   @file       a96l41x_gpio.h
*   @brief      A96L41x gpio peripheral function 
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
#ifndef __GPIO_H_
#define __GPIO_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "typedef.h"


/*******************************************************************************
* Public Macro
*******************************************************************************/
#define PUSH_PULL		0
#define OPEN_DRAIN		1

#define NO_PULL		0
#define PULL_UP		1

#define PORT0           0
#define PORT1           1
#define PORT2           2

#define PIN0            0
#define PIN1            1
#define PIN2            2
#define PIN3            3
#define PIN4            4
#define PIN5            5
#define PIN6            6
#define PIN7            7

#define EXTINT_CH0      0
#define EXTINT_CH1      1
#define EXTINT_CH2      2
#define EXTINT_CH3      3
#define EXTINT_CH4      4
#define EXTINT_CH5      5
#define EXTINT_CH6      6
#define EXTINT_CH7      7
#define EXTINT_CH8      8
#define EXTINT_CH9      9
#define EXTINT_CH10     10
#define EXTINT_CH11     11
#define EXTINT_CH12     12

/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum  interrupt_edge{ 
        NONE = 0,
		RISING_EDGE,
		FALLING_EDGE,
		BOTH_EDGE,
};

/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void Port_Initial(void);
void Port_SetOutputpin(uint8_t port, uint8_t pin, uint8_t mode);
void Port_SetInputpin(uint8_t port, uint8_t pin, uint8_t mode);
void Port_SetAlterFunctionpin(uint8_t port, uint8_t pin, uint8_t val);
void Port_SetOutputHighpin(uint8_t port, uint8_t pin);
void Port_SetOutputLowpin(uint8_t port, uint8_t pin);
void Port_SetOutputTogglepin(uint8_t port, uint8_t pin);
uint8_t Port_GetInputpinValue(uint8_t port, uint8_t pin);
void Port_EnableInterrupt(uint8_t channel);
void Port_DisableInterrupt(uint8_t channel);
void Port_ConfigureInterrupt(uint8_t channel, uint8_t edge);
uint8_t Port_GetInterruptStatus(uint8_t channel);
void Port_ClearInterruptStatus(uint8_t channel);

#endif   /* End of __GPIO_H_ */
/* --------------------------------- End Of File ------------------------------ */