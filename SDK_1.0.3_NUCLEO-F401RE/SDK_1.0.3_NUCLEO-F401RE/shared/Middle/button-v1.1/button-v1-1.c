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
 * File name: button-v1-1.c
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
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stm32f401re_rcc.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_exti.h>
#include <stm32f401re_syscfg.h>
#include <timer.h>
#include <misc.h>
#include <button-v1-1.h>
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
static BUTTON_t buttonB3,buttonB2,buttonB4;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
void buttonInit(void);

static uint32_t dwCalculatorTime(uint32_t dwTimeInit,uint32_t dwTimeCurrent);

static void GetEventButton(BUTTON_t *button);

ValueKey_e processEventButton(void);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

void buttonInit(void)
{
	GPIO_InitTypeDef gpioInitStruct;
	EXTI_InitTypeDef extiInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	//---------------------------------------------------------------------------B3
	//1. Bat xung clock
	RCC_AHB1PeriphClockCmd(BUTTON_B3_GPIO_RCC, ENABLE);

	//2. Config GPIO
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
	gpioInitStruct.GPIO_Pin = BUTTON_B3_PIN;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(BUTTON_B3_PORT, &gpioInitStruct);
	//3. Config ngat
		//3.1 Bat xung cap cho ngat
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(BUTTON_B3_EXTI_PORTSOURCE, BUTTON_B3_EXTI_PinSource);

		//3.2 Cau hinh EXTI
	extiInitStruct.EXTI_Line = BUTTON_B3_EXTI_LINE;
	extiInitStruct.EXTI_LineCmd = ENABLE;
	extiInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

	EXTI_Init(&extiInitStruct);

		//3.3 Cau hinh trinh phuc vu ngat
	nvicInitStruct.NVIC_IRQChannel = BUTTON_B3_NVIC_IRQCHANNEL;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&nvicInitStruct);
	//---------------------------------------------------------------------------B2
	//1. Bat xung clock
	RCC_AHB1PeriphClockCmd(BUTTON_B2_GPIO_RCC, ENABLE);

	//2. Config GPIO
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
	gpioInitStruct.GPIO_Pin = BUTTON_B2_PIN;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(BUTTON_B2_PORT, &gpioInitStruct);
	//3. Config ngat
		//3.1 Bat xung cap cho ngat
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(BUTTON_B2_EXTI_PORTSOURCE, BUTTON_B2_EXTI_PinSource);

		//3.2 Cau hinh EXTI
	extiInitStruct.EXTI_Line = BUTTON_B2_EXTI_LINE;
	extiInitStruct.EXTI_LineCmd = ENABLE;
	extiInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

	EXTI_Init(&extiInitStruct);

		//3.3 Cau hinh trinh phuc vu ngat
	nvicInitStruct.NVIC_IRQChannel = BUTTON_B2_NVIC_IRQCHANNEL;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&nvicInitStruct);
	//---------------------------------------------------------------------------B4
	//1. Bat xung clock
	RCC_AHB1PeriphClockCmd(BUTTON_B4_GPIO_RCC, ENABLE);

	//2. Config GPIO
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
	gpioInitStruct.GPIO_Pin = BUTTON_B4_PIN;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(BUTTON_B4_PORT, &gpioInitStruct);
	//3. Config ngat
		//3.1 Bat xung cap cho ngat
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(BUTTON_B4_EXTI_PORTSOURCE, BUTTON_B4_EXTI_PinSource);

		//3.2 Cau hinh EXTI
	extiInitStruct.EXTI_Line = BUTTON_B4_EXTI_LINE;
	extiInitStruct.EXTI_LineCmd = ENABLE;
	extiInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

	EXTI_Init(&extiInitStruct);

		//3.3 Cau hinh trinh phuc vu ngat
	nvicInitStruct.NVIC_IRQChannel = BUTTON_B4_NVIC_IRQCHANNEL;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&nvicInitStruct);
}
static uint32_t dwCalculatorTime(uint32_t dwTimeInit,uint32_t dwTimeCurrent)
{
	if(dwTimeCurrent >= dwTimeInit)
	{
		return (dwTimeCurrent - dwTimeInit);
	}else
	{
		return (0xFFFFFFFF + dwTimeCurrent - dwTimeInit);
	}
}
ValueKey_e processEventButton(void)
{
	ValueKey_e valueKeyTemp = NOKEY;

	GetEventButton(&buttonB3);
	GetEventButton(&buttonB2);
	GetEventButton(&buttonB4);
	switch(buttonB3.buttonEven)
	{
	case EVENT_OF_BUTTON_HOLD_500MS:

		buttonB3.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_1_TIMES:
		valueKeyTemp = UP;
		buttonB3.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_2_TIMES:
		buttonB3.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_3_TIMES:
		buttonB3.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	default:
		buttonB3.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	}
	switch(buttonB2.buttonEven)
	{
	case EVENT_OF_BUTTON_HOLD_500MS:
		valueKeyTemp = RETURN;
		buttonB2.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_1_TIMES:
		valueKeyTemp = SELECT;
		buttonB2.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_2_TIMES:

		buttonB2.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_3_TIMES:

		buttonB2.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	default:
		buttonB2.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	}
	switch(buttonB4.buttonEven)
	{
	case EVENT_OF_BUTTON_HOLD_500MS:
		buttonB4.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_1_TIMES:
		valueKeyTemp = DOWN;
		buttonB4.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_2_TIMES:
		buttonB4.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	case EVENT_OF_BUTTON_PRESS_3_TIMES:
		buttonB4.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	default:
		buttonB4.buttonEven = EVENT_OF_BUTTON_NOCLICK;
		break;
	}
	return valueKeyTemp;
}

