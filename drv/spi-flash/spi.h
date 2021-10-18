#ifndef __SPI_H
#define __SPI_H
#include "bsp_init.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//SPI驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/16/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern SPI_HandleTypeDef SPI4_Handler;  //SPI句柄

void SPI4_Init(void);
void SPI4_SetSpeed(UINT8 SPI_BaudRatePrescaler);
UINT8 SPI4_ReadWriteByte(UINT8 TxData);
#endif
