C_INC += \
-I$(CMBACK_TRACE_DIR)/cm_backtrace

ASM_SRC += \
$(CMBACK_TRACE_DIR)/cm_backtrace/fault_handler/gcc/cmb_fault.S

C_SRC += \
$(CMBACK_TRACE_DIR)/cm_backtrace/cm_backtrace.c
