/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "shell.h"
#include "stm32f4xx_hal.h"
#include "log.h"
#include "shell_cfg.h"

Shell shell;
char shellBuffer[512];

static SemaphoreHandle_t shellMutex;

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
extern int _write(int fd, char * pBuffer, int size);
short userShellWrite(char *data, unsigned short len)
{
	_write(1, data, len);
	return len;
}

/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
QueueHandle_t xRxedChars;
short userShellRead(char *data, unsigned short len)
{
	if( xQueueReceive( xRxedChars, (void*)data, portMAX_DELAY ) ) {
		return 1;
	}
	else
		return 0;
}

/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
	xRxedChars = xQueueCreate( SHELL_QUEUE_LENGTH, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
    if (xTaskCreate(shellTask, "shell", 256, &shell, 5, NULL) != pdPASS)
    {
        logError("shell task creat failed");
    }
}

