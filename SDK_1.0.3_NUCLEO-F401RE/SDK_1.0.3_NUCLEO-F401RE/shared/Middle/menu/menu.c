
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
 * File name: menu.c
 *
 * Description:
 *
 * Author: CuuNV
 *
 * Last Changed By:  $Author: CuuNV $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Feb 10, 2023
 *
 * Code sample:
 ******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <qrcode-to-lcd.h>
#include <button-v1-1.h>
#include <menu.h>
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
static char pMenuMain[4][20] = {
		"MENU",
		"1.Dual mode",
		"2.ZB mode",
		"3.BLE mode"
};
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
TestSwMode_e getModeTest(void);

uint8_t getChooseRows(uint8_t byNumOfRows,\
				uint8_t bySizeOfRow,\
				char pStrInterface[][20],\
				const ucg_fntpgm_uint8_t *font);
void prinfPoint(uint8_t bySizeOfRows,\
		uint8_t byChooseRow,\
		const ucg_fntpgm_uint8_t *font,\
		ValueKey_e key);


/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
TestSwMode_e getModeTest(void)
{
	TestSwMode_e modeTemp;
	uint8_t byChooseCol = 0;
	while(byChooseCol == 0)
	{
		byChooseCol = getChooseRows(4, 20, pMenuMain, ucg_font_10x20_mr);
	}
	lcdPrintText(pMenuMain[byChooseCol], 0, 15, ucg_font_10x20_mr);
	switch(byChooseCol)
	{
	case 1:
		byChooseCol = 0;
		modeTemp = DUAL_MODE;
		break;
	case 2:
		byChooseCol = 0;
		modeTemp = ZIGBEE_MODE;
		break;
	case 3:
		byChooseCol = 0;
		modeTemp = BLE_MODE;
		break;
	default:
		break;
	}
	return modeTemp;
}
uint8_t getChooseRows(uint8_t byNumOfRows,\
				uint8_t bySizeOfRow,\
				char pStrInterface[][20],\
				const ucg_fntpgm_uint8_t *font)
{
	ValueKey_e key = NOKEY ;
	uint8_t byRow = 0;
	static uint8_t byRowTemp = 2;
	static uint8_t byFlag = 0;

	if(byFlag == 0)
	{
		lcdClear();

		for(uint8_t i = 0;i<byNumOfRows;i++)
		{
			//print interface
			lcdPrintText (pStrInterface[i],0, bySizeOfRow+bySizeOfRow*i,font);
			lcdPrintText("<", 110, bySizeOfRow*byRowTemp, font);
		}
		byFlag = 1;
	}



	while(key == NOKEY)
	{
		key = processEventButton();
	}
	switch(key)
	{
	case UP:
		byRowTemp--;
		if(byRowTemp <2)
		{
			byRowTemp = byNumOfRows;
			lcdPrintText(" ", 110, bySizeOfRow*2, font);
		}
		lcdPrintText(" ", 110, bySizeOfRow * (byRowTemp+1), font);
		lcdPrintText("<", 110, bySizeOfRow*byRowTemp, font);
		key = NOKEY;
		break;
	case DOWN:
		byRowTemp++;
		if(byRowTemp > byNumOfRows)
		{
			byRowTemp = 2;
			lcdPrintText(" ", 110, bySizeOfRow * byNumOfRows, font);
		}
		lcdPrintText(" ", 110, bySizeOfRow * (byRowTemp-1), font);
		lcdPrintText("<", 110, byRowTemp*bySizeOfRow, font);
		prinfPoint(bySizeOfRow, byRowTemp, font, key);
		key = NOKEY;
		break;
	case SELECT:
		lcdClear();
		key = NOKEY;
		byFlag = 0;
		byRow = byRowTemp-1;
		break;
	default:
		break;
	}
	return byRow;
}
void prinfPoint(uint8_t bySizeOfRows,\
		uint8_t byChooseRow,\
		const ucg_fntpgm_uint8_t *font,\
		ValueKey_e key)
{

	if(key == UP)
	{

	}else if(key == DOWN)
	{
		lcdPrintText(" ", 110, bySizeOfRows * (byChooseRow-1), font);
		lcdPrintText("<", 110, byChooseRow*bySizeOfRows, font);

	}

}
/******************************************************************************/
