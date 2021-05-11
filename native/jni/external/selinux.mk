LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libpcre2

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/pcre/include \
    $(LOCAL_PATH)/pcre/include_internal \

LOCAL_EXPORT_C_INCLUDES := \
    $(LOCAL_PATH)/pcre/include \

LOCAL_SRC_FILES := \
    pcre/dist2/src/pcre2_auto_possess.c \
    pcre/dist2/src/pcre2_compile.c \
    pcre/dist2/src/pcre2_config.c \
    pcre/dist2/src/pcre2_context.c \
    pcre/dist2/src/pcre2_convert.c \
    pcre/dist2/src/pcre2_dfa_match.c \
    pcre/dist2/src/pcre2_error.c \
    pcre/dist2/src/pcre2_extuni.c \
    pcre/dist2/src/pcre2_find_bracket.c \
    pcre/dist2/src/pcre2_maketables.c \
    pcre/dist2/src/pcre2_match.c \
    pcre/dist2/src/pcre2_match_data.c \
    pcre/dist2/src/pcre2_jit_compile.c \
    pcre/dist2/src/pcre2_newline.c \
    pcre/dist2/src/pcre2_ord2utf.c \
    pcre/dist2/src/pcre2_pattern_info.c \
    pcre/dist2/src/pcre2_script_run.c \
    pcre/dist2/src/pcre2_serialize.c \
    pcre/dist2/src/pcre2_string_utils.c \
    pcre/dist2/src/pcre2_study.c \
    pcre/dist2/src/pcre2_substitute.c \
    pcre/dist2/src/pcre2_substring.c \
    pcre/dist2/src/pcre2_tables.c \
    pcre/dist2/src/pcre2_ucd.c \
    pcre/dist2/src/pcre2_valid_utf.c \
    pcre/dist2/src/pcre2_xclass.c \
    pcre/dist2/src/pcre2_chartables.c \
    
LOCAL_CFLAGS := -DHAVE_CONFIG_H

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libselinux
LOCAL_STATIC_LIBRARIES := pcre2 log

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/selinux/libselinux/include \
    $(LOCAL_PATH)/selinux/libselinux/src \
    $(LOCAL_PATH)/android_system_core/include \
    
LOCAL_EXPORT_C_INCLUDES := \
    $(LOCAL_PATH)/selinux/libselinux/include \

LOCAL_SRC_FILES := \
    selinux/libselinux/src/booleans.c \
    selinux/libselinux/src/callbacks.c \
    selinux/libselinux/src/freecon.c \
    selinux/libselinux/src/label_backends_android.c \
    selinux/libselinux/src/label.c \
    selinux/libselinux/src/label_support.c \
    selinux/libselinux/src/matchpathcon.c \
    selinux/libselinux/src/setrans_client.c \
    selinux/libselinux/src/sha1.c \
    selinux/libselinux/src/selinux_config.c \
    selinux/libselinux/src/seusers.c \
    selinux/libselinux/src/label_file.c \
    selinux/libselinux/src/regex.c \
    selinux/libselinux/src/avc.c \
    selinux/libselinux/src/avc_internal.c \
    selinux/libselinux/src/avc_sidtab.c \
    selinux/libselinux/src/canonicalize_context.c \
    selinux/libselinux/src/checkAccess.c \
    selinux/libselinux/src/check_context.c \
    selinux/libselinux/src/compute_av.c \
    selinux/libselinux/src/compute_create.c \
    selinux/libselinux/src/compute_member.c \
    selinux/libselinux/src/context.c \
    selinux/libselinux/src/deny_unknown.c \
    selinux/libselinux/src/disable.c \
    selinux/libselinux/src/enabled.c \
    selinux/libselinux/src/fgetfilecon.c \
    selinux/libselinux/src/fsetfilecon.c \
    selinux/libselinux/src/getenforce.c \
    selinux/libselinux/src/getfilecon.c \
    selinux/libselinux/src/get_initial_context.c \
    selinux/libselinux/src/getpeercon.c \
    selinux/libselinux/src/init.c \
    selinux/libselinux/src/lgetfilecon.c \
    selinux/libselinux/src/load_policy.c \
    selinux/libselinux/src/lsetfilecon.c \
    selinux/libselinux/src/mapping.c \
    selinux/libselinux/src/policyvers.c \
    selinux/libselinux/src/procattr.c \
    selinux/libselinux/src/reject_unknown.c \
    selinux/libselinux/src/sestatus.c \
    selinux/libselinux/src/setenforce.c \
    selinux/libselinux/src/setfilecon.c \
    selinux/libselinux/src/stringrep.c \
    
LOCAL_CFLAGS := \
    -DNO_PERSISTENTLY_STORED_PATTERNS \
    -DDISABLE_SETRANS \
    -DDISABLE_BOOL \
    -D_GNU_SOURCE \
    -DNO_MEDIA_BACKEND \
    -DNO_X_BACKEND \
    -DNO_DB_BACKEND \
    -DUSE_PCRE2 \
    -DAUDITD_LOG_TAG=1003 \
    
include $(BUILD_STATIC_LIBRARY) 
