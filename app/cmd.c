#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "sfud_def.h"
#include "sfud.h"

/* 重启命令 */
extern void HAL_NVIC_SystemReset(void);


static void mcu_reboot(int argc, char *argv[])
{
	printf("reboot...\r\n");
	HAL_NVIC_SystemReset();
}


SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(
			 SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, \
                 reboot, mcu_reboot, mcu reboot);

/* 查看任务状态命令 */
extern void *pvPortMalloc(size_t xWantedSize);


static void task_state(int argc, char *argv[])
{
	char *info_buffer = pvPortMalloc(1024);

	memset(info_buffer, 0, 1024);
	const char *pcHeader =
		"TaskName      State   Priority  Stack   #\r\n************************************************\r\n";

	strcpy(info_buffer, pcHeader);
	vTaskList(info_buffer + strlen(pcHeader));
	printf("%s\r\n", info_buffer);
	vPortFree((void *)info_buffer);
}


SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(
			 SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, \
                 taskStat, task_state, task state);

static void hexdump(const void *p, size_t size)
{
	const uint8_t *c = p;

	assert_param(p);
	printf("Dumping %u bytes from %p:\r\n", size, p);
	while (size > 0)
	{
		unsigned i;
		printf("%p: ", c);
		for (i = 0; i < 16; i++)
		{
			if (i < size) {
				printf("%02x ", c[i]);
			}else{
				printf("   ");
			}
		}
		for (i = 0; i < 16; i++)
		{
			if (i < size) {
				printf("%c",
				       c[i] >= 32 && c[i] < 127 ? c[i] : '.');
			}else{
				printf(" ");
			}
		}
		printf("\r\n");
		c += 16;
		if (size <= 16) {
			break;
		}
		size -= 16;
	}
}

int htol(char s[])
{
	UINT32 i;
	ULONG n = 0;

	if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) {
		i = 2;
	}else{
		i = 0;
	}
	for ( ;
	      (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') ||
	      (s[i] >= 'A' && s[i] <= 'Z'); ++i)
	{
		if (tolower(s[i]) > '9') {
			n = 16 * n + (10 + tolower(s[i]) - 'a');
		}else{
			n = 16 * n + (tolower(s[i]) - '0');
		}
	}
	return (n);
}


/* 打印内存内容 */
static void mem_hex_show(int argc, char *argv[])
{
	if (argc < 3) {
		printf("cmd format error!\r\n");
		return;
	}
	hexdump((void *)htol(argv[1]), htol(argv[2]));
}


SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(
			 SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, \
                 hexMem, mem_hex_show, printf mem);

/* 打印flash内容 */
extern sfud_err sfud_read(const sfud_flash *flash, uint32_t addr, size_t size, uint8_t *data);
extern sfud_err sfud_erase(const sfud_flash *flash, uint32_t addr, size_t size);
extern sfud_err sfud_write(const sfud_flash *flash, uint32_t addr, size_t size, const uint8_t *data);

static void flash_hex_show(int argc, char *argv[])
{
#define FLASH_READ_BUFFER_SIZE 0x1000
	if (argc < 3) {
		DEBUG("cmd format error!\r\n");
		return;
	}
	ULONG addr = htol(argv[1]);
	ULONG size = htol(argv[2]);
	if(size > FLASH_READ_BUFFER_SIZE) {
		DEBUG("size overflow!\r\n");
		return;
	}
	UINT8* flashBuf = (UINT8*)pvPortMalloc(FLASH_READ_BUFFER_SIZE);
	sfud_flash* pFlash =  sfud_get_device(SFUD_W25Q32_DEVICE_INDEX);
	if(pFlash == NULL || pFlash->init_ok == false) {
		DEBUG("sfud_get_device error\r\n");
		return;
	}
	sfud_read(pFlash, addr, size, flashBuf);
	hexdump((void *)flashBuf, size);
	vPortFree((void*)flashBuf);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(
			 SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN, \
                 hexFlash, flash_hex_show, printf spi flash);
