#ifndef SYS_HELPERS_H_
#define SYS_HELPERS_H_

#ifdef __cplusplus
extern "C" {
#endif

const char* helper_getenv(const char* name, const char* fallback);
int helper_mkdir_p(const char* path_);
int helper_rm(const char mode, const char* fmt, ...);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SYS_HELPERS_H_
