C_INC += \
-I$(EASY_FLASH)/easyflash/inc \

C_SRC += \
$(EASY_FLASH)/easyflash/src/easyflash.c \
$(EASY_FLASH)/easyflash/src/ef_utils.c \
$(EASY_FLASH)/easyflash/src/ef_env.c \
$(EASY_FLASH)/easyflash/port/ef_port.c