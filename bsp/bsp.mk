C_INC += \
-I$(BSP_DIR) \
-I$(BSP_DIR)/hal-lib/inc

ASM_SRC += \
$(BSP_DIR)/startup_stm32f429xx.s

C_SRC += \
$(BSP_DIR)/stm32f4xx_it.c \
$(BSP_DIR)/system_stm32f4xx.c \
$(BSP_DIR)/bsp_init.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_cortex.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_gpio.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_spi.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_pwr.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_pwr_ex.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_rcc.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_uart.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_usart.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_dma.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_dma_ex.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_tim.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_tim_ex.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_ll_fmc.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_sram.c \
$(BSP_DIR)/hal-lib/src/stm32f4xx_hal_sdram.c
