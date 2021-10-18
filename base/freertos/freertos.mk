C_INC += \
-I$(FREERTOS_DIR)/inc

C_SRC += \
$(FREERTOS_DIR)/src/croutine.c \
$(FREERTOS_DIR)/src/event_groups.c \
$(FREERTOS_DIR)/src/heap_4.c \
$(FREERTOS_DIR)/src/list.c \
$(FREERTOS_DIR)/src/port.c \
$(FREERTOS_DIR)/src/queue.c \
$(FREERTOS_DIR)/src/stream_buffer.c \
$(FREERTOS_DIR)/src/tasks.c \
$(FREERTOS_DIR)/src/timers.c