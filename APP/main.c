#include "gd32f4xx.h"
#include "gd32f427v_start.h"
#include "systick.h"
#include "gd32f427v_usart.h"
#include "stdlib.h"
#include "gd32f4xx_usart.h"
#include "fmc_operation.h"
#include "common.h"
#include "menu.h"
#include "main.h"

int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    /* configure systick */
    systick_config();
    
    gd_eval_led_init(LED2);
    gd_eval_led_on(LED2);
    
    common_init();
    init_print(USART_1, 115200); // debug
    printf("into Bootloader\n"); // debug
    Main_Menu();
    while(1) {
        gd_eval_led_toggle(LED2);
        delay_ms(100);
    }
}

void jump_to_app(uint32_t addr)
{
	uint32_t app_address;
	void* (*application)(void);
    
	/* test if user code is programmed starting from address */
	if (((*(__IO uint32_t*)addr) & 0x2FFE0000) == 0x20000000)
	{
		app_address = *(__IO uint32_t*) (addr + 4);
		application = (void* (*)(void)) app_address;

		/* initialize user application's stack pointer */
		__set_MSP(*(__IO uint32_t*) addr);
		
        
		rcu_deinit();
		
		/* jump to user application */
		application();
	}
}
