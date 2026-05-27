/**
*   @file       a96l41x_pwr.h
*   @brief      A96L41x power mode function 
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
#ifndef __PWR_H_
#define __PWR_H_
/*******************************************************************************
* Included File
*******************************************************************************/
#include "A96L41x.h"
#include "typedef.h"
#include "Intrins.h"

/*******************************************************************************
* Public Macro
*******************************************************************************/
#define PWR_IDLE_MODE		0x01
#define PWR_STOP_MODE  0x03

#define POR_FLAG								0x80
#define EXT_RESET_FLAG		0x40
#define WDT_RESET_FLAG	0x20
#define OCD_RESET_FLAG 	0x10
#define LVR_RESET_FLAG		0x08


/*******************************************************************************
* Public Typedef
*******************************************************************************/
enum  lvr_level{ 
	LVR_1_60		 	= 0,
	LVR_2_00,
	LVR_2_40,
	LVR_2_70,
};


/*******************************************************************************
* Exported Public Function
*******************************************************************************/
void PWR_EnterIdle(void);
void PWR_EnterStop(void);
uint8_t PWR_GetResetStatus(void);
void PWR_ClearResetStatus(uint8_t flag);

void PWR_ConfigureLVR(uint8_t select_level, uint8_t enable);

#endif  /* End of __PWR_H_ */
/* --------------------------------- End Of File ------------------------------ */