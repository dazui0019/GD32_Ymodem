#include "iap_flash_operation.h"
#include "fmc_operation.h"
#include "printf.h"

uint32_t fmc_sector_get(uint32_t addr);

/* Private functions ---------------------------------------------------------*/

/*!
    \brief      gets the sector of a given address
    \param[in]  address: a given address(0x08000000~0x080FFFFF)
    \param[out] none
    \retval     the sector of a given address
*/
uint32_t fmc_sector_get(uint32_t addr)
{
    uint32_t sector = 0U;

    if ((addr < ADDR_FMC_SECTOR_1) && (addr >= ADDR_FMC_SECTOR_0)) {
        sector = CTL_SECTOR_NUMBER_0;
    } else if ((addr < ADDR_FMC_SECTOR_2) && (addr >= ADDR_FMC_SECTOR_1)) {
        sector = CTL_SECTOR_NUMBER_1;
    } else if ((addr < ADDR_FMC_SECTOR_3) && (addr >= ADDR_FMC_SECTOR_2)) {
        sector = CTL_SECTOR_NUMBER_2;
    } else if ((addr < ADDR_FMC_SECTOR_4) && (addr >= ADDR_FMC_SECTOR_3)) {
        sector = CTL_SECTOR_NUMBER_3;  
    } else if ((addr < ADDR_FMC_SECTOR_5) && (addr >= ADDR_FMC_SECTOR_4)) {
        sector = CTL_SECTOR_NUMBER_4;
    } else if ((addr < ADDR_FMC_SECTOR_6) && (addr >= ADDR_FMC_SECTOR_5)) {
        sector = CTL_SECTOR_NUMBER_5;
    } else if ((addr < ADDR_FMC_SECTOR_7) && (addr >= ADDR_FMC_SECTOR_6)) {
        sector = CTL_SECTOR_NUMBER_6;
    } else if ((addr < ADDR_FMC_SECTOR_8) && (addr >= ADDR_FMC_SECTOR_7)) {
        sector = CTL_SECTOR_NUMBER_7;  
    } else if ((addr < ADDR_FMC_SECTOR_9) && (addr >= ADDR_FMC_SECTOR_8)) {
        sector = CTL_SECTOR_NUMBER_8;
    } else if ((addr < ADDR_FMC_SECTOR_10) && (addr >= ADDR_FMC_SECTOR_9)) {
        sector = CTL_SECTOR_NUMBER_9;
    } else if ((addr < ADDR_FMC_SECTOR_11) && (addr >= ADDR_FMC_SECTOR_10)) {
        sector = CTL_SECTOR_NUMBER_10;
    } else if ((addr < ADDR_FMC_SECTOR_12) && (addr >= ADDR_FMC_SECTOR_11)) {
        sector = CTL_SECTOR_NUMBER_11;
    } else if ((addr < ADDR_FMC_SECTOR_13) && (addr >= ADDR_FMC_SECTOR_12)) {
        sector = CTL_SECTOR_NUMBER_12;
    } else if ((addr < ADDR_FMC_SECTOR_14) && (addr >= ADDR_FMC_SECTOR_13)) {
        sector = CTL_SECTOR_NUMBER_13;
    } else if ((addr < ADDR_FMC_SECTOR_15) && (addr >= ADDR_FMC_SECTOR_14)) {
        sector = CTL_SECTOR_NUMBER_14;
    } else if ((addr < ADDR_FMC_SECTOR_16) && (addr >= ADDR_FMC_SECTOR_15)) {
        sector = CTL_SECTOR_NUMBER_15;
    } else if ((addr < ADDR_FMC_SECTOR_17) && (addr >= ADDR_FMC_SECTOR_16)) {
        sector = CTL_SECTOR_NUMBER_16;
    } else if ((addr < ADDR_FMC_SECTOR_18) && (addr >= ADDR_FMC_SECTOR_17)) {
        sector = CTL_SECTOR_NUMBER_17;
    } else if ((addr < ADDR_FMC_SECTOR_19) && (addr >= ADDR_FMC_SECTOR_18)) {
        sector = CTL_SECTOR_NUMBER_18;
    } else if ((addr < ADDR_FMC_SECTOR_20) && (addr >= ADDR_FMC_SECTOR_19)) {
        sector = CTL_SECTOR_NUMBER_19;
    } else if ((addr < ADDR_FMC_SECTOR_21) && (addr >= ADDR_FMC_SECTOR_20)) {
        sector = CTL_SECTOR_NUMBER_20;
    } else if ((addr < ADDR_FMC_SECTOR_22) && (addr >= ADDR_FMC_SECTOR_21)) {
        sector = CTL_SECTOR_NUMBER_21;
    } else if ((addr < ADDR_FMC_SECTOR_23) && (addr >= ADDR_FMC_SECTOR_22)) {
        sector = CTL_SECTOR_NUMBER_22;
    } else if ((addr < ADDR_FMC_SECTOR_24) && (addr >= ADDR_FMC_SECTOR_23)) {
        sector = CTL_SECTOR_NUMBER_23;
    } else if ((addr < ADDR_FMC_SECTOR_25) && (addr >= ADDR_FMC_SECTOR_24)) {
        sector = CTL_SECTOR_NUMBER_24;
    } else if ((addr < ADDR_FMC_SECTOR_26) && (addr >= ADDR_FMC_SECTOR_25)) {
        sector = CTL_SECTOR_NUMBER_25;
    } else if ((addr < ADDR_FMC_SECTOR_27) && (addr >= ADDR_FMC_SECTOR_26)) {
        sector = CTL_SECTOR_NUMBER_26;
    } else {
    }

    return sector;
}

/* Public functions ---------------------------------------------------------*/

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
    \brief      erase flash
    \param[in]  address: erase start address
    \param[in]  file_length: file length
    \param[in]  report_buffer: report buffer
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
void iap_flash_erase(uint32_t address, uint32_t flash_length)
{
    uint16_t start_sector = 0U, end_sector = 0U, i = 0U;
    
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    start_sector = fmc_sector_get(address);
    end_sector = fmc_sector_get(address + flash_length);
    CLR_DEBUG();
    CLR_DEBUG("IAP flash erase operation:");
    CLR_DEBUG("Start sector: 0x%X", start_sector);
    CLR_DEBUG("End sector: 0x%X", end_sector);
    CLR_DEBUG("sector count: %d", (end_sector-start_sector)/8);
    CLR_DEBUG();
    fmc_unlock();
    for (i = start_sector; i <= end_sector; i += 8U) {
        if (FMC_READY != fmc_sector_erase(i)) {
            while(1){
            }
        }
    }
   fmc_lock();
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
