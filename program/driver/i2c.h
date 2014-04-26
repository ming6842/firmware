/* #include "stm32f4_i2c.h" */

#ifndef __STM32F4_I2C_H
#define __STM32F4_I2C_H

#include "stm32f4xx.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
void I2C_Config(void);

u32 I2C_ReadBytes(volatile u8 SlaveAddr, volatile u8 *ReadBuf, u8 NumByte);
u32 I2C_WriteBytes(volatile u8 SlaveAddr, volatile u8 *WriteBuf, u8 NumByte);
u32 I2C_DMA_ReadReg(volatile u8 SlaveAddr, volatile u8 ReadAddr, volatile u8 *ReadBuf, u8 NumByte);
u32 I2C_DMA_WriteReg(volatile u8 SlaveAddr, volatile u8 WriteAddr, volatile u8 *WriteBuf, u8 NumByte);

u32 I2C_TimeOut(void);

void I2C_TX_DMA_IRQ(void);
void I2C_RX_DMA_IRQ(void);
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
