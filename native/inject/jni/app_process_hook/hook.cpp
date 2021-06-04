#define LOG_TAG "app_process_hook"
#include <inject/common.h>

#include <iostream>
#include <fstream>
#include <string>
#include <type_traits>

// Shim for sp<T> (strong pointer) in libutils
// This is only here to make the calling ABI the same as in libhidlbase
class sp_fake {
private:
    void* m_ptr;
public:
    sp_fake(void *ptr) : m_ptr(ptr) {};
    // Use a deconstructor declaration to force sp_fake to be an incomplete UDT (User-Defined Type)
    // As per C++ ABI, this should stop it from being passed via a register
    // and thus keeping it consistent with the interface from libutils
    ~sp_fake();
};

// Assert that sp_fake must not be a "trivial" type; this ensures it can never be returned in a register
static_assert(!std::is_trivial<sp_fake>::value);
// sp_fake must be standard layout
static_assert(std::is_standard_layout<sp_fake>::value);

// android::hardware::details::getRawServiceInternal
extern "C" __attribute__ ((optnone))
sp_fake _ZN7android8hardware7details21getRawServiceInternalERKNSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEESA_bb(
        const std::string &descriptor, const std::string &instance, bool retry, bool getStub) {
    if (descriptor == "android.hardware.weaver@1.0::IWeaver") {
        LOGI("Blocking Weaver HAL %s/%s", descriptor.c_str(), instance.c_str());
        return sp_fake(nullptr);
    }
    
    return call_original(sp_fake, descriptor, instance, retry, getStub);
}
