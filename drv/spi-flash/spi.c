#include "spi.h"

SPI_HandleTypeDef SPI4_Handler;

//以下是SPI模块的初始化代码，配置成主机模式
//SPI口初始化
//这里针是对SPI4的初始化
//extern HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
void SPI4_Init(void)
{
    SPI4_Handler.Instance=SPI4;                         //SP4
    SPI4_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI4_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI4_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI4_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //串行同步时钟的空闲状态为高电平
    SPI4_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI4_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI4_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI4_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI4_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI4_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI4_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI4_Handler);//初始化
    
    __HAL_SPI_ENABLE(&SPI4_Handler);                    //使能SPI4
	
    SPI4_ReadWriteByte(0Xff);                           //启动传输
}

//SPI4底层驱动，时钟使能，引脚配置
//此函数会被HAL_SPI_Init()调用
//hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();       //使能GPIOE时钟
    __HAL_RCC_SPI4_CLK_ENABLE();        //使能SPI4时钟
    
    /* PE2,5,6 */
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI4;           //复用为SPI4
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为45Mhz：
void SPI4_SetSpeed(UINT8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI4_Handler);            //关闭SPI
    SPI4_Handler.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    SPI4_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI4_Handler);             //使能SPI
    
}

//SPI5 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
UINT8 SPI4_ReadWriteByte(UINT8 TxData)
{
    UINT8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI4_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}
