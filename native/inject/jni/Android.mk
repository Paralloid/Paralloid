LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := hooked_entry

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/common/include

LOCAL_SRC_FILES := \
    common/entry.cpp

LOCAL_LDLIBS := -llog
    
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libgatekeeperd_hook

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/common/include

LOCAL_SRC_FILES := \
    gatekeeperd_hook/hook.cpp

LOCAL_LDLIBS := -llog
    
include $(BUILD_SHARED_LIBRARY)
