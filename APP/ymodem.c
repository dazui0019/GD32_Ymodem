/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/ymodem.c 
  * @author  MCD Application Team
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
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
#include "main.h"
#include "gd32f427v_usart.h"
#include "iap.h"
#include "common.h"
#include "ymodem.h"
#include "string.h"
#include "menu.h"
#include "fmc_operation.h"
#include "flash_operation.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CRC16_F       /* activate the CRC16 integrity */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t flashdestination;
/* @note ATTENTION - please keep this variable 32bit aligned */
__attribute__((aligned(32))) uint8_t aPacketData[PACKET_1K_SIZE + PACKET_DATA_INDEX + PACKET_TRAILER_SIZE];

/* Private function prototypes -----------------------------------------------*/
//static void PrepareIntialPacket(uint8_t *p_data, const uint8_t *p_file_name, uint32_t length);
//static void PreparePacket(uint8_t *p_source, uint8_t *p_packet, uint8_t pkt_nr, uint32_t size_blk);
//static ErrStatus ReceivePacket(uint8_t *p_data, uint32_t *p_length, uint32_t timeout);
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte);
uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size);
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  *     0: end of transmission
  *     2: abort by sender
  *    >0: packet length
  * @param  timeout
  * @retval GD_OK: normally return
  *         GD_BUSY: abort by user
  */
static GD_StatusTypeDef ReceivePacket(uint8_t *p_data, uint32_t *p_length, uint32_t timeout)
{
  uint32_t crc;
  uint32_t packet_size = 0;
  GD_StatusTypeDef status;
  uint8_t char1;

  *p_length = 0;
  status = GD_UART_Receive(IAP_USART, &char1, 1, timeout);

  if (status == GD_OK)
  {
    switch (char1)
    {
      case SOH:
        packet_size = PACKET_SIZE;
        break;
      case STX:
        packet_size = PACKET_1K_SIZE;
        break;
      case EOT:
        break;
      case CA:
        if ((GD_UART_Receive(IAP_USART, &char1, 1, timeout) == GD_OK) && (char1 == CA))
        {
          packet_size = 2;
        }
        else
        {
          status = GD_ERROR;
        }
        break;
      case ABORT1:
      case ABORT2:
        status = GD_BUSY;
        break;
      default:
        status = GD_ERROR;
        break;
    }
    *p_data = char1;

    if (packet_size >= PACKET_SIZE )
    {
      status = GD_UART_Receive(IAP_USART, &p_data[PACKET_NUMBER_INDEX], packet_size + PACKET_OVERHEAD_SIZE, timeout);

      /* Simple packet sanity check */
      if (status == GD_OK )
      {
        if (p_data[PACKET_NUMBER_INDEX] != ((p_data[PACKET_CNUMBER_INDEX]) ^ NEGATIVE_BYTE))
        {
          packet_size = 0;
          status = GD_ERROR;
        }
        else
        {
          /* Check packet CRC */
          crc = p_data[ packet_size + PACKET_DATA_INDEX ] << 8;
          crc += p_data[ packet_size + PACKET_DATA_INDEX + 1 ];
          if (Cal_CRC16(&p_data[PACKET_DATA_INDEX], packet_size) != crc )
          {
            packet_size = 0;
            status = GD_ERROR;
          }
        }
      }
      else
      {
        packet_size = 0;
      }
    }
  }
  *p_length = packet_size;
  return status;
}

/**
  * @brief  Update CRC16 for input byte
  * @param  crc_in input value 
  * @param  input byte
  * @retval None
  */
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
  uint32_t crc = crc_in;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}

/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = p_data+size;

  while(p_data < dataEnd)
    crc = UpdateCRC16(crc, *p_data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}

/**
  * @brief  Calculate Check sum for YModem Packet
  * @param  p_data Pointer to input data
  * @param  size length of input data
  * @retval uint8_t checksum value
  */
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size)
{
  uint32_t sum = 0;
  const uint8_t *p_data_end = p_data + size;

  while (p_data < p_data_end )
  {
    sum += *p_data++;
  }

  return (sum & 0xffu);
}

/* Public functions ---------------------------------------------------------*/
/**
  * @brief  Receive a file using the ymodem protocol with CRC16.
  * @param  p_size The size of the file.
  * @retval COM_StatusTypeDef result of reception/programming
  */
