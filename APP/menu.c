/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/menu.c 
  * @author  MCD Application Team

  * @brief   This file provides the software which contains the main menu routine.
  *          The main menu gives the options of:
  *             - downloading a new binary file, 
  *             - uploading internal flash memory,
  *             - executing the binary file already loaded 
  *             - configuring the write protection of the Flash sectors where the 
  *               user loads his binary file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup STM32F4xx_IAP_Main
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f4xx.h"
#include "main.h"
#include "iap.h"
#include "menu.h"
#include "printf.h"
#include "gd32f427v_usart.h"
#include "ymodem.h"
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction JumpToApplication;
uint32_t JumpAddress;
uint32_t FlashProtection = 0;
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
  uint8_t number[11] = {0};
  uint32_t size = 0;
  COM_StatusTypeDef result;

  Serial_PutString((uint8_t *)"Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  result = Ymodem_Receive( &size );
  if (result == COM_OK)
  {
    Serial_PutString((uint8_t *)"\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
    Serial_PutString(aFileName);
    Int2Str(number, size);
    Serial_PutString((uint8_t *)"\n\r Size: ");
    Serial_PutString(number);
    Serial_PutString((uint8_t *)" Bytes\r\n");
    Serial_PutString((uint8_t *)"-------------------\n");
  }
  else if (result == COM_LIMIT)
  {
    Serial_PutString((uint8_t *)"\n\n\rThe image size is higher than the allowed space memory!\n\r");
  }
  else if (result == COM_DATA)
  {
    Serial_PutString((uint8_t *)"\n\n\rVerification failed!\n\r");
  }
  else if (result == COM_ABORT)
  {
    Serial_PutString((uint8_t *)"\r\n\nAborted by user.\n\r");
  }
  else
  {
    Serial_PutString((uint8_t *)"\n\rFailed to receive the file!\n\r");
  }
}

/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Main_Menu(void)
{
  uint8_t key = 0;

  Serial_PutString((uint8_t* )"\r\n======================================================================");
  Serial_PutString((uint8_t* )"\r\n=              (C) COPYRIGHT 2016 STMicroelectronics                 =");
  Serial_PutString((uint8_t* )"\r\n=                                                                    =");
  Serial_PutString((uint8_t* )"\r\n=          STM32F4xx In-Application Programming Application          =");
  Serial_PutString((uint8_t* )"\r\n=                                                                    =");
  Serial_PutString((uint8_t* )"\r\n=                       By MCD Application Team                      =");
  Serial_PutString((uint8_t* )"\r\n======================================================================");
  Serial_PutString((uint8_t* )"\r\n\r\n");

  while (1)
  {

    /* Test if any sector of Flash memory where user application will be loaded is write protected */
    FlashProtection = app_flash_write_protection_get();
    
    Serial_PutString((uint8_t* )"\r\n=================== Main Menu ============================\r\n\n");
    Serial_PutString((uint8_t* )"  Download image to the internal Flash ----------------- 1\r\n\n");
    Serial_PutString((uint8_t* )"  Upload image from the internal Flash ----------------- 2\r\n\n");
    Serial_PutString((uint8_t* )"  Execute the loaded application ----------------------- 3\r\n\n");

    if(FlashProtection != FLASHIF_PROTECTION_NONE)
    {
      Serial_PutString((uint8_t* )"  Disable the write protection ------------------------- 4\r\n\n");
    }
    else
    {
      Serial_PutString((uint8_t* )"  Enable the write protection -------------------------- 4\r\n\n");
    }
    Serial_PutString((uint8_t* )"==========================================================\r\n\n");

    /* Clean the input path (防止用户多输入几个字符)*/
    __UART_FLUSH_DATAREGISTER(USART0);
	
    /* Receive key */
    GD_UART_Receive(USART0, &key, 1, 0xFFFFFFFFU); // TODO: 10s 后自动进入APP

    switch (key)
    {
    case '1' :
      /* Download user application in the Flash */
      SerialDownload();
      break;
    case '2' :
      /* Upload user application from the Flash */
    Serial_PutString((uint8_t* )"Upload\n");
      break;
    case '3' :
      Serial_PutString((uint8_t *)"Start program execution......\r\n\n");
      jump_to_app(APPLICATION_ADDRESS);
      /* execute the new program */
      break;
    case '4' :
      if (FlashProtection != FLASHIF_PROTECTION_NONE)
      {
        /* Disable the write protection */
        if (app_flash_write_protection_disable() == SUCCESS)
        {
          Serial_PutString((uint8_t* )"Write Protection disabled...\r\n");
          Serial_PutString((uint8_t* )"System will now restart...\r\n");
        }
        else
        {
          Serial_PutString((uint8_t* )"Error: Flash write un-protection failed...\r\n");
        }
      }
      else
      {
          /* enable the write protection */
        if (app_flash_write_protection_enable() == SUCCESS)
        {
          Serial_PutString((uint8_t* )"Write Protection enabled...\r\n");
          Serial_PutString((uint8_t* )"System will now restart...\r\n");
        }
        else
        {
          Serial_PutString((uint8_t* )"Error: Flash write protection failed...\r\n");
        }
      }
      break;
    default:
	Serial_PutString((uint8_t* )"Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
	break;
    }
  }
}

/**
  * @}
  */
