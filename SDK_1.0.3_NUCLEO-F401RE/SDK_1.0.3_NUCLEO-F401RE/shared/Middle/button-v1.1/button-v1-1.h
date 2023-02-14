/*******************************************************************************
 *				 _ _                                             _ _
				|   |                                           (_ _)
				|   |        _ _     _ _   _ _ _ _ _ _ _ _ _ _   _ _
				|   |       |   |   |   | |    _ _     _ _    | |   |
				|   |       |   |   |   | |   |   |   |   |   | |   |
				|   |       |   |   |   | |   |   |   |   |   | |   |
				|   |_ _ _  |   |_ _|   | |   |   |   |   |   | |   |
				|_ _ _ _ _| |_ _ _ _ _ _| |_ _|   |_ _|   |_ _| |_ _|
								(C)2023 Lumi
 * Copyright (c) 2023
 * Lumi, JSC.
 * All Rights Reserved
 *
 * File name: button-v1-1.h
 *
 * Description: This code is used for tranning Lumi IOT member. It is the code form statandard.
 *
 * Author: CuuNV
 *
 * Last Changed By:  $Author: CuuNV $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Feb 9, 2023
 *
 * Code sample:
 ******************************************************************************/
#ifndef SDK_1_0_3_NUCLEO_F401RE_SHARED_MIDDLE_BUTTON_V1_1_BUTTON_V1_1_H_
#define SDK_1_0_3_NUCLEO_F401RE_SHARED_MIDDLE_BUTTON_V1_1_BUTTON_V1_1_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stm32f401re_rcc.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_exti.h>
#include <stm32f401re_syscfg.h>
#include <misc.h>
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
//define of button B3:PA4
#define BUTTON_B3_PIN				GPIO_Pin_4
#define BUTTON_B3_PINSOURCE			GPIO_PinSource4
#define BUTTON_B3_PORT				GPIOA
#define BUTTON_B3_GPIO_RCC			RCC_AHB1Periph_GPIOA
#define BUTTON_B3_EXTI_PinSource	EXTI_PinSource4
#define BUTTON_B3_EXTI_PORTSOURCE   EXTI_PortSourceGPIOA
#define BUTTON_B3_EXTI_LINE			EXTI_Line4
#define BUTTON_B3_NVIC_IRQCHANNEL 	EXTI4_IRQn
//define of button B2:PB3
#define BUTTON_B2_PIN				GPIO_Pin_3
#define BUTTON_B2_PINSOURCE			GPIO_PinSource3
#define BUTTON_B2_PORT				GPIOB
#define BUTTON_B2_GPIO_RCC			RCC_AHB1Periph_GPIOB
#define BUTTON_B2_EXTI_PinSource	EXTI_PinSource3
#define BUTTON_B2_EXTI_PORTSOURCE   EXTI_PortSourceGPIOB
#define BUTTON_B2_EXTI_LINE			EXTI_Line3
#define BUTTON_B2_NVIC_IRQCHANNEL 	EXTI3_IRQn
//define of button B4:PB0
#define BUTTON_B4_PIN				GPIO_Pin_0
#define BUTTON_B4_PINSOURCE			GPIO_PinSource0
#define BUTTON_B4_PORT				GPIOB
#define BUTTON_B4_GPIO_RCC			RCC_AHB1Periph_GPIOB
#define BUTTON_B4_EXTI_PinSource	EXTI_PinSource0
#define BUTTON_B4_EXTI_PORTSOURCE   EXTI_PortSourceGPIOB
#define BUTTON_B4_EXTI_LINE			EXTI_Line0
#define BUTTON_B4_NVIC_IRQCHANNEL 	EXTI0_IRQn

#define BUTTON_DEBOUNCE_TIME      		5u
#define BUTTON_NORMAL_PRESS_TIME  		100u
#define BUTTON_LONG_PRESS_TIME    		500u
#define BW2PRESS_TIME		            400u

/* Button byStatus */
#define BUTTON_EDGE_RISING              	1
#define BUTTON_EDGE_FALLING             	2
#define Null								0
/* Button states */
#define BUTTON_STATE_START        	0
#define BUTTON_STATE_DEBOUNCE     	1
#define BUTTON_STATE_PRESSED      	2
#define BUTTON_STATE_WAITPRESS		3
#define BUTTON_STATE_WAITRELEASE  	4
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
// enum of button
typedef enum
{
	EVENT_OF_BUTTON_NOCLICK,
	EVENT_OF_BUTTON_PRESS_1_TIMES,
	EVENT_OF_BUTTON_PRESS_2_TIMES,
	EVENT_OF_BUTTON_PRESS_3_TIMES,
	EVENT_OF_BUTTON_HOLD_500MS,
	EVENT_OF_BUTTON_RELEASED,

}EventButton_e;
typedef enum
{
	NOKEY,
	SELECT,
	UP,
	DOWN,
	RETURN
}ValueKey_e;

//Struct of button
typedef struct
{
	EventButton_e buttonEven ;
	uint8_t byPressCnt ;												/*!< Time when button was pressed */
	uint8_t byPressCntEnd;

	uint32_t dwTimeInit;
	uint32_t dwTimeCurrent;
	uint32_t dwTimePress;
	uint32_t dwTimeOut;
	uint8_t byState;														/*!< Current button state */
	uint8_t byStatus;
}BUTTON_t;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void buttonInit(void);

ValueKey_e processEventButton(void);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/

#endif /* SDK_1_0_3_NUCLEO_F401RE_SHARED_MIDDLE_BUTTON_V1_1_BUTTON_V1_1_H_ */
