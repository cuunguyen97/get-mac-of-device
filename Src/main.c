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
 * Author: CuuNV
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define USART6_BAUDRATE			115200
#define USART6_PIN_RX			GPIO_Pin_12
#define USART6_PINSOURCE_RX		GPIO_PinSource12
#define USART6_PORT				GPIOA
#define USART6_GPIO_RCC			RCC_AHB1Periph_GPIOA
#define USART6_RCC				RCC_APB2Periph_USART6
#define USART6_AF				GPIO_AF_USART6
//define other
#define RX_MAX_INDEX_BYTE						256
#define LENGTH_OF_MAC							8
#define LENGTH_OF_VERSION						3
#define LENGTH_OF_DEVICE_TYPE					1
#define LENGTH_OF_PID							2
#define CMD_ID_INFORMATION_FEEDBACK				0xFF
#define BYTE_START_1							0x4C
#define BYTE_START_2							0x4D

//define of device type

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
typedef enum {
	RX_STATE_START_1_BYTE,
	RX_STATE_START_2_BYTE,
	RX_STATE_DATA_BYTES,
	RX_STATE_CXOR_BYTE
}RxState_e;
typedef enum {
	UART_STATE_IDLE,
	UART_STATE_DATA_RECEIVED,
	UART_STATE_ACK_RECEIVED,
	UART_STATE_NACK_RECEIVED,
	UART_STATE_ERROR,
	UART_STATE_RX_TIMEOUT
}UsartState_e;

typedef enum {
	PROTOCOL_TYPE_ZIGBEE     = 0x00,
	PROTOCOL_TYPE_BLUETOOTH	 = 0x01,
	PROTOCOL_TYPE_ZWAVE		 = 0x02
}ProtocolType_e;
typedef enum {
	SWITCH_DEVICE			= 0x01,
	SOCKETS_DEVICE			= 0x02,
	CURTAIN_DEVICE			= 0x03,
	DIMMER_DEVICE			= 0x04,
	FAN_DEVICE				= 0x05,
	IR_CONTROLLER_DEVICE	= 0x06,
	PIR_SENSOR				= 0x07,
	LIGHT_SENSOR			= 0x08,
	TEMP_SENSOR				= 0x09,
	HUMI_SENSOR				= 0x0A
}DeviceType_e;
typedef enum {
	UN_PROVISION			= 0x00,
	PROVISIONING			= 0x01,
	PROVISIONED				= 0x02
}ProvisionState_e;
typedef struct {
	uint8_t 			byCmdId;
	uint8_t				protocolType;
	uint8_t				deviceType;
	uint8_t				byEndpointCnt;
	ProvisionState_e	provisonState;
	uint8_t				pbyMAC[8];
	uint8_t				pbyVersion[3];
	uint8_t				pbyPID[2];
}CmdData_t;

#define SIZE_BUFF_DATA_RX		256
static uint8_t g_pBuffDataRx[SIZE_BUFF_DATA_RX] = {0};
static buffqueue_t g_pUartRxQueue = {0};

#define RX_MAX_INDEX_BYTE	256
static RxState_e g_RxState;
static uint8_t g_pbyRxDataByte[RX_MAX_INDEX_BYTE];
static uint8_t g_byRxCheckXor;
static uint8_t g_byRxIndexByte;
static uint8_t g_byRxNumByte;

static uint8_t g_byEnpointCntLast = 0;
static char g_pstrVersionBluetooth[LENGTH_OF_VERSION*2+1] = {0};
static char g_pstrVersionZigBee[LENGTH_OF_VERSION*2+1] = {0};
static char g_pstrVersionZwave[LENGTH_OF_VERSION*2+1] = {0};
static char g_pstrMACLast[LENGTH_OF_MAC*2+1] = {0};

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void appInitCommon(void);
static void serialUartInit(void);
static void usartInit(void);
void printMACLcd(char *pTextMAC,uint8_t x,uint8_t y,const ucg_fntpgm_uint8_t *font);
static void hexToAscii(char *pByDataOutPut,uint8_t *pByDataInPut,uint8_t byDataLength);
static uint8_t PollRxBuff(void);
static void processUartReceiveCommandInformationFeedBack(CmdData_t *pCmd);
static void procUartCmd(void *arg);
static void processSerialUartReceiver(void);
/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/


