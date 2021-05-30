LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := gatekeeperd_hooked
LOCAL_SRC_FILES := \
    gatekeeperd_hook/entry.cpp
    
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libgatekeeperd_hook
LOCAL_SRC_FILES := \
    gatekeeperd_hook/hook.cpp
LOCAL_LDLIBS := -llog
    
include $(BUILD_SHARED_LIBRARY)
