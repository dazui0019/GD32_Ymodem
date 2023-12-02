#ifndef __IAP_C__
#define __IAP_C__

#include "gd32f4xx.h"

#define IAP_USART USART0

/* Error code */
enum 
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR
};
  
enum{
  FLASHIF_PROTECTION_NONE         = 0,
  FLASHIF_PROTECTION_PCROPENABLED = 0x1,
  FLASHIF_PROTECTION_WRPENABLED   = 0x2,
  FLASHIF_PROTECTION_RDPENABLED   = 0x4,
};

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x0801FFFF is reserved for the IAP code */
#define APPLICATION_ADDRESS   (uint32_t)0x08020000
/* End of the Flash address */
#define APP_FLASH_END_ADDRESS        0x0807FFFF // 512Kbyte for bootloader and app
/* Define the user application size */
#define APP_FLASH_SIZE   ((APP_FLASH_END_ADDRESS + 1) - APPLICATION_ADDRESS)
/* Define bitmap representing user flash area that could be write protected . */
#define APP_FLASH_SECTOR ( OB_WP_2 | OB_WP_3 | OB_WP_4 |\
                           OB_WP_5 | OB_WP_6 | OB_WP_7 )

/* get write or erase protection state */
uint8_t flash_write_protection_get(uint32_t ob_wp);
/* get write or erase protection state of app flash  sector*/
uint8_t app_flash_write_protection_get(void);
/* enable write protection of app flash */
ErrStatus app_flash_write_protection_enable(void);
/* disable write protection of app flash */
ErrStatus app_flash_write_protection_disable(void);

uint32_t iap_write_32bit_data(uint32_t address, uint16_t length, int32_t* data_32);

#endif
