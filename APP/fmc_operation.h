/*!
    \file    fmc_operation.h
    \brief   the header file of fmc_operation 
    
    \version 2023-06-25, V3.1.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef __FMC_OPERATION_H
#define __FMC_OPERATION_H

#include "gd32f4xx.h"
#include "printf.h"

/* FMC sector information */
typedef struct
{
    uint32_t sector_name;                                         /*!< the name of the sector */
    uint32_t sector_num;                                          /*!< the number of the sector */
    uint32_t sector_size;                                         /*!< the size of the sector */
    uint32_t sector_start_addr;                                   /*!< the start address of the sector */
    uint32_t sector_end_addr;                                     /*!< the end address of the sector */
} fmc_sector_info_struct;

/* Error code */
enum 
{
  FLASH_OK = 0,
  FLASH_ERASEKO,
  FLASH_WRITINGCTRL_ERROR,
  FLASH_WRITING_ERROR
};

/* sector size */
#define SIZE_16KB                  ((uint32_t)0x00004000U)        /*!< size of 16KB*/
#define SIZE_64KB                  ((uint32_t)0x00010000U)        /*!< size of 64KB*/
#define SIZE_128KB                 ((uint32_t)0x00020000U)        /*!< size of 128KB*/
#define SIZE_256KB                 ((uint32_t)0x00040000U)        /*!< size of 256KB*/

/* FMC BANK address */
#define FMC_START_ADDRESS          FLASH_BASE                               /*!< FMC start address */
#define FMC_BANK0_START_ADDRESS    FMC_START_ADDRESS                        /*!< FMC BANK0 start address */
#define FMC_BANK1_START_ADDRESS    ((uint32_t)0x08100000U)                  /*!< FMC BANK1 start address */
#define FMC_SIZE                   (*(uint16_t *)0x1FFF7A22U)               /*!< FMC SIZE */
#define FMC_END_ADDRESS            (FLASH_BASE + (FMC_SIZE * 1024) - 1)     /*!< FMC end address */
#define FMC_MAX_END_ADDRESS        ((uint32_t)0x08300000U)                  /*!< FMC maximum end address */

/* FMC error message */
#define FMC_WRONG_SECTOR_NAME      ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector name*/
#define FMC_WRONG_SECTOR_NUM       ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector number*/
#define FMC_INVALID_SIZE           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector size*/
#define FMC_INVALID_ADDR           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector address*/

/* base address of the FMC sectors */
/* Block0: 1024KB */
#define ADDR_FMC_SECTOR_0       ((uint32_t)0x08000000) /*!< base address of sector 0,  16 kbytes */
#define ADDR_FMC_SECTOR_1       ((uint32_t)0x08004000) /*!< base address of sector 1,  16 kbytes */
#define ADDR_FMC_SECTOR_2       ((uint32_t)0x08008000) /*!< base address of sector 2,  16 kbytes */
#define ADDR_FMC_SECTOR_3       ((uint32_t)0x0800C000) /*!< base address of sector 3,  16 kbytes */
#define ADDR_FMC_SECTOR_4       ((uint32_t)0x08010000) /*!< base address of sector 4,  64 kbytes */
#define ADDR_FMC_SECTOR_5       ((uint32_t)0x08020000) /*!< base address of sector 5, 128 kbytes */
#define ADDR_FMC_SECTOR_6       ((uint32_t)0x08040000) /*!< base address of sector 6, 128 kbytes */
#define ADDR_FMC_SECTOR_7       ((uint32_t)0x08060000) /*!< base address of sector 7, 128 kbytes */
#define ADDR_FMC_SECTOR_8       ((uint32_t)0x08080000) /*!< base address of sector 8, 128 kbytes */
#define ADDR_FMC_SECTOR_9       ((uint32_t)0x080A0000) /*!< base address of sector 9, 128 kbytes */
#define ADDR_FMC_SECTOR_10      ((uint32_t)0x080C0000) /*!< base address of sector 10,128 kbytes */
#define ADDR_FMC_SECTOR_11      ((uint32_t)0x080E0000) /*!< base address of sector 11,128 kbytes */
/* Block1: 2816KB */
#define ADDR_FMC_SECTOR_12      ((uint32_t)0x08100000) /*!< base address of sector 12, 16 kbytes */
#define ADDR_FMC_SECTOR_13      ((uint32_t)0x08104000) /*!< base address of sector 13, 16 kbytes */
#define ADDR_FMC_SECTOR_14      ((uint32_t)0x08108000) /*!< base address of sector 14, 16 kbytes */
#define ADDR_FMC_SECTOR_15      ((uint32_t)0x0810C000) /*!< base address of sector 15, 16 kbytes */
#define ADDR_FMC_SECTOR_16      ((uint32_t)0x08110000) /*!< base address of sector 16, 64 kbytes */
#define ADDR_FMC_SECTOR_17      ((uint32_t)0x08120000) /*!< base address of sector 17,128 kbytes */
#define ADDR_FMC_SECTOR_18      ((uint32_t)0x08140000) /*!< base address of sector 18,128 kbytes */
#define ADDR_FMC_SECTOR_19      ((uint32_t)0x08160000) /*!< base address of sector 19,128 kbytes */
#define ADDR_FMC_SECTOR_20      ((uint32_t)0x08180000) /*!< base address of sector 20,128 kbytes */
#define ADDR_FMC_SECTOR_21      ((uint32_t)0x081A0000) /*!< base address of sector 21,128 kbytes */
#define ADDR_FMC_SECTOR_22      ((uint32_t)0x081C0000) /*!< base address of sector 22,128 kbytes */
#define ADDR_FMC_SECTOR_23      ((uint32_t)0x081E0000) /*!< base address of sector 23,128 kbytes */
#define ADDR_FMC_SECTOR_24      ((uint32_t)0x08200000) /*!< base address of sector 24,256 kbytes */
#define ADDR_FMC_SECTOR_25      ((uint32_t)0x08240000) /*!< base address of sector 25,256 kbytes */
#define ADDR_FMC_SECTOR_26      ((uint32_t)0x08280000) /*!< base address of sector 26,256 kbytes */
#define ADDR_FMC_SECTOR_27      ((uint32_t)0x082C0000) /*!< base address of sector 27,256 kbytes */

/* get the sector number, size and range of the given address */
fmc_sector_info_struct fmc_sector_info_get(uint32_t addr);
/* get the sector number by sector name */
uint32_t sector_name_to_number(uint32_t sector_name);
/* erases the sector of a given sector number */
void fmc_erase_sector_by_address(uint32_t address);
/* write 32 bit length data to a given address */
//void fmc_write_32bit_data(uint32_t address, uint16_t length, int32_t* data_32);
uint32_t fmc_write_32bit_data(uint32_t address, uint16_t length, int32_t* data_32);
/* read 32 bit length data from a given address */
void fmc_read_32bit_data(uint32_t address, uint16_t length, int32_t* data_32);
/* write 16 bit length data to a given address */
void fmc_write_16bit_data(uint32_t address, uint16_t length, int16_t* data_16);
/* read 16 bit length data from a given address */
void fmc_read_16bit_data(uint32_t address, uint16_t length, int16_t* data_16);
/* write 8 bit length data to a given address */
void fmc_write_8bit_data(uint32_t address, uint16_t length, int8_t* data_8);
/* read 8 bit length data from a given address */
void fmc_read_8bit_data(uint32_t address, uint16_t length, int8_t* data_8);

#endif/* __FMC_OPERATION_H */
