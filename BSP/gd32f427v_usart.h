#ifndef GD32F427V_USART_H
#define GD32F427V_USART_H

#include "gd32f4xx.h"
#include "printf.h"
#include "stdlib.h"
#include "main.h"

#define USARTn              2U

typedef enum 
{
    USART_0 = 0,
    USART_1 = 1,
} usart_typedef_enum;

#define USART0_TX_PIN                   GPIO_PIN_6
#define USART0_RX_PIN                   GPIO_PIN_7
#define USART0_GPIO_PORT                GPIOB
#define USART0_GPIO_CLK                 RCU_GPIOB

#define USART1_TX_PIN                   GPIO_PIN_5
#define USART1_RX_PIN                   GPIO_PIN_6
#define USART1_GPIO_PORT                GPIOD
#define USART1_GPIO_CLK                 RCU_GPIOD

#define USART0_RDATA_ADDRESS            ((uint32_t)&USART_DATA(USART0))
#define USART1_RDATA_ADDRESS            ((uint32_t)&USART_DATA(USART1))

#define __UART_FLUSH_DATAREGISTER(usartx) USART_DATA(usartx)

void gd_usart0_init(uint32_t baudval);
void gd_usart1_init(uint32_t baudval);
void init_print(usart_typedef_enum usart_num, uint32_t baudval);
ErrStatus usart_receive_byte(uint32_t usart_periph, uint8_t* data);
ErrStatus usart_receive(uint32_t usart_periph, uint8_t *pData, uint16_t Size);

GD_StatusTypeDef GD_UART_Transmit(uint32_t usart_periph, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
GD_StatusTypeDef GD_UART_Receive(uint32_t usart_periph, uint8_t *pData, uint16_t Size, uint32_t Timeout);

#endif /* GD32F427V_USART_H */
