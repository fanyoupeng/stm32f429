C_INC += \
-I$(LETTER_SHELL_DIR)/src \
-I$(LETTER_SHELL_DIR)/extensions/log

C_SRC += \
$(LETTER_SHELL_DIR)/src/shell.c \
$(LETTER_SHELL_DIR)/src/shell_port.c \
$(LETTER_SHELL_DIR)/src/shell_companion.c \
$(LETTER_SHELL_DIR)/src/shell_cmd_list.c \
$(LETTER_SHELL_DIR)/src/shell_ext.c \
$(LETTER_SHELL_DIR)/extensions/log/log.c
