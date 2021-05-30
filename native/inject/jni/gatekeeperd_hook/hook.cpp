// Hook gatekeeperd to offset the uid passed to the Gatekeeper HAL
// to avoid conflict with other images
// This is similar to what Google does with DSU images; however,
// the DSU uid offset is hard-coded, and thus useless to us.
#include <android/log.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/system_properties.h>

#define LOG_TAG "gatekeeperd_hook"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

typedef int32_t status_t;

static thread_local char prop_tmp[PROP_VALUE_MAX];

// Record if the last call to writeInterfaceToken showed we are writing
// a Parcel for the IGatekeeper interface
// Define it as thread_local so that we don't need to worry about multi-threading
static thread_local bool last_interface_token_gatekeeper_1_0 = false;

// Override functions in android::hardware::Parcel in libhwbinder / libhidlbase
// Gatekeeper HAL is HIDL-only as of now.
extern "C"
status_t _ZN7android8hardware6Parcel19writeInterfaceTokenEPKc(void *self, const char *interface) {
    // We have only the 1.0 version right now. If this changes in the future, we'll need to update too
    last_interface_token_gatekeeper_1_0 = (strcmp(interface, "android.hardware.gatekeeper@1.0::IGatekeeper") == 0);
    
    typedef status_t (*writeInterfaceToken_t)(void*, const char*);
    writeInterfaceToken_t orig_pointer = (writeInterfaceToken_t) dlsym(RTLD_NEXT, "_ZN7android8hardware6Parcel19writeInterfaceTokenEPKc");
    return orig_pointer(self, interface);
}

extern "C"
status_t _ZN7android8hardware6Parcel11writeUint32Ej(void *self, uint32_t val) {
    if (last_interface_token_gatekeeper_1_0) {
        LOGI("writeUint32: we are in android.hardware.gatekeeper@1.0::IGatekeeper!");
        // The first call to writeUint32 in a IGatekeeper 1.0 Parcel is always `uid`
        // Cancel the offset set by gatekeeperd by default first
        // This happens because Paralloid forces gatekeeperd to think that we are booting
        // a DSU image by replacing the DSU property
        if (val >= 1000000) {
            val -= 1000000;
        }
        LOGI("writeUint32 uid val: %u", val);
        // We apply an offset based on a property passed from Paralloid
        if (__system_property_get("ro.gsid.paralloid_rom_id", prop_tmp)) {
            uint32_t offset = atoi(prop_tmp) * 1000000;
            uint32_t new_val = val + offset;
            LOGI("writeUint32 uid: %u -> %u", val, new_val);
            val = new_val;
        }
        // Reset the flag -- we only want to hook the first argument
        last_interface_token_gatekeeper_1_0 = false;
    }
    
    typedef status_t (*writeUint32_t)(void*, uint32_t);
    writeUint32_t orig_pointer = (writeUint32_t) dlsym(RTLD_NEXT, "_ZN7android8hardware6Parcel11writeUint32Ej");
    return orig_pointer(self, val);
}
