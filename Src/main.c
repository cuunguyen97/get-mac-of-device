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
 * File name: main.c
 *
 * Description: This code is used for tranning Lumi IOT member. It is the code form statandard.
 *
 * Author: PhuongNP
 *
 * Last Changed By:  $Author: CuuNV $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Jan 27, 2023
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdint.h>
#include <stm32f401re_rcc.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_usart.h>
#include <timer.h>
#include <qrcode-to-lcd.h>
#include <buff.h>
#include <misc.h>
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

//define of USART6
#define USART6_BAUDRATE			57600
#define USART6_PIN_RX			GPIO_Pin_12
#define USART6_PINSOURCE_RX		GPIO_PinSource12
#define USART6_PORT				GPIOA
#define USART6_GPIO_RCC			RCC_AHB1Periph_GPIOA
#define USART6_RCC				RCC_APB2Periph_USART6
#define USART6_AF				GPIO_AF_USART6
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

#define SIZE_BUFF_DATA_RX		256
static uint8_t pBuffDataRx[SIZE_BUFF_DATA_RX] = {0};
static buffqueue_t pUartRxQueue = {0};
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void appInitCommon(void);
static void serialUartInit(void);
static void usartInit(void);
void generateQRCode(char *pByData);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/


int main(void)
{
	uint8_t byRxData[8];

	appInitCommon();

    /* Loop forever */
	while(1)
	{
		processTimerScheduler();
		if(bufNumItems(&pUartRxQueue)>=8)
		{
			while(bufNumItems(&pUartRxQueue) !=0)
					{
						for(int i = 0; i < 8;i++)
						{
							bufDeDat(&pUartRxQueue, &byRxData[i]);

						}
					}
			generateQRCode((char *)byRxData);
		}
	}
}
/**
 * @func   appInitCommon
 * @brief  Initializes functions
 * @param  None
 * @retval None
 */

static void appInitCommon(void)
{
	SystemCoreClockUpdate();
	serialUartInit();
	lcdInit();
	TimerInit();

}

/**
 * @func   serialUartInit
 * @brief  Usart & Buffer initialization
 * @param  None
 * @retval None
 */
static void serialUartInit(void)
{
	//1. Khoi tao bo dem chua du lieu gui ve
	bufInit(pBuffDataRx, &pUartRxQueue, sizeof(pBuffDataRx[0]), SIZE_BUFF_DATA_RX);

	//2. Khoi tao ket noi USART2
	usartInit();
}

/**
 * @func   usart6Init
 * @brief  Usart initialization
 * @param  None
 * @retval None
 */
static void usartInit(void)
{
	/*
		 * USART: USART6
		 * RX: PA12
		 */
		GPIO_InitTypeDef	GPIO_InitStruct;
		USART_InitTypeDef	USART_InitStruct;
		NVIC_InitTypeDef	NVIC_InitStruct;

		//1. Configuare GPIO & AF
		RCC_AHB1PeriphClockCmd(USART6_GPIO_RCC, ENABLE);

		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

		GPIO_InitStruct.GPIO_Pin = USART6_PIN_RX;
		GPIO_Init(USART6_PORT, &GPIO_InitStruct);

		GPIO_PinAFConfig(USART6_PORT, USART6_PINSOURCE_RX, USART6_AF);
		//2. Configuare USART6: Bus APB2
		RCC_APB2PeriphClockCmd(USART6_RCC, ENABLE);

		USART_InitStruct.USART_BaudRate = USART6_BAUDRATE;
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStruct.USART_Mode = USART_Mode_Rx;
		USART_InitStruct.USART_Parity = USART_Parity_No;
		USART_InitStruct.USART_StopBits = USART_StopBits_1;
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;

		USART_Init(USART6,&USART_InitStruct);

		//3. Enable USART6 Receive intterupt
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

		//4. Configuare NVIC
		NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

		NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
		NVIC_Init(&NVIC_InitStruct);

		//5. ENABLE USART6
		USART_Cmd(USART6, ENABLE);
}

/**
 * @func   USART6x_IRQHandler
 * @brief
 * @param  None
 * @retval None
 */
void USART6x_IRQHandler(void)
{
	uint8_t byData;
	if(USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{
		byData = USART_ReceiveData(USART6);
		bufEnDat(&pUartRxQueue,&byData);
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
	}
}
