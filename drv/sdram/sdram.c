#include "bsp_init.h"
#include "stm32f4xx_hal_sdram.h"
#include "stm32f4xx_ll_fmc.h"
#include <string.h>

#define SDRAM_MODEREG_BURST_LENGTH_1             ((UINT16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((UINT16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((UINT16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((UINT16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((UINT16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((UINT16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((UINT16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((UINT16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((UINT16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((UINT16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((UINT16)0x0200)


static SDRAM_HandleTypeDef SDRAM_Handler;

static UINT8 SDRAM_Send_Cmd(UINT8 bankx,UINT8 cmd,UINT8 refresh,UINT16 regval)
{
	UINT32 target_bank = 0;
	FMC_SDRAM_CommandTypeDef Command;

	if (bankx == 0)
		target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
	else if (bankx == 1)
		target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.CommandMode = cmd;
	Command.CommandTarget = target_bank;
	Command.AutoRefreshNumber = refresh;
	Command.ModeRegisterDefinition = regval;
	if (HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command,0X1000) == HAL_OK) {
		return 0;
	}
	else
		return 1;
}

static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
	UINT32 temp = 0;
	SDRAM_Send_Cmd(1, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);
	delay_us(500);
	SDRAM_Send_Cmd(1, FMC_SDRAM_CMD_PALL, 1, 0);
	SDRAM_Send_Cmd(1, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);
	temp = (UINT32)SDRAM_MODEREG_BURST_LENGTH_1		|
			SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL		|
			SDRAM_MODEREG_CAS_LATENCY_3				|
			SDRAM_MODEREG_OPERATING_MODE_STANDARD	|
			SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
	SDRAM_Send_Cmd(1, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_FMC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_Initure.Mode = GPIO_MODE_AF_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	GPIO_Initure.Alternate = GPIO_AF12_FMC;

	GPIO_Initure.Pin = GPIO_PIN_5|GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);

	GPIO_Initure.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);

	GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);

	GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10| GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);

	GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);

	GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_Initure);
	
	
}

extern int __sdram_data_begin, __sdram_data_end;
void sdram_init(void)
{

	FMC_SDRAM_TimingTypeDef SDRAM_Timing;
	SDRAM_Handler.Instance=FMC_SDRAM_DEVICE;
	SDRAM_Handler.Init.SDBank=FMC_SDRAM_BANK2;
	SDRAM_Handler.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_9;
	SDRAM_Handler.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_13;
	SDRAM_Handler.Init.MemoryDataWidth=FMC_SDRAM_MEM_BUS_WIDTH_16;
	SDRAM_Handler.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_4;
	SDRAM_Handler.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_3;
	SDRAM_Handler.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	SDRAM_Handler.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;
	SDRAM_Handler.Init.ReadBurst=FMC_SDRAM_RBURST_ENABLE;
	SDRAM_Handler.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_1;
	SDRAM_Timing.LoadToActiveDelay=2;
	SDRAM_Timing.ExitSelfRefreshDelay=8;
	SDRAM_Timing.SelfRefreshTime=6;
	SDRAM_Timing.RowCycleDelay=6;
	SDRAM_Timing.WriteRecoveryTime=2;
	SDRAM_Timing.RPDelay=2;
	SDRAM_Timing.RCDDelay=2;

	HAL_SDRAM_Init(&SDRAM_Handler,&SDRAM_Timing);
	SDRAM_Initialization_Sequence(&SDRAM_Handler);
	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler, 683);
	/* sdram 清零 */
	memset((void*)(size_t)(&__sdram_data_begin), 0, (size_t)(&__sdram_data_end) - (size_t)(&__sdram_data_begin));
}

