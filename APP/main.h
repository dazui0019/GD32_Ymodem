#ifndef __MAIN_H
#define __MAIN_H

#include "gd32f4xx.h"

/** 
  * @brief  GD Status structures definition  
  */  
typedef enum 
{
  GD_OK       = 0x00U,
  GD_ERROR    = 0x01U,
  GD_BUSY     = 0x02U,
  GD_TIMEOUT  = 0x03U
} GD_StatusTypeDef;

void jump_to_app(uint32_t addr);

#endif