static void GetEventButton(BUTTON_t *button)
{

static uint8_t byTemp;
uint32_t dwTimeNow,dwTimeTemp;
dwTimeNow = GetMilSecTick();
if(button->byState == BUTTON_EDGE_RISING)
	{
		//Neu nut nhan o trang thai dang nhan
		dwTimeTemp = dwCalculatorTime(button->dwTimeInit, dwTimeNow);
		if(dwTimeTemp >= BUTTON_LONG_PRESS_TIME)
		{
			//Nhan giu qua 500ms thi update event cua nut nhan la nhan giu
			button->buttonEven = EVENT_OF_BUTTON_HOLD_500MS;
			byTemp = 1;

		}
	}
if(button->byState == BUTTON_EDGE_FALLING)
{
	//Neu nut nhan duoc nha ra
	if((byTemp == 0))
	{
		dwTimeTemp = dwCalculatorTime(button->dwTimeCurrent, dwTimeNow);
		if(dwTimeTemp >= BW2PRESS_TIME)
		{
			button->byPressCntEnd = button->byPressCnt;
			button->dwTimeInit = 0;
			button->dwTimeCurrent = 0;
			button->byPressCnt = 0;
			switch(button->byPressCntEnd)
			{
			case 1:
			{
				button->buttonEven = EVENT_OF_BUTTON_PRESS_1_TIMES;
				button->byPressCntEnd = 0;
				button->byPressCnt = 0;
				break;
			}
			case 2:
			{
				button->buttonEven = EVENT_OF_BUTTON_PRESS_2_TIMES;
				button->byPressCntEnd = 0;
				button->byPressCnt = 0;
				break;
			}
			case 3:
			{
				button->buttonEven = EVENT_OF_BUTTON_PRESS_3_TIMES;
				button->byPressCntEnd = 0;
				button->byPressCnt = 0;
				break;
			}
			default:
				button->byPressCntEnd = 0;
				button->byPressCnt = 0;
				break;
			}
			button->byState = Null;
		}
	}else
	{	button->buttonEven = EVENT_OF_BUTTON_RELEASED;
		button->byPressCntEnd = 0;
		button->byPressCnt = 0;
		byTemp = 0;
		button->byState = Null;
	}
}
}


