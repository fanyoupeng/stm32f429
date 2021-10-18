C_INC += \
-I$(DRV_DIR)/spi-flash

C_SRC += \
$(DRV_DIR)/sdram/sdram.c \
$(DRV_DIR)/spi-flash/spi.c \
$(DRV_DIR)/spi-flash/w25qxx.c
