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
#include <qrcode.h>
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
uint8_t checkDataLength(char *pByData, uint8_t byEccLevel, uint8_t byVersion);
void generateQRCode(char *pByData);
/******************************************************************************/
void lcdInit(void)
{
	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&ucg);
	ucg_SetColor(&ucg, 0, 255, 255, 255);//khoi tao mau chu la mau trang
	ucg_SetColor(&ucg, 1, 0, 0, 0);// khoi tao mau backgroud la mau den
	ucg_SetRotate180(&ucg);
}
/**
 * @func   generateQRCode
 * @brief  Print qr_code in LCD
 * @param  data
 * @retval None
 */
void generateQRCode(char *pByData)
{
	  // Create the QR code
	    QRCode qrcode;

	    const uint8_t byEcc = ECC_LEVEL;

	    const uint8_t byVersion = VERSION_OF_QR;

	    uint8_t pbyQrcodeData[qrcode_getBufferSize(byVersion)];

	    qrcode_initText(&qrcode,
	                    pbyQrcodeData,
	                    byVersion,
						byEcc,
						pByData);
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

	    ucg_SetColor(&ucg, 0, 255, 255, 255);
	    //ucg_ClearScreen(&ucg);
	    // Top quiet zone
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
uint8_t checkDataLength(char *pByData, uint8_t byEccLevel, uint8_t byVersion)
{
	switch(byVersion)
	{
	case 1:
		if(byEccLevel == ECC_LOW)
		{
			if(strlen(pByData)>19)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(strlen(pByData)>16)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(strlen(pByData)>13)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(strlen(pByData)>9)
				return -1;
		}
		break;
	case 2:
		if(byEccLevel == ECC_LOW)
		{
			if(strlen(pByData)>34)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(strlen(pByData)>28)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(strlen(pByData)>22)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(strlen(pByData)>16)
				return -1;
		}
		break;
	case 3:
		if(byEccLevel == ECC_LOW)
		{
			if(strlen(pByData)>55)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(strlen(pByData)>44)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(strlen(pByData)>34)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(strlen(pByData)>26)
				return -1;
		}
		break;
	case 4:
		if(byEccLevel == ECC_LOW)
		{
			if(strlen(pByData)>80)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(strlen(pByData)>64)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(strlen(pByData)>48)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(strlen(pByData)>36)
				return -1;
		}
		break;
	case 5:
		if(byEccLevel == ECC_LOW)
		{
			if(strlen(pByData)>108)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(strlen(pByData)>86)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(strlen(pByData)>62)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(strlen(pByData)>46)
				return -1;
		}
		break;
	case 6:
		if(byEccLevel == ECC_LOW)
		{
			if(strlen(pByData)>136)
				return -1;
		}else if(byEccLevel == ECC_MEDIUM)
		{
			if(strlen(pByData)>108)
				return -1;
		}else if(byEccLevel == ECC_QUARTILE)
		{
			if(strlen(pByData)>76)
				return -1;
		}else if(byEccLevel == ECC_HIGH)
		{
			if(strlen(pByData)>60)
				return -1;
		}
		break;
	default:
		break;
	}
	return 0;
}
