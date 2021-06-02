#include <android/log.h>
#include <dlfcn.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Calls the original symbol with the same name as the function (RTLD_NEXT)
#define call_original(ret_type, args...) _call_original<ret_type>(__func__, args)

// Actual implementation of call_original using a template -- it is impossible
// to implement it with just macros because C++ mandates argument list in function pointers
template<typename Ret, typename ... Args>
Ret _call_original(const char *func_name, Args... args) {
    typedef Ret (*_orig_f_t)(Args...);
    _orig_f_t orig_ptr = (_orig_f_t) dlsym(RTLD_NEXT, func_name);
    return orig_ptr(args...);
}
