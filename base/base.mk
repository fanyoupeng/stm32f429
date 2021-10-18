FREERTOS_DIR = $(BASE_DIR)/freertos
include $(FREERTOS_DIR)/freertos.mk

CMBACK_TRACE_DIR = $(BASE_DIR)/cmBacktrace
include $(CMBACK_TRACE_DIR)/cmBackTrace.mk

SFUD_DIR = $(BASE_DIR)/sfud
include $(SFUD_DIR)/sfud.mk

EASY_FLASH = $(BASE_DIR)/easyFlash
include $(EASY_FLASH)/easyFlash.mk

LETTER_SHELL_DIR = $(BASE_DIR)/letterShell
include $(LETTER_SHELL_DIR)/letterShell.mk
