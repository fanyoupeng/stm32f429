#目标文件
TARGET = stm32f429

#工具链
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

#h 文件
C_INC =  \
-Iuser  \
-Icore  \
-Ilib/inc  \
-Isys/sys \
-Isys/delay \
-Isys/usart \
-Ihardware/SDRAM

#c & asm文件
C_SRC = \

BSP_DIR = bsp
include $(BSP_DIR)/bsp.mk

BASE_DIR = base
include $(BASE_DIR)/base.mk

DRV_DIR = drv
include $(DRV_DIR)/drv.mk

APP_DIR = app
include $(APP_DIR)/app.mk

OBJS =  \
$(patsubst %.c, %.o, $(C_SRC))  \
$(patsubst %.s, %.o, $(ASM_SRC))


#编译参数
# cpu
CPU = -mcpu=cortex-m4
# fpu
FPU = -mfpu=fpv4-sp-d16
# float-abi
FLOAT-ABI = -mfloat-abi=hard
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
# AS defines
AS_DEFS = 
# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F429xx
CFLAGS = $(MCU) $(C_DEFS) $(OPT) -Wall -fdata-sections -ffunction-sections -Wformat -Wno-format-zero-length

#链接脚本
LDSCRIPT = stm32f429.ld

#链接的库
LIBS = -lc -lm -lnosys 

#链接参数
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=obj.map,--cref -Wl,--gc-sections

.PHONY:all clean

#预处理-编译-汇编-链接-转为hex

all: $(TARGET).hex
	@echo "build success!"
#4、转化为hex文件
$(TARGET).hex: $(TARGET).elf
	@$(CP) -O ihex $< $@	
#3、链接.o文件
$(TARGET).elf: $(OBJS)
	@echo "LINK" $(TARGET).elf
	@$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SZ) $@
#3、链接.o文件
%.o: %.c
	@echo "CC" $<
	@$(CC) -c $(CFLAGS) $(C_DEFS) $(C_INC) $< -o $@	
#3、链接.o文件
%.o: %.s
	@echo "AS" $<
	@$(AS) -c $(CFLAGS) $< -o $@			

#清理
clean:
	@find ./ -name "*.o"| xargs rm -f
	@find ./ -name "*.elf"| xargs rm -f
	@find ./ -name "*.map"| xargs rm -f
	@find ./ -name "*.hex" | xargs rm -f
	@echo "clean down!"
