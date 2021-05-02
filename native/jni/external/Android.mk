LOCAL_PATH := $(call my-dir)

# Libraries from the AOSP source tree
# These are required by minui, but note that some functionalities
# MAY NOT WORK independently; they are here to satisfy the need for
# headers / symbols of utility functions but everything may not
# actually be called. Having them here at least reduces our deviation
# from upstream minui.

include $(CLEAR_VARS)
LOCAL_MODULE := libbase
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android_system_core/base/include \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    android_system_core/base/abi_compatibility.cpp \
    android_system_core/base/chrono_utils.cpp \
    android_system_core/base/cmsg.cpp \
    android_system_core/base/file.cpp \
    android_system_core/base/liblog_symbols.cpp \
    android_system_core/base/logging.cpp \
    android_system_core/base/mapped_file.cpp \
    android_system_core/base/parsebool.cpp \
    android_system_core/base/parsenetaddress.cpp \
    android_system_core/base/process.cpp \
    android_system_core/base/properties.cpp \
    android_system_core/base/stringprintf.cpp \
    android_system_core/base/strings.cpp \
    android_system_core/base/threads.cpp \

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := liblog
LOCAL_STATIC_LIBRARIES := libbase
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android_system_core/include \
    $(LOCAL_PATH)/android_system_core/liblog/include \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    android_system_core/liblog/log_event_list.cpp \
    android_system_core/liblog/log_event_write.cpp \
    android_system_core/liblog/logger_name.cpp \
    android_system_core/liblog/logger_read.cpp \
    android_system_core/liblog/logger_write.cpp \
    android_system_core/liblog/logprint.cpp \
    android_system_core/liblog/properties.cpp \
    android_system_core/liblog/event_tag_map.cpp \
    android_system_core/liblog/log_time.cpp \
    android_system_core/liblog/pmsg_reader.cpp \
    android_system_core/liblog/pmsg_writer.cpp \
    android_system_core/liblog/logd_reader.cpp \
    android_system_core/liblog/logd_writer.cpp \
    
LOCAL_CPPFLAGS := -DLIBLOG_LOG_TAG=1006 -DSNET_EVENT_LOG_TAG=1397638484
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libadf
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android_system_core/deprecated-adf/libadf/include \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
    
LOCAL_SRC_FILES := \
    android_system_core/deprecated-adf/libadf/adf.cpp \
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libsync
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android_system_core/libsync/include \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    android_system_core/libsync/sync.c \
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libdrm
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/libdrm/include/drm \
    
LOCAL_SRC_FILES := \
    libdrm/xf86drm.c \
    libdrm/xf86drmHash.c \
    libdrm/xf86drmRandom.c \
    libdrm/xf86drmSL.c \
    libdrm/xf86drmMode.c \
    
LOCAL_EXPORT_C_INCLUDES := \
    $(LOCAL_C_INCLUDES) \
    $(LOCAL_PATH)/libdrm \

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libpng

LOCAL_SRC_FILES := \
    $(wildcard $(LOCAL_PATH)/libpng/*.c) \
    $(wildcard $(LOCAL_PATH)/libpng/arm/*.c) \
    
# Strip $(LOCAL_PATH) from the wildcarded src files list
LOCAL_SRC_FILES := \
    $(foreach file, $(LOCAL_SRC_FILES), $(file:$(LOCAL_PATH)/%=%))

LOCAL_EXPORT_C_INCLUDES := \
    $(LOCAL_PATH)/libpng
    
include $(BUILD_STATIC_LIBRARY)
