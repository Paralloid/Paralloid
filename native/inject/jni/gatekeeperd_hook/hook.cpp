// Hook gatekeeperd to offset the uid passed to the Gatekeeper HAL
// to avoid conflict with other images
// This is similar to what Google does with DSU images; however,
// the DSU uid offset is hard-coded, and thus useless to us.
#define LOG_TAG "gatekeeperd_hook"
#include <inject/common.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/system_properties.h>
#include <unistd.h>

typedef int32_t status_t;

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
    
    return call_original(status_t, self, interface);
}

extern "C"
status_t _ZN7android8hardware6Parcel11writeUint32Ej(void *self, uint32_t val) {
    if (last_interface_token_gatekeeper_1_0) {
        LOGI("writeUint32: we are in android.hardware.gatekeeper@1.0::IGatekeeper!");
        // The first call to writeUint32 in a IGatekeeper 1.0 Parcel is always `uid`
        LOGI("writeUint32 uid val: %u", val);
        // We apply an offset based on a property passed from Paralloid
        char *_target_id;
        if ((_target_id = getenv("PARALLOID_TARGET_ID")) != nullptr) {
            uint32_t offset = atoi(_target_id) * 1000000;
            uint32_t new_val = val + offset;
            LOGI("writeUint32 uid: %u -> %u", val, new_val);
            val = new_val;
        }
        // Reset the flag -- we only want to hook the first argument
        last_interface_token_gatekeeper_1_0 = false;
    }
    
    return call_original(status_t, self, val);
}

static void create_marker_file(const char *filename) {
    int fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return;
    }
    close(fd);
}

// see mark_cold_boot() in gatekeeperd
static void mark_cold_boot() {
    create_marker_file("/data/misc/gatekeeper/.coldboot");
    // liblog does not work yet in the library constructor, so create another
    // marker to signal our existence
    create_marker_file("/data/misc/gatekeeper/.coldboot_supressed_paralloid");
}

__attribute__((constructor))
static void setup() {
    // Create the cold boot marker, so that gatekeeperd won't clear state of Gatekeeper HAL
    mark_cold_boot();
}
