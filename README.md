# GD32_Ymodem

IAP + Ymodem

USART0(YMODEM)
- PB6: Tx
- PB7: Rx

USART1(DEBUG)
- PD5: Tx
- PD6: Rx

Hardware: GD32F427V-Start \
reference: [STM32CubeF4](https://github.com/STMicroelectronics/STM32CubeF4/tree/9e7d2aa86197059fc29cf89931d46d0ab64e6081/Projects/STM32446E_EVAL/Applications/IAP/IAP_Main)

## 闪存结构

Block0(1024KB): 0x08000000-0x080FFFFF\
Block1(2816KB): 0x08100000-0x082FFFFF

其中Block0的前512KByte是零等待闪存, 所以我觉得代码尽量放到这里。

目前是`Sector0 - Sector1`用来存放bootloader, `Sector2 - Sector7`用来存放APP。后面打算把`Sector2 - Sector11`全部用来存放APP。
```
Top Flash Memory address  /-------------------------------------------\  0x082FFFFF
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |          Sector 12 - Sector 27            |
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |                                           |
                          |-------------------------------------------|	 0x080FFFFF
                          |                                           |
                          |                                           |
                          |          Sector 8 - Sector 11             |
                          |                                           |
                          |                                           |
                          |-------------------------------------------|  0x0807FFFF
                          |          User code                        |
                          |- - - - - - - - - - - - - - - - - - - - - -|
                          |          Vector table                     |
                  		  |-------------------------------------------|  0x08008000
                          |          IAP code                         |
                          |- - - - - - - - - - - - - - - - - - - - - -|
                          |          Vector table                     |
                          \-------------------------------------------/	 0x08000000					  
```
