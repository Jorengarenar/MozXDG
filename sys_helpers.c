#include "sys_helpers.h"

#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>

#include <ftw.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

const char* helper_getenv(const char* name, const char* fallback)
{
    char* temp = getenv(name);
    return temp ? temp : fallback;
}

int helper_mkdir_p(const char* path_)
{
    errno = 0;

    if (strlen(path_) > PATH_MAX-1) {
        errno = ENAMETOOLONG;
        return -1;
    }

    char path[PATH_MAX];
    strcpy(path, path_);

    // Create "intermediate" directories
    for (char* p = path + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0'; // temporarily truncate
            if (mkdir(path, S_IRWXU) < 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }

    // Create final dir
    if (mkdir(path, S_IRWXU) < 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }

    return 0;
}

static int helper_ntfw_rm(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf)
{
    return remove(fpath);
}

static int helper_ntfw_rmdir(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf)
{
    return rmdir(fpath);
}

int helper_rm(const char mode, const char* fmt, ...)
{
    va_list vars;
    va_start(vars, fmt);
    char path[BUFSIZ];
    vsnprintf(path, BUFSIZ, fmt, vars);
    va_end(vars);

    switch (mode) {
        case 'r': // remove recursively
            return nftw(path, helper_ntfw_rm, 100, FTW_DEPTH|FTW_PHYS);
        case 'd': // remove dir
            return rmdir(path);
        case 'D': // remove empty subdirs and if emptied, then dir too
            return nftw(path, helper_ntfw_rmdir, 100, FTW_DEPTH|FTW_PHYS);
        default:  // remove file
            return unlink(path);
    }
}