void EXTI4_IRQHandler(void)
{
	//BUTTON_B3
	static uint8_t byFlagPress = 0,byFlagOut = 0;
	//Khi nut nhan duoc nhan
	if(GPIO_ReadInputDataBit(BUTTON_B3_PORT, BUTTON_B3_PIN)==0)
		{
			buttonB3.dwTimeInit = GetMilSecTick();
			buttonB3.byState = BUTTON_EDGE_RISING;
			byFlagOut++;
		}
	//khi nut nhan duoc nha ra
	if(GPIO_ReadInputDataBit(BUTTON_B3_PORT, BUTTON_B3_PIN)!=0)
		{
			buttonB3.dwTimeCurrent = GetMilSecTick();
			buttonB3.byState = BUTTON_EDGE_FALLING;
			buttonB3.dwTimePress = dwCalculatorTime(buttonB3.dwTimeInit, buttonB3.dwTimeCurrent);
			byFlagPress = 1;
		}
	//Thuc hien du 1 chu trinh nhan xong nha cua nut bam
	if(byFlagPress == 1)
		{
			buttonB3.dwTimePress = dwCalculatorTime(buttonB3.dwTimeInit, buttonB3.dwTimeCurrent);
			if((buttonB3.dwTimePress >=BUTTON_NORMAL_PRESS_TIME)&&(buttonB3.dwTimePress < BUTTON_LONG_PRESS_TIME))
				{
					buttonB3.byPressCnt ++;
				}
			byFlagPress = 0;
			buttonB3.byStatus = BUTTON_STATE_WAITPRESS;
		}
	//Khi nut nhan duoc nhan lien tiep nhau
	if(byFlagOut == 3)
	{
		buttonB3.dwTimeOut = dwCalculatorTime(buttonB3.dwTimeCurrent, buttonB3.dwTimeInit);
			if(buttonB3.dwTimeOut >= BW2PRESS_TIME)
				{
					buttonB3.byPressCntEnd = buttonB3.byPressCnt;
					buttonB3.byPressCnt = 0;
				}
			byFlagOut = 0;
		}
	EXTI_ClearITPendingBit(BUTTON_B3_EXTI_LINE);
}
void EXTI3_IRQHandler(void)
{
	//BUTTON_B2
	static uint8_t byFlagPress = 0,byFlagOut = 0;
	//Khi nut nhan duoc nhan
	if(GPIO_ReadInputDataBit(BUTTON_B2_PORT, BUTTON_B2_PIN)==0)
		{
			buttonB2.dwTimeInit = GetMilSecTick();
			buttonB2.byState = BUTTON_EDGE_RISING;
			byFlagOut++;
		}
	//khi nut nhan duoc nha ra
	if(GPIO_ReadInputDataBit(BUTTON_B2_PORT, BUTTON_B2_PIN)!=0)
		{
			buttonB2.dwTimeCurrent = GetMilSecTick();
			buttonB2.byState = BUTTON_EDGE_FALLING;
			buttonB2.dwTimePress = dwCalculatorTime(buttonB2.dwTimeInit, buttonB2.dwTimeCurrent);
			byFlagPress = 1;
		}
	//Thuc hien du 1 chu trinh nhan xong nha cua nut bam
	if(byFlagPress == 1)
		{
			buttonB2.dwTimePress = dwCalculatorTime(buttonB2.dwTimeInit, buttonB2.dwTimeCurrent);
			if((buttonB2.dwTimePress >=BUTTON_NORMAL_PRESS_TIME)&&(buttonB2.dwTimePress < BUTTON_LONG_PRESS_TIME))
				{
					buttonB2.byPressCnt ++;
				}
			byFlagPress = 0;
			buttonB2.byStatus = BUTTON_STATE_WAITPRESS;
		}
	//Khi nut nhan duoc nhan lien tiep nhau
	if(byFlagOut == 3)
	{
		buttonB2.dwTimeOut = dwCalculatorTime(buttonB2.dwTimeCurrent, buttonB2.dwTimeInit);
			if(buttonB2.dwTimeOut >= BW2PRESS_TIME)
				{
					buttonB2.byPressCntEnd = buttonB2.byPressCnt;
					buttonB2.byPressCnt = 0;
				}
			byFlagOut = 0;
		}
	EXTI_ClearITPendingBit(BUTTON_B2_EXTI_LINE);
}
void EXTI0_IRQHandler(void)
{
	//BUTTON_B4
	static uint8_t byFlagPress = 0,byFlagOut = 0;
	//Khi nut nhan duoc nhan
	if(GPIO_ReadInputDataBit(BUTTON_B4_PORT, BUTTON_B4_PIN)==0)
		{
			buttonB4.dwTimeInit = GetMilSecTick();
			buttonB4.byState = BUTTON_EDGE_RISING;
			byFlagOut++;
		}
	//khi nut nhan duoc nha ra
	if(GPIO_ReadInputDataBit(BUTTON_B4_PORT, BUTTON_B4_PIN)!=0)
		{
			buttonB4.dwTimeCurrent = GetMilSecTick();
			buttonB4.byState = BUTTON_EDGE_FALLING;
			buttonB4.dwTimePress = dwCalculatorTime(buttonB4.dwTimeInit, buttonB4.dwTimeCurrent);
			byFlagPress = 1;
		}
	//Thuc hien du 1 chu trinh nhan xong nha cua nut bam
	if(byFlagPress == 1)
		{
			buttonB4.dwTimePress = dwCalculatorTime(buttonB4.dwTimeInit, buttonB4.dwTimeCurrent);
			if((buttonB4.dwTimePress >=BUTTON_NORMAL_PRESS_TIME)&&(buttonB4.dwTimePress < BUTTON_LONG_PRESS_TIME))
				{
					buttonB4.byPressCnt ++;
				}
			byFlagPress = 0;
			buttonB4.byStatus = BUTTON_STATE_WAITPRESS;
		}
	//Khi nut nhan duoc nhan lien tiep nhau
	if(byFlagOut == 3)
	{
		buttonB4.dwTimeOut = dwCalculatorTime(buttonB4.dwTimeCurrent, buttonB4.dwTimeInit);
			if(buttonB4.dwTimeOut >= BW2PRESS_TIME)
				{
					buttonB4.byPressCntEnd = buttonB4.byPressCnt;
					buttonB4.byPressCnt = 0;
				}
			byFlagOut = 0;
		}
	EXTI_ClearITPendingBit(BUTTON_B4_EXTI_LINE);
}
/******************************************************************************/
