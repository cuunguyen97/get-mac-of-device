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
#include <button-v1-1.h>
#include <menu.h>
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
#define CMD_ID_MCU_INFORMATION					0xAB
#define BYTE_START_1							0x4C
#define BYTE_START_2							0x4D

//define of device type

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
// enum of system
typedef enum{
	STATE_APP_STARTUP,
	STATE_APP_IDLE,
	STATE_APP_RESET
}StateApp_e;

// enum of uart
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

// enum of LUMI_SWITCH
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
typedef struct {
    uint8_t  cmd_id;
    uint8_t  msg_from;
    uint8_t  version[3];
    uint8_t  type;
    uint8_t  endpoint_cnt;
    uint8_t  led_intensity;
    uint8_t  full_port;
    uint8_t  relay_type;
    uint8_t  cz_type[4];
    uint8_t  c_Xor;
}McuInfor_t;

#define SIZE_BUFF_DATA_RX		256
static uint8_t g_pBuffDataRx[SIZE_BUFF_DATA_RX] = {0};
static buffqueue_t g_pUartRxQueue = {0};

#define RX_MAX_INDEX_BYTE	256
static RxState_e g_eRxState;
static uint8_t g_pbyRxDataByte[RX_MAX_INDEX_BYTE];
static uint8_t g_byRxCheckXor;
static uint8_t g_byRxIndexByte;
static uint8_t g_byRxNumByte;

static uint8_t g_byEnpointCntMCU = 0;
static uint8_t g_byEnpointCntBLE = 0;
static uint8_t g_byEnpointCntZigBee = 0;
static char g_pstrMACZigbee[LENGTH_OF_MAC * 2+1] = {0};
static char g_pstrMACBle[LENGTH_OF_MAC * 2+1] = {0};
static char g_pstrMACLast[LENGTH_OF_MAC * 2+1] = {0};
static char g_pstrVersionBluetooth[LENGTH_OF_VERSION*2+1] = {0};
static char g_pstrVersionZigBee[LENGTH_OF_VERSION*2+1] = {0};
static char byText[20]="";
static TestSwMode_e modeTest = NONE;

StateApp_e eCurrentState = STATE_APP_STARTUP;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void serialUartInit(void);

static void usartInit(void);

static void appInitCommon(void);

static void setStateApp(StateApp_e state);

static StateApp_e getStateApp(void);

static void startApp(void);

static void appStateManager(StateApp_e event);

void printMACLcd(char *pTextMAC,uint8_t x,uint8_t y,const ucg_fntpgm_uint8_t *font);

static void hexToAscii(char *pByDataOutPut,uint8_t *pByDataInPut,uint8_t byDataLength);

static uint8_t PollRxBuff(void);

static void processUartReceiveCommandInformationFeedBack(CmdData_t *pCmd);

static void processUartReceiveMCUInformation(McuInfor_t *pCmd);

static void procUartCmd(void *arg);

static void processSerialUartReceiver(void);

static void delayMs(uint32_t ms);

static uint32_t dwCalculatorTime(uint32_t dwTimeInit,uint32_t dwTimeCurrent);

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

		appStateManager(eCurrentState);
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
	buttonInit();
	g_eRxState = RX_STATE_START_1_BYTE;
	eCurrentState = STATE_APP_STARTUP;
	lcdClear();
	lcdPrintText("IOT", 42, 30, ucg_font_ncenR10_hf);
	lcdPrintText("Programming by", 6, 50, ucg_font_ncenR10_hf);
	lcdPrintText("Lumi Smarthome", 4, 75, ucg_font_ncenR10_hf);
}
/**
 * @func   setStateApp
 * @brief  Set state of application
 * @param  state: State of application
 * @retval None
 */

static void setStateApp(StateApp_e state)
{
	eCurrentState = state;
}

/**
 * @func   setStateApp
 * @brief  Get state of application
 * @param  None
 * @retval State of application
 */
static StateApp_e getStateApp(void)
{
	return eCurrentState;
}
/**
 * @func   appStateManager
 * @brief  Manager state application
 * @param  event: Su kien
 * @retval None
 */
static void appStateManager(StateApp_e event)
{
	switch(event)
	{
	case STATE_APP_STARTUP: //Su kien khi he thong bat dau duoc cap nguon
		do{
			modeTest = getModeTest();
		}
		while(modeTest == NONE);
		setStateApp(STATE_APP_IDLE);
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
		break;
	case STATE_APP_IDLE:

		if(processEventButton() == RETURN)
		{
			USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
			setStateApp(STATE_APP_RESET);
		}
		processSerialUartReceiver();
		break;
	case STATE_APP_RESET:
		memset(g_pstrMACLast,0,sizeof(g_pstrMACLast));
		setStateApp(STATE_APP_STARTUP);
		break;
	default:
		break;

	}
}
/**
 * @func   startApp
 * @brief  Ham khoi dong truoc khi chay chuong trinh chinh
 * @param  None
 * @retval None
 */
