/**
*   @file       delay.h
*   @brief      Polling delay header file
*   @author     V&V Team, ABOV Semiconductor Co., Ltd.
*   @version    V1.00
*   @date       29. MAY. 2020
*
* Copyright(C) 2020, ABOV Semiconductor
* All rights reserved.
*
********************************************************************************
* DISCLAIMER 
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, ABOV SEMICONDUCTOR DISCLAIMS ALL LIABILITIES FROM ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ABOV96_CORE_DELAY_H_
#define __ABOV96_CORE_DELAY_H_
/* Includes ------------------------------------------------------------------*/

/* Private Pre-processor Definition & Macro ----------------------------------*/
/* Private Typedef -----------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
/* Public Variable -----------------------------------------------------------*/
/* Public Function -----------------------------------------------------------*/

//------------------------------------------------------------------------------

volatile void NOP_4us_Delay(void);
volatile void NOP_20us_Delay(unsigned int delay);

#endif  /* End of __ABOV96_CORE_DELAY_H */
/* --------------------------------- End Of File ------------------------------ */
