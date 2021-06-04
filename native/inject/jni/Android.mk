LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libgatekeeperd_hook

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/common/include

LOCAL_SRC_FILES := \
    gatekeeperd_hook/hook.cpp

LOCAL_LDLIBS := -llog
    
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := app_process_wrapper

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/common/include

LOCAL_SRC_FILES := \
    app_process_hook/wrapper.cpp

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libapp_process_hook

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/common/include

LOCAL_SRC_FILES := \
    app_process_hook/hook.cpp

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