int main(void)
{
	appInitCommon();

    /* Loop forever */
	while(1)
	{
		processTimerScheduler();
		processSerialUartReceiver();
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
	TimerInit();
	lcdInit();
	serialUartInit();
	g_RxState = RX_STATE_START_1_BYTE;




}
/**
 * @func   processUartReceiveCommandInformationFeedBack
 * @brief  Xu ly ban tin feedback cua thiet bi
 * @param  CmdData_t *pCmd
 * @retval None
 */

static void processUartReceiveCommandInformationFeedBack(CmdData_t *pCmd)
{

	//1. Khai bao chuoi chua thong tin in ra QR
	char byDataPrint[5+(LENGTH_OF_MAC+LENGTH_OF_VERSION*2 +LENGTH_OF_PID \
			+ LENGTH_OF_DEVICE_TYPE)*2] = {0};

	//2. Chuoi chua thong tin hien tai
	char pstrMAC[LENGTH_OF_MAC * 2+1] = {0};
	char pstrDeviceType[LENGTH_OF_DEVICE_TYPE * 2 +1] = {0};
	char pstrPID[LENGTH_OF_PID * 2 +1] = {0};

	//3. Xoa du lieu cu
	memset(byDataPrint,0,sizeof(byDataPrint));
	//4. Chuyen doi du lieu tu dang Hex sang ma ASCII
	hexToAscii( pstrMAC,pCmd->pbyMAC, LENGTH_OF_MAC);

	hexToAscii(pstrDeviceType, &(pCmd->deviceType), LENGTH_OF_DEVICE_TYPE);

	hexToAscii( pstrPID,pCmd->pbyPID, LENGTH_OF_PID);

	//5. Quet 2 lan de lay version cua zigbee va bluetooth
	static uint8_t byStatusTemp = 0;

	if(pCmd->protocolType == PROTOCOL_TYPE_ZIGBEE)
	{
		byStatusTemp ++;
		memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
		hexToAscii( g_pstrVersionZigBee,pCmd->pbyVersion, LENGTH_OF_VERSION);
	}
	else if(pCmd->protocolType == PROTOCOL_TYPE_BLUETOOTH)
	{
		byStatusTemp ++;
		memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
		hexToAscii( g_pstrVersionBluetooth,pCmd->pbyVersion, LENGTH_OF_VERSION);
	}else
	{
		hexToAscii( g_pstrVersionZwave,pCmd->pbyVersion, LENGTH_OF_VERSION);
	}
	//6. Luu gia tri endPointCnt truoc khi so sanh
	if(byStatusTemp%2 !=0)
	{
		g_byEnpointCntLast = pCmd->byEndpointCnt;

	}


	//7. So sanh MAC , Ghep thong tin vao 1 chuoi, va in ma Qr_Code ra man hinh
		//Gia tri dem so lan quet lai ban tin khi thay doi thiet bi co endpoint khac
	static uint8_t byCountTemp = 0;

	if(strcmp(pstrMAC,g_pstrMACLast)!=0 &&(byStatusTemp>=2))
	{
		// In ra gia tri khi quet du thong tin

		strcpy(g_pstrMACLast,pstrMAC);

		if(g_byEnpointCntLast == pCmd->byEndpointCnt)
		{

			byCountTemp = 0;

			strcat(byDataPrint,pstrMAC);

			strcat(byDataPrint,",");

			strcat(byDataPrint,pstrDeviceType);

			strcat(byDataPrint,",");

			strcat(byDataPrint,pstrPID);

			strcat(byDataPrint,",");

			strcat(byDataPrint,g_pstrVersionZigBee);

			strcat(byDataPrint,",");

			strcat(byDataPrint,g_pstrVersionBluetooth);


			generateQRCode(byDataPrint,strlen(byDataPrint));

			printMACLcd(pstrMAC,8,120,ucg_font_5x7_mf);

		}else
		{
			//7. Neu gia tri endPointCnt khac thi cho quet lai lan nua

				if(byCountTemp == 0)
				{
					memset(g_pstrMACLast,0,sizeof(g_pstrMACLast));
					g_byEnpointCntLast =0;
					byCountTemp ++;
				}else if(byCountTemp>=1)
				{
					char byText[] = "Firmware ERROR!!";
					clearLCD();
					printText(byText,20,50,ucg_font_6x10_mf);
					//In ra MAC loi.
					printMACLcd(pstrMAC,8,70,ucg_font_5x7_mf);
					memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
					memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
					memset(pstrPID,0,sizeof(pstrPID));
					memset(pstrDeviceType,0,sizeof(pstrDeviceType));
					byCountTemp = 0;
				}



		}
	}
	if(byStatusTemp >=2)
	{
		byStatusTemp =0;
	}


}
/**
 * @func   procUartCmd
 * @brief  Xu ly truong CMD_ID cua thiet bi
 * @param  void *arg
 * @retval None
 */
static void procUartCmd(void *arg)
{

	CmdData_t *CmdData = (CmdData_t*)arg;
	switch(CmdData->byCmdId)
	{
	case CMD_ID_INFORMATION_FEEDBACK:
		processUartReceiveCommandInformationFeedBack(CmdData);
		break;
	default:
			break;
	}

}
/**
 * @func   processSerialUartReceiver
 * @brief  Xu ly ban tin nhan duoc tu UART
 * @param  None
 * @retval None
 */
static void processSerialUartReceiver(void)
{
	static uint8_t uartState;
	uartState = PollRxBuff();

	if(uartState != UART_STATE_IDLE)
	{
		switch(uartState)
		{
		case UART_STATE_DATA_RECEIVED:
		{
			procUartCmd((void*)&g_pbyRxDataByte[1]);
			g_RxState = RX_STATE_START_1_BYTE;
			break;
		}
		case UART_STATE_ERROR:
			uartState = UART_STATE_IDLE;
			g_RxState = RX_STATE_START_1_BYTE;
			break;
		default:
			break;
		}
	}
}
/**
 * @func   PollRxBuff
 * @brief  Phan tich ban tin UART nhan duoc, loai bo cac ban tin loi.
 * @param  void *arg
 * @retval None
 */
static uint8_t PollRxBuff(void)
{
	//1. Khai bao bien chua dÃ†Â° lieu lay tu hang doi
	uint8_t RxDataTemp;
	uint8_t UartState = (uint8_t)UART_STATE_IDLE;

	//2. Tao vong lap den khi het du lieu trong hang doi
	while((bufNumItems(&g_pUartRxQueue) !=0) && (UartState == UART_STATE_IDLE)){
		//2.0 Lay 1 phan tu trong hang doi
		bufDeDat(&g_pUartRxQueue, &RxDataTemp);
		switch(g_RxState)
		{
		//2.1 Uart state- Start byte: 0xb1
		case RX_STATE_START_1_BYTE:
		{
			//2.1.1 TH: byte du lieu lay ra la byte start 0xb1
			if(RxDataTemp == BYTE_START_1)
			{
				/*
				 * Neu la byte start thi se khoi tao
				 * 1. Bien check xor = 0xff
				 * 2. Khoi tao so byte data doc duoc
				 * 3. Khoi tao g_RxState sang trang thai bat dau doc du lieu
				 */
				g_RxState = RX_STATE_START_2_BYTE;
			}
			//2.1.4 TH: con lai
			else
			{
				UartState = UART_STATE_ERROR;
			}
			break;
		}
		case RX_STATE_START_2_BYTE:
				{
					//2.1.1 TH: byte du lieu lay ra la byte start 0xb1
					if(RxDataTemp == BYTE_START_2)
					{
						/*
						 * Neu la byte start thi se khoi tao
						 * 1. Bien check xor = 0xff
						 * 2. Khoi tao so byte data doc duoc
						 * 3. Khoi tao g_RxState sang trang thai bat dau doc du lieu
						 */
						g_byRxCheckXor = 0;
						g_byRxIndexByte = 0;
						g_RxState = RX_STATE_DATA_BYTES;
					}
					//2.1.4 TH: con lai
					else
					{
						g_RxState = RX_STATE_START_1_BYTE;
						UartState = UART_STATE_ERROR;
					}
					break;
				}
		//2.2 Uart state- Data byte
		case RX_STATE_DATA_BYTES:
		{
			/*
			 * Trong qua trinh doc du lieu:
			 * 1. Luu du lieu vao  mang de xu ly tiep
			 * 2. Tinh toan check Xor de xac nhan toan ven cua data da nhan duoc
			 * 3. Neu so byte doc duoc lon hon gia tri ma mang co the luu thi reset va bao loi
			 */
			if(g_byRxIndexByte < RX_MAX_INDEX_BYTE)
			{
				g_pbyRxDataByte[g_byRxIndexByte] = RxDataTemp;
				if(g_byRxIndexByte >0)
				{
					g_byRxCheckXor ^= RxDataTemp;
				}
				// Neu la byte cuoi cung thi se chuyen sang check xor
				// Sau byte start la byte chua do dai du lieu
				if(++g_byRxIndexByte == g_pbyRxDataByte[0])
				{
					g_RxState = RX_STATE_CXOR_BYTE;
				}
			}else
			{
				g_RxState = RX_STATE_START_1_BYTE;
				UartState = UART_STATE_ERROR;
			}
			break;
		}
		//2.3 Uart state- XOR byte
		case RX_STATE_CXOR_BYTE:
			/*
			 * Neu byte check xor gui toi bang gia tri tinh duoc thi data duoc chap nhan
			 * Nguoc lai thi bao loi
			 */
			//USART_SendData(USARTx, RxDataTemp);
			//USART_SendData(USARTx, g_byRxCheckXor);
			if(RxDataTemp == g_byRxCheckXor) {
				UartState = UART_STATE_DATA_RECEIVED;
				return UartState;
			} else {
				UartState = UART_STATE_ERROR;
			}

			break;
		//2.1 Default
		default:
			g_RxState = RX_STATE_START_1_BYTE;
			break;
		}

	}
	return UartState;
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
	bufInit(g_pBuffDataRx, &g_pUartRxQueue, sizeof(g_pBuffDataRx[0]), SIZE_BUFF_DATA_RX);

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

static void hexToAscii(char *pByDataOutPut,uint8_t *pByDataInPut,uint8_t byDataLength)
{
	uint8_t byMsb,byLsb;

	static char str[2]={0};
	for(uint8_t i = 0; i<byDataLength; i++)
	{
		byMsb = (pByDataInPut[i]>>4);
		byLsb = pByDataInPut[i] & 0x0F;
		if((byMsb>=0)&&(byMsb<=9))
		{
			byMsb += 48;
		}else
		{
			byMsb += 55;
		}
		if((byLsb>=0)&&(byLsb<=9))
		{
			byLsb += 48;
		}else
		{
			byLsb += 55;
		}
		str[0] = (char)byMsb;
		str[1] = (char)byLsb;
		strcat(pByDataOutPut,str);
	}
}
/**
 * @func   printMACLcd
 * @brief  In ra man hinh MAC cua thiet bi theo form XX:XX:XX:XX
 * @param
 * @retval None
 */
void printMACLcd(char *pTextMAC,uint8_t x,uint8_t y,const ucg_fntpgm_uint8_t *font)
{

	char strTemp[24];

	char strTemp2[] = ":";

	memset(strTemp,0,sizeof(strTemp));

	uint8_t j = 0;
	uint8_t i = 0;
	for(i = 0; i<strlen(pTextMAC); i++)
	{
			if((i%2 == 0)&&(i !=0))
			{
				j++;
				strcat(strTemp,strTemp2);
			}
			strTemp[j] = pTextMAC[i];
			j++;
	}
	printText(strTemp,x,y,font);

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
		g_byRxNumByte++;
		bufEnDat(&g_pUartRxQueue,&byData);
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
	}
}
