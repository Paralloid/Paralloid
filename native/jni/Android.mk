LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := minui
LOCAL_STATIC_LIBRARIES := \
    libadf \
    libbase \
    libdrm \
    libpng \
    libsync \
    
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/minui/include \
    
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES := \
    minui/events.cpp \
    minui/graphics_adf.cpp \
    minui/graphics_drm.cpp \
    minui/graphics_fbdev.cpp \
    minui/graphics.cpp \
    minui/resources.cpp \
    
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := diverter
LOCAL_STATIC_LIBRARIES := minui liblog

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \

LOCAL_SRC_FILES := \
    diverter/main.cpp \
    diverter/ui.cpp \
    diverter/menu.cpp \
    
LOCAL_LDLIBS += -lz
    
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := minfastbootd
LOCAL_STATIC_LIBRARIES := libbase

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \

LOCAL_SRC_FILES := \
    minfastbootd/main.cpp \
    minfastbootd/fastboot_device.cpp \
    minfastbootd/usb_client.cpp \
    minfastbootd/usb.cpp \
    minfastbootd/commands.cpp \
    
include $(BUILD_EXECUTABLE)

include $(LOCAL_PATH)/external/Android.mk