static void startApp(void)
{
	delayMs(2000);
	lcdClear();
	lcdPrintText("Choose mode",0,20,ucg_font_ncenR10_hf);
	lcdPrintText("PRESS B3:",0,40,ucg_font_ncenR10_hf);
	lcdPrintText(">1 time:Dual mode", 0, 60, ucg_font_ncenR10_hf);
	lcdPrintText(">2 time:ZB mode", 0, 80, ucg_font_ncenR10_hf);
	lcdPrintText(">3 time:BLE mode", 0, 100, ucg_font_ncenR10_hf);
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
static void delayMs(uint32_t ms)
{
	uint32_t dwTimeInit = GetMilSecTick();
	uint32_t dwTimeCurrent = 0;

	do{
		dwTimeCurrent = GetMilSecTick();
	}
	while(dwCalculatorTime(dwTimeInit, dwTimeCurrent)<ms);

}
static void processUartReceiveMCUInformation(McuInfor_t *pCmd)
{
	g_byEnpointCntMCU = pCmd->endpoint_cnt;
}
/**
 * @func   processUartReceiveCommandInformationFeedBack
 * @brief  Xu ly ban tin feedback cua thiet bi
 * @param  CmdData_t *pCmd
 * @retval None
 */

static void processUartReceiveCommandInformationFeedBack(CmdData_t *pCmd)
{
	//0.Get mode

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

	//Single mode
	//Dual mode
	if(pCmd->protocolType == PROTOCOL_TYPE_ZIGBEE)
	{
		byStatusTemp ++;
		memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
		hexToAscii( g_pstrVersionZigBee,pCmd->pbyVersion, LENGTH_OF_VERSION);

		memset(g_pstrMACZigbee,0,sizeof(g_pstrMACZigbee));
		strcpy(g_pstrMACZigbee,pstrMAC);
		g_byEnpointCntZigBee = pCmd->byEndpointCnt;
	}
	else if(pCmd->protocolType == PROTOCOL_TYPE_BLUETOOTH)
	{
		byStatusTemp ++;
		memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
		hexToAscii( g_pstrVersionBluetooth,pCmd->pbyVersion, LENGTH_OF_VERSION);

		memset(g_pstrMACBle,0,sizeof(g_pstrMACBle));
		strcpy(g_pstrMACBle,pstrMAC);
		g_byEnpointCntBLE = pCmd->byEndpointCnt;
	}

		//6.1 Reset buf khi mac thay doi
	static uint8_t byFlagOfBufReset = 0;
	uint8_t temp;
	if((strcmp(&g_pstrMACZigbee[4],&g_pstrMACLast[4])!=0)||(strcmp(&g_pstrMACBle[4],&g_pstrMACLast[4])!=0))
	{
		USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
		if(byFlagOfBufReset == 0)
		{
			while((bufNumItems(&g_pUartRxQueue) !=0))
			{

				bufDeDat(&g_pUartRxQueue, &temp);
			};
			g_byEnpointCntMCU = 0;
			memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
			memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
			byFlagOfBufReset = 1;

		}
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	}
	//7. So sanh MAC , Ghep thong tin vao 1 chuoi, va in ma Qr_Code ra man hinh
		//Gia tri dem so lan quet lai ban tin khi thay doi thiet bi co endpoint khac
	static uint8_t byCountTemp = 0;

	//7.1 Dual mode

	switch(modeTest)
			{
			case DUAL_MODE:
				if(strcmp(&g_pstrMACZigbee[4],&g_pstrMACLast[4])!=0 &&(byStatusTemp>=2))
				{
					// In ra gia tri khi quet du thong tin
					strcpy(g_pstrMACLast,pstrMAC);


					if((g_byEnpointCntMCU == g_byEnpointCntBLE)&&(g_byEnpointCntMCU == g_byEnpointCntZigBee))
					{
						byCountTemp = 0;

						g_byEnpointCntMCU = 0;

						g_byEnpointCntBLE = 0;

						g_byEnpointCntZigBee = 0;

						strcat(byDataPrint,g_pstrMACZigbee);

						strcat(byDataPrint,",");

						strcat(byDataPrint,pstrDeviceType);

						strcat(byDataPrint,",");

						strcat(byDataPrint,pstrPID);

						strcat(byDataPrint,",");

						strcat(byDataPrint,g_pstrVersionZigBee);

						strcat(byDataPrint,",");

						strcat(byDataPrint,g_pstrVersionBluetooth);

						generateQRCode(byDataPrint,strlen(byDataPrint));

						printMACLcd(g_pstrMACZigbee,8,120,ucg_font_5x7_mf);

						byFlagOfBufReset = 0;

					}else
					{
						//7. Neu gia tri endPointCnt khac thi cho quet lai lan nua

							if(byCountTemp == 0)
							{
								memset(g_pstrMACLast,0,sizeof(g_pstrMACLast));
								byStatusTemp =0;
								byCountTemp ++;
							}else if(byCountTemp>=1)
							{

								if((g_byEnpointCntMCU != g_byEnpointCntBLE))
								{
									lcdClearAxisY(20,128);
									lcdPrintText("Firmware BLE",20,30,ucg_font_6x10_mf);
									lcdPrintText("ERROR!!!",50,50,ucg_font_6x10_mf);
									//In ra MAC loi.
									lcdPrintText("ECntZigBee:",0,70,ucg_font_6x10_mf);
									memset(byText,0,sizeof(byText));
									hexToAscii( byText,&g_byEnpointCntZigBee, 1);
									lcdPrintText(byText,100,70,ucg_font_6x10_mf);

									lcdPrintText("ECntBLE:",0,90,ucg_font_6x10_mf);
									memset(byText,0,sizeof(byText));
									hexToAscii( byText,&g_byEnpointCntBLE, 1);
									lcdPrintText(byText,100,90,ucg_font_6x10_mf);

									printMACLcd(g_pstrMACZigbee,8,110,ucg_font_5x7_mf);
									memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
									memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
									memset(pstrPID,0,sizeof(pstrPID));
									memset(pstrDeviceType,0,sizeof(pstrDeviceType));
									byCountTemp = 0;
								}
								if(g_byEnpointCntMCU != g_byEnpointCntZigBee)
								{
									lcdClearAxisY(20,128);
									lcdPrintText("Firmware ZigBee",20,30,ucg_font_6x10_mf);
									lcdPrintText("ERROR!!!",50,50,ucg_font_6x10_mf);
									//In ra MAC loi.

									lcdPrintText("ECntZigBee:",0,70,ucg_font_6x10_mf);
									memset(byText,0,sizeof(byText));
									hexToAscii( byText,&g_byEnpointCntZigBee, 1);
									lcdPrintText(byText,100,70,ucg_font_6x10_mf);

									lcdPrintText("ECntBLE:",0,90,ucg_font_6x10_mf);
									memset(byText,0,sizeof(byText));
									hexToAscii( byText,&g_byEnpointCntBLE, 1);
									lcdPrintText(byText,100,90,ucg_font_6x10_mf);

									printMACLcd(g_pstrMACZigbee,8,110,ucg_font_5x7_mf);
									memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
									memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
									memset(pstrPID,0,sizeof(pstrPID));
									memset(pstrDeviceType,0,sizeof(pstrDeviceType));
									byCountTemp = 0;
								}

							}
					}
				}
				break;
			case ZIGBEE_MODE:
				if(strcmp(&g_pstrMACZigbee[4],&g_pstrMACLast[4])!=0 &&(byStatusTemp>=2))
				{
					// In ra gia tri khi quet du thong tin
					strcpy(g_pstrMACLast,pstrMAC);


					if(g_byEnpointCntMCU == g_byEnpointCntZigBee)
					{
						byCountTemp = 0;

						g_byEnpointCntMCU = 0;

						g_byEnpointCntZigBee = 0;

						strcat(byDataPrint,g_pstrMACZigbee);

						strcat(byDataPrint,",");

						strcat(byDataPrint,pstrDeviceType);

						strcat(byDataPrint,",");

						strcat(byDataPrint,pstrPID);

						strcat(byDataPrint,",");

						strcat(byDataPrint,g_pstrVersionZigBee);

						strcat(byDataPrint,",");

						strcat(byDataPrint,g_pstrVersionBluetooth);

						generateQRCode(byDataPrint,strlen(byDataPrint));

						printMACLcd(g_pstrMACZigbee,8,120,ucg_font_5x7_mf);

						byFlagOfBufReset = 0;

					}else
					{
						//7. Neu gia tri endPointCnt khac thi cho quet lai lan nua

							if(byCountTemp == 0)
							{
								memset(g_pstrMACLast,0,sizeof(g_pstrMACLast));
								byStatusTemp =0;
								byCountTemp ++;
							}else if(byCountTemp>=1)
							{
								char byText[] = "Firmware ERROR!!!";
								lcdClearAxisY(20,128);
								lcdPrintText(byText,20,50,ucg_font_6x10_mf);
								//In ra MAC loi.
								printMACLcd(g_pstrMACZigbee,8,70,ucg_font_5x7_mf);
								memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
								memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
								memset(pstrPID,0,sizeof(pstrPID));
								memset(pstrDeviceType,0,sizeof(pstrDeviceType));
								byCountTemp = 0;
							}
					}
				}
				break;
			case BLE_MODE:
				if(strcmp(&g_pstrMACBle[4],&g_pstrMACLast[4])!=0 &&(byStatusTemp>=2))
				{
					// In ra gia tri khi quet du thong tin
					strcpy(g_pstrMACLast,pstrMAC);


					if(g_byEnpointCntMCU == g_byEnpointCntBLE)
					{
						byCountTemp = 0;

						g_byEnpointCntMCU = 0;

						g_byEnpointCntBLE = 0;

						strcat(byDataPrint,g_pstrMACBle);

						strcat(byDataPrint,",");

						strcat(byDataPrint,pstrDeviceType);

						strcat(byDataPrint,",");

						strcat(byDataPrint,pstrPID);

						strcat(byDataPrint,",");

						strcat(byDataPrint,g_pstrVersionZigBee);

						strcat(byDataPrint,",");

						strcat(byDataPrint,g_pstrVersionBluetooth);

						generateQRCode(byDataPrint,strlen(byDataPrint));

						printMACLcd(g_pstrMACBle,8,120,ucg_font_5x7_mf);

						byFlagOfBufReset = 0;

					}else
					{
						//7. Neu gia tri endPointCnt khac thi cho quet lai lan nua

							if(byCountTemp == 0)
							{
								memset(g_pstrMACLast,0,sizeof(g_pstrMACLast));
								byStatusTemp =0;
								byCountTemp ++;
							}else if(byCountTemp>=1)
							{
								char byText[] = "Firmware ERROR!!!";
								lcdClearAxisY(20,128);
								lcdPrintText(byText,20,50,ucg_font_6x10_mf);
								//In ra MAC loi.
								printMACLcd(g_pstrMACBle,8,70,ucg_font_5x7_mf);
								memset(g_pstrVersionBluetooth,0,sizeof(g_pstrVersionBluetooth));
								memset(g_pstrVersionZigBee,0,sizeof(g_pstrVersionZigBee));
								memset(pstrPID,0,sizeof(pstrPID));
								memset(pstrDeviceType,0,sizeof(pstrDeviceType));
								byCountTemp = 0;
							}
					}
				}
				break;
				break;
			default:
				break;
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
	McuInfor_t * McuInfor = (McuInfor_t*)arg;
	switch(CmdData->byCmdId)
	{
	case CMD_ID_INFORMATION_FEEDBACK:
		processUartReceiveCommandInformationFeedBack(CmdData);
		break;
	case CMD_ID_MCU_INFORMATION:
		processUartReceiveMCUInformation(McuInfor);
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
			g_eRxState = RX_STATE_START_1_BYTE;
			break;
		}
		case UART_STATE_ERROR:
			uartState = UART_STATE_IDLE;
			g_eRxState = RX_STATE_START_1_BYTE;
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
		switch(g_eRxState)
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
				 * 3. Khoi tao g_eRxState sang trang thai bat dau doc du lieu
				 */
				g_eRxState = RX_STATE_START_2_BYTE;
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
						 * 3. Khoi tao g_eRxState sang trang thai bat dau doc du lieu
						 */
						g_byRxCheckXor = 0;
						g_byRxIndexByte = 0;
						g_eRxState = RX_STATE_DATA_BYTES;
					}
					//2.1.4 TH: con lai
					else
					{
						g_eRxState = RX_STATE_START_1_BYTE;
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
					g_eRxState = RX_STATE_CXOR_BYTE;
				}
			}else
			{
				g_eRxState = RX_STATE_START_1_BYTE;
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
			g_eRxState = RX_STATE_START_1_BYTE;
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
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

		NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
		NVIC_Init(&NVIC_InitStruct);

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
	lcdPrintText(strTemp,x,y,font);

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
	if((USART_GetITStatus(USART6, USART_IT_RXNE) == SET))
	{
		byData = USART_ReceiveData(USART6);
		g_byRxNumByte++;
		bufEnDat(&g_pUartRxQueue,&byData);
	}
	USART_ClearITPendingBit(USART6, USART_IT_RXNE);
}


