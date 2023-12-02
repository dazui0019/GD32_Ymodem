#include "gd32f427v_usart.h"
#include "systick.h"
#include "main.h"

/* private variables */
const static uint32_t USART_X[USARTn]                           = {USART0, USART1};
const static uint32_t USART_GPIO_PORT[USARTn]                   = {USART0_GPIO_PORT, USART1_GPIO_PORT};
const static uint32_t USART_GPIO_PIN_RX[USARTn]                 = {USART0_RX_PIN, USART1_RX_PIN};
const static uint32_t USART_GPIO_PIN_TX[USARTn]                 = {USART0_TX_PIN, USART1_TX_PIN};
const static rcu_periph_enum USART_GPIO_CLK[USARTn]             = {USART0_GPIO_CLK, USART1_GPIO_CLK};
const static rcu_periph_enum USART_CLK[USARTn]                  = {RCU_USART0, RCU_USART1};

static uint32_t pUART = NULL; // printf重定向时使用。

void gd_usart_init(usart_typedef_enum usart_num, uint32_t baudval)
    {
    /* enable GPIO clock */
    rcu_periph_clock_enable(USART_GPIO_CLK[usart_num]);

    /* enable USART clock */
    rcu_periph_clock_enable(USART_CLK[usart_num]);

    /* configure the USARTx TX pin and USARTx RX pin */
    gpio_af_set(USART_GPIO_PORT[usart_num], GPIO_AF_7, USART_GPIO_PIN_TX[usart_num]);
    gpio_af_set(USART_GPIO_PORT[usart_num], GPIO_AF_7, USART_GPIO_PIN_RX[usart_num]);

    /* configure USARTx TX as alternate function push-pull */
    gpio_mode_set(USART_GPIO_PORT[usart_num], GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART_GPIO_PIN_TX[usart_num]);
    gpio_output_options_set(USART_GPIO_PORT[usart_num], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART_GPIO_PIN_TX[usart_num]);

    /* configure USARTx RX as alternate function push-pull */
    gpio_mode_set(USART_GPIO_PORT[usart_num], GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART_GPIO_PIN_RX[usart_num]);
    gpio_output_options_set(USART_GPIO_PORT[usart_num], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART_GPIO_PIN_RX[usart_num]);

    /* USART configure */
    usart_deinit(USART_X[usart_num]);
    usart_baudrate_set(USART_X[usart_num], baudval);
    usart_receive_config(USART_X[usart_num], USART_RECEIVE_ENABLE);
    usart_transmit_config(USART_X[usart_num], USART_TRANSMIT_ENABLE);
    usart_enable(USART_X[usart_num]);
}

void gd_usart0_init(uint32_t baudval){
    gd_usart_init(USART_0, baudval);
}

void gd_usart1_init(uint32_t baudval){
    gd_usart_init(USART_1, baudval);
}

/* 以下函数配合tinyprintf.c使用 */
void fputc(void* p, char ch)
{
    usart_data_transmit(pUART, (uint8_t)ch);
    while(RESET == usart_flag_get(pUART, USART_FLAG_TBE));
}

/* 初始化 */
void init_print(usart_typedef_enum usart_num, uint32_t baudval){
    pUART = USART_X[usart_num];
    init_printf(NULL, fputc);
    gd_usart_init(usart_num, baudval);
}

/* 接收一字节数据 */
ErrStatus usart_receive_byte(uint32_t usart_periph, uint8_t* data)
{
    /* 等待串口输入数据 */
    while (usart_flag_get(usart_periph, USART_FLAG_RBNE) == RESET);
    *data = (uint8_t)usart_data_receive(usart_periph);
    return SUCCESS;
}

/* 通过阻塞模式，接收多字节数据 */
ErrStatus usart_receive(uint32_t usart_periph, uint8_t *pData, uint16_t Size)
{
    uint16_t i = 0;
    while (i < Size){
        /* 等待串口输入数据 */
        if(usart_flag_get(usart_periph, USART_FLAG_RBNE) == SET){
            *(pData+i) = (uint8_t)usart_data_receive(usart_periph);
            i++;
        }
    }
    return SUCCESS;
}

/**
  * @brief  This function handles UART Communication Timeout. It waits
  *         until a flag is no longer in the specified status.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  Flag specifies the UART flag to check.
  * @param  Status The actual Flag status (SET or RESET).
  * @param  Tickstart Tick start value
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
static GD_StatusTypeDef UART_WaitOnFlagUntilTimeout(uint32_t usart_periph, usart_flag_enum Flag, FlagStatus Status,
                                                     uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while ((usart_flag_get(usart_periph, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != MAX_DELAY)
    {
      if (((getTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {

        return GD_TIMEOUT;
      }
    }
  }
  return GD_OK;
}

/**
  * @brief  Sends an amount of data in blocking mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the sent data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 provided through pData.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *               the configuration information for the specified UART module.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be sent
  * @param  Timeout Timeout duration
  * @retval GD status
  */
GD_StatusTypeDef GD_UART_Transmit(uint32_t usart_periph, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    const uint8_t  *pdata8bits = pData;
    uint16_t count = Size;
    uint32_t tickstart = 0U;
    
    if ((pData == NULL) || (Size == 0U))
    {
      return  GD_ERROR;
    }

    /* Init tickstart for timeout management */
    tickstart = getTick();

    while (count > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(usart_periph, USART_FLAG_TBE, RESET, tickstart, Timeout) != GD_OK)
      {
        return GD_TIMEOUT;
      }
      USART_DATA(usart_periph) = USART_DATA_DATA & *pdata8bits;
      pdata8bits++;
      count--;
    }

    if (UART_WaitOnFlagUntilTimeout(usart_periph, USART_FLAG_TBE, RESET, tickstart, Timeout) != GD_OK)
    {
      return GD_TIMEOUT;
    }

    return GD_OK;
}

/**
  * @brief  Receives an amount of data in blocking mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *               the configuration information for the specified UART module.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be received.
  * @param  Timeout Timeout duration
  * @retval GD status
  */
GD_StatusTypeDef GD_UART_Receive(uint32_t usart_periph, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint8_t  *pdata8bits = pData;
    uint16_t count = Size;
    uint32_t tickstart = 0U;

  /* Check that a Rx process is not already ongoing */
    if ((pData == NULL) || (Size == 0U))
    {
      return  GD_ERROR;
    }

    /* Init tickstart for timeout management */
    tickstart = getTick();

    pdata8bits  = pData;

    /* Check the remain data to be received */
    while (count > 0U)
    {
      if (UART_WaitOnFlagUntilTimeout(usart_periph, USART_FLAG_RBNE, RESET, tickstart, Timeout) != GD_OK)
      {
        return GD_TIMEOUT;
      }
      *pdata8bits = (uint8_t)usart_data_receive(usart_periph);
      pdata8bits++;
      count--;
    }
    return GD_OK;
}
