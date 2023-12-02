#include "iap.h"
#include "fmc_operation.h"
#include "printf.h"

/*!
    \brief    get write protection
    \param[in]  ob_wp: specify sector to be get
                one or more parameters can be selected which are shown as below:
      \arg        OB_WP_x(x=0..22):sector x(x = 0,1,2...22)
      \arg        OB_WP_23_27: sector23~27
      \arg        OB_WP_ALL: all sector
    \param[out] none
    \retval     1 protected, 0 no protected
*/
uint8_t flash_write_protection_get(uint32_t ob_wp)
{
    uint16_t res;
    if(ob_wp < OB_WP_12){
        res = ob_write_protection0_get();
        /* 先将读到的寄存器值"ob_write_protection0_get()"按位取反, 再与ob_wp(OB_WP_x)按位与, 此时被置零(被保护)的位会保留为1. */
        res = (~res) & (uint16_t)ob_wp; 
    }
    else{
        res = ob_write_protection1_get();
        /* 同上 */
        res = (~res) & (uint16_t)(ob_wp >> 16);
    }
    if(res == 0)
        return FLASHIF_PROTECTION_NONE;
    else
        return FLASHIF_PROTECTION_PCROPENABLED;
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint8_t app_flash_write_protection_get(void)
{
    return flash_write_protection_get(APP_FLASH_SECTOR);
}

/**
  * @brief  enable write protection of app flash sector.
  * @param  None
  * @retval 0: ERROR
  *         1: SUCCESS
  */
ErrStatus app_flash_write_protection_enable(void)
{
    ErrStatus res;
    ob_unlock();
    res = ob_write_protection_enable(APP_FLASH_SECTOR);
    ob_lock();
    return res;
}

/**
  * @brief  disable write protection of app flash sector.
  * @param  None
  * @retval 0: ERROR
  *         1: SUCCESS
  */
ErrStatus app_flash_write_protection_disable(void)
{
    ErrStatus res;
    ob_unlock();
    res = ob_write_protection_disable(APP_FLASH_SECTOR);
    ob_lock();
    return res;
}

/*!
    \brief      write 32 bit length data to a given address
    \param[in]  address: a given address(0x08000000~0x082FFFFF)
    \param[in]  length: data length
    \param[in]  data_32: data pointer
    \param[out] none
    \retval     none
*/
uint32_t iap_write_32bit_data(uint32_t address, uint16_t length, int32_t* data_32)
{
    uint32_t idx = 0U;
    
    CLR_DEBUG("FMC word programe operation:");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    /* write data_32 to the corresponding address */
    for(idx = 0U; idx < length; idx += 1){
        if(FMC_READY == fmc_word_program(address, *(data_32+idx))){
            address = address + 4;
        }else{ 
            while(1);
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    CLR_DEBUG("Write complete!");
    CLR_DEBUG();
    return FLASH_OK;
}
