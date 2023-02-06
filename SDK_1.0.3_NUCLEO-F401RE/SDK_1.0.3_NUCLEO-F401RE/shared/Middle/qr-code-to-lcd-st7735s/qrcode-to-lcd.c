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
 * File name: qrcode-to-lcd.c
 *
 * Description: This code is used for
 *
 * Author: CuuNV
 *
 * Last Changed By:  $Author: CuuNV $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $Jan 28, 2023
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <qrcode.h>
#include <timer.h>
#include <Ucglib.h>
#include <qrcode-to-lcd.h>
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
static ucg_t ucg;
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
void lcdInit(void);

uint8_t checkDataLength(uint8_t byDataLength, uint8_t byEccLevel, uint8_t byVersion);
void generateQRCode(char *pByData,uint8_t byDataLength);
void clearLCD(void);
void printText(char *str,uint8_t x,uint8_t y,const ucg_fntpgm_uint8_t *font);
/******************************************************************************/
/**
 * @func   lcdInit
 * @brief  Initializes LCD
 * @param  None
 * @retval None
 */
void lcdInit(void)
{
	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&ucg);
	ucg_SetFont(&ucg, ucg_font_ncenR12_hr);
	ucg_SetColor(&ucg, 0, 255, 255, 255);//khoi tao mau chu la mau trang
	ucg_SetColor(&ucg, 1, 0, 0, 0);// khoi tao mau backgroud la mau den
	ucg_SetRotate180(&ucg);
	ucg_DrawString(&ucg, 25, 60, 0, "Hello Lumi");
}
/**
 * @func   generateQRCode
 * @brief  Print qr_code in LCD
 * @param  data
 * @retval None
 */
void generateQRCode(char *pByData,uint8_t byDataLength)
{
	  // Create the QR code
	    QRCode qrcode;

	    const uint8_t byEcc = ECC_LEVEL;

	    const uint8_t byVersion = VERSION_OF_QR;

	    uint8_t pbyQrcodeData[qrcode_getBufferSize(byVersion)];

	    checkDataLength(byDataLength, byEcc, byVersion);

	    qrcode_initText(&qrcode, pbyQrcodeData, byVersion, byEcc, pByData);

	    const uint8_t byXyScale = SCALE_ONE_PIXEL;

	    const uint8_t byWidth = WIDTH_LCD;
	    const uint8_t byHeight = HEIGHT_LCD;
	    uint8_t byXmax = byWidth/2;
	    uint8_t byYmax = byHeight/2;
	    uint8_t byOffset = (byXyScale*qrcode.size);
	    uint8_t byX1 = byXmax - (byOffset/2);
	    uint8_t byY1 = byYmax - (byOffset/2);


	    uint8_t byPx1 = byX1;
	    uint8_t byPy1 = byY1;

	    uint8_t byPx2 = byPx1;
	    uint8_t byPy2 = byPy1;


	    //Clear LCD
	    ucg_ClearScreen(&ucg);

	    // Top quiet zone
	    ucg_SetColor(&ucg, 0, 255, 255, 255);
	    for (uint8_t y = 0; y < qrcode.size; y++) {
	        for(uint8_t x = 0; x < qrcode.size; x++) {
	            bool mod = qrcode_getModule(&qrcode,x, y);
	            byPx1 = byX1 + x * byXyScale;
	            byPy1 = byX1 + y * byXyScale;
	            byPx2 = byPx1 + byXyScale;
	            byPy2 = byPy1 + byXyScale;
	            if(mod){
	            	for(uint8_t i =byPx1;i<=byPx2;i++)
	            	{
	            		for(uint8_t k = byPy1; k<= byPy2;k++)
	            		{
	            			ucg_DrawPixel(&ucg, i, k);
	            		}
	            	}

	            }
	        }
	    }

}
/**
 * @func   clearLCD
 * @brief  Clear LCD
 * @param  None
 * @retval None
 */
void clearLCD(void)
{
	ucg_ClearScreen(&ucg);
}
/**
 * @func   printText
 * @brief  In Ra chuoi Str ra man hinh
 * @param
 * @retval None
 */
void printText(char *str,uint8_t x,uint8_t y,const ucg_fntpgm_uint8_t *font)
{
	uint8_t byDelta = 0;
	ucg_SetFont(&ucg, font);
	for(uint8_t i = 0;i<strlen(str);i++)
	{
		byDelta = ucg_DrawGlyph(&ucg, x, y , 0, (uint8_t)str[i]);

		x += byDelta;
	}
}
/**
 * @func   checkDataLength
 * @brief  Kiem tra do dai chuoi ma QR-Code co the chua
 * @param
 * @retval None
 */
uint8_t checkDataLength(uint8_t byDataLength, uint8_t byEccLevel, uint8_t byVersion)
{
	switch(byVersion)
	{
	case 1:
		if(byEccLevel == ECC_LOW)
		{
			if(byDataLength>19)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(byDataLength>16)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(byDataLength>13)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(byDataLength>9)
				return -1;
		}
		break;
	case 2:
		if(byEccLevel == ECC_LOW)
		{
			if(byDataLength>34)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(byDataLength>28)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(byDataLength>22)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(byDataLength>16)
				return -1;
		}
		break;
	case 3:
		if(byEccLevel == ECC_LOW)
		{
			if(byDataLength>55)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(byDataLength>44)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(byDataLength>34)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(byDataLength>26)
				return -1;
		}
		break;
	case 4:
		if(byEccLevel == ECC_LOW)
		{
			if(byDataLength>80)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(byDataLength>64)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(byDataLength>48)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(byDataLength>36)
				return -1;
		}
		break;
	case 5:
		if(byEccLevel == ECC_LOW)
		{
			if(byDataLength>108)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(byDataLength>86)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(byDataLength>62)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(byDataLength>46)
				return -1;
		}
		break;
	case 6:
		if(byEccLevel == ECC_LOW)
		{
			if(byDataLength>136)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(byDataLength>108)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(byDataLength>76)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(byDataLength>60)
				return -1;
		}
		break;
	default:
		break;
	}
	return 0;
}
