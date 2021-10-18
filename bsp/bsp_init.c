#include "bsp_init.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "shell.h"
#include "sfud_def.h"
#include "ef_def.h"

void Stm32_Clock_Init(UINT32 plln,UINT32 pllm,UINT32 pllp,UINT32 pllq)
{
	HAL_StatusTypeDef ret = HAL_OK;
	RCC_OscInitTypeDef RCC_OscInitStructure; 
	RCC_ClkInitTypeDef RCC_ClkInitStructure;

	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStructure.HSEState=RCC_HSE_ON;
	RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;
	RCC_OscInitStructure.PLL.PLLM=pllm;
	RCC_OscInitStructure.PLL.PLLN=plln;
	RCC_OscInitStructure.PLL.PLLP=pllp;
	RCC_OscInitStructure.PLL.PLLQ=pllq;
	ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);

	if(ret!=HAL_OK)
		while(1);

	ret=HAL_PWREx_EnableOverDrive();
	if(ret!=HAL_OK)
		while(1);

	RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;
	RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4;
	RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2;
	ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_5);

	if(ret!=HAL_OK)
		while(1);
}

static UINT32 fac_us=0;
void delay_init(UINT8 SYSCLK)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	fac_us=SYSCLK;
}

void delay_us(UINT32 nus)
{
	UINT32 ticks;
	UINT32 told,tnow,tcnt=0;
	UINT32 reload=SysTick->LOAD;
	ticks=nus*fac_us;
	told=SysTick->VAL;
	while(1) {
		tnow=SysTick->VAL;
		if (tnow!=told) {
			if (tnow<told)
				tcnt+=told-tnow;
			else
				tcnt+=reload-tnow+told;
			told=tnow;
			if(tcnt>=ticks)
				break;
		}
	};
}

void delay_ms(UINT16 nms)
{
	UINT32 i;
	for(i=0;i<nms;i++)
		delay_us(1000);
}

int _write(int fd, char * pBuffer, int size)
{
	for (int i = 0; i < size; i++) {
		while ((USART1->SR & 0X40) == 0);
		USART1->DR			= (UINT8) pBuffer[i];
	}
	return size;
}

UINT8				iChar;
UART_HandleTypeDef	UART1_Handler;

void uart_init(UINT32 bound)
{
	UART1_Handler.Instance = USART1;
	UART1_Handler.Init.BaudRate = bound;
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;
	UART1_Handler.Init.Parity = UART_PARITY_NONE;
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&UART1_Handler);
	HAL_UART_Receive_IT(&UART1_Handler, &iChar, 1);
}

void HAL_UART_MspInit(UART_HandleTypeDef * huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	if (huart->Instance == USART1) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();

		GPIO_Initure.Pin	= GPIO_PIN_9;
		GPIO_Initure.Mode	= GPIO_MODE_AF_PP;
		GPIO_Initure.Pull	= GPIO_PULLUP;
		GPIO_Initure.Speed	= GPIO_SPEED_FAST;
		GPIO_Initure.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
		GPIO_Initure.Pin	= GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
		__HAL_UART_DISABLE_IT(huart, UART_IT_TC);
		__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
	}

}

extern QueueHandle_t xRxedChars;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1) {
		BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR(xRxedChars, &iChar, &pxHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( pxHigherPriorityTaskWoken );
	}
}

void USART1_IRQHandler(void)
{
	UINT32 timeout=0;
	UINT32 maxDelay=0x1FFFF;
	HAL_UART_IRQHandler(&UART1_Handler);
	timeout=0;
	while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) {
		timeout++;
		if(timeout>maxDelay)
			break;
	}
	timeout=0;
	while (HAL_UART_Receive_IT(&UART1_Handler, (UINT8 *)&iChar, 1) != HAL_OK) {
		timeout++;
		if(timeout>maxDelay)
			break;
	}
}

extern void sdram_init(void);
//extern UINT32 spi_flash_init(void);
extern sfud_err sfud_init(void);
extern EfErrCode easyflash_init(void);
extern void userShellInit(void);
extern void cm_backtrace_init(const char *firmware_name, \
						const char *hardware_ver, const char *software_ver);
void bsp_init()
{
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"
	HAL_Init();
	/* clock init */
	Stm32_Clock_Init(360,25,2,8);
	delay_init(180);
	/* uart  init */
	uart_init(115200);
	/* cm_backtracet init */
	cm_backtrace_init("stm32f429", HARDWARE_VERSION, SOFTWARE_VERSION);
	/* SDRAM init */
	sdram_init();
	/* flash init */
	//spi_flash_init();
	/* sfud init */
	sfud_init();
	/* easyflash init */
	easyflash_init();
	/* letter_shell3.1 init */
	userShellInit();
}