COM_StatusTypeDef Ymodem_Receive ( uint32_t *p_size )
{
  uint32_t i, packet_length, session_done = 0, file_done, errors = 0, session_begin = 0;
 // uint32_t flashdestination;
  uint32_t ramsource, filesize, packets_received;
  uint8_t *file_ptr;
  uint8_t file_size[FILE_SIZE_LENGTH], tmp;
  COM_StatusTypeDef result = COM_OK;

  /* Initialize flashdestination variable */
  flashdestination = APPLICATION_ADDRESS;

  while ((session_done == 0) && (result == COM_OK))
  {
    packets_received = 0;
    file_done = 0;
    while ((file_done == 0) && (result == COM_OK))
    {
      switch (ReceivePacket(aPacketData, &packet_length, DOWNLOAD_TIMEOUT))
      {
        case GD_OK:
          errors = 0;
          switch (packet_length)
          {
            case 2:
              /* Abort by sender */
              Serial_PutByte(ACK);
              result = COM_ABORT;
              break;
            case 0:
              /* End of transmission */
              Serial_PutByte(ACK);
              file_done = 1;
              break;
            default:
              /* Normal packet */
              if (aPacketData[PACKET_NUMBER_INDEX] != (uint8_t)packets_received)
              {
                Serial_PutByte(NAK);
              }
              else
              {
                if (packets_received == 0)
                {
                  /* File name packet */
                  if (aPacketData[PACKET_DATA_INDEX] != 0)
                  {
                    /* File name extraction */
                    i = 0;
                    file_ptr = aPacketData + PACKET_DATA_INDEX;
                    while ( (*file_ptr != 0) && (i < FILE_NAME_LENGTH))
                    {
                      aFileName[i++] = *file_ptr++;
                    }

                    /* File size extraction */
                    aFileName[i++] = '\0';
                    i = 0;
                    file_ptr ++;
                    while ( (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH))
                    {
                      file_size[i++] = *file_ptr++;
                    }
                    file_size[i++] = '\0';
                    Str2Int(file_size, &filesize);

                    /* Test the size of the image to be sent */
                    /* Image size is greater than Flash size */
                    if (*p_size > (APP_FLASH_SIZE + 1))
                    {
                      /* End session */
                      tmp = CA;
                      GD_UART_Transmit(IAP_USART, &tmp, 1, NAK_TIMEOUT);
                      GD_UART_Transmit(IAP_USART, &tmp, 1, NAK_TIMEOUT);
                      result = COM_LIMIT;
                    }
                    /* erase user application area */
                    fmc_erase_sector_by_address(APPLICATION_ADDRESS);
                    *p_size = filesize;

                    Serial_PutByte(ACK);
                    Serial_PutByte(CRC16);
                  }
                  /* File header packet is empty, end session */
                  else
                  {
                    Serial_PutByte(ACK);
                    file_done = 1;
                    session_done = 1;
                    break;
                  }
                }
                else /* Data packet */
                {
                  ramsource = (uint32_t) & aPacketData[PACKET_DATA_INDEX];
                  /* Write received data in Flash */
                  if (iap_write_32bit_data(flashdestination, packet_length/4, (int32_t*) ramsource) == FLASHIF_OK)
                  {
                    flashdestination += packet_length;
                    Serial_PutByte(ACK);
                  }
                  else /* An error occurred while writing to Flash memory */
                  {
                    /* End session */
                    Serial_PutByte(CA);
                    Serial_PutByte(CA);
                    result = COM_DATA;
                  }
                }
                packets_received ++;
                session_begin = 1;
              }
              break;
          }
          break;
        case GD_BUSY: /* Abort actually */
          Serial_PutByte(CA);
          Serial_PutByte(CA);
          result = COM_ABORT;
          break;
        default:
          if (session_begin > 0)
          {
            errors ++;
          }
          if (errors > MAX_ERRORS)
          {
            /* Abort communication */
            Serial_PutByte(CA);
            Serial_PutByte(CA);
          }
          else
          {
            Serial_PutByte(CRC16); /* Ask for a packet */
          }
          break;
      }
    }
  }
  return result;
}
/**
  * @}
  */

/*******************(C)COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
