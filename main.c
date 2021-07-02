#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/limits.h> // it's simple wrapper, thus the assumption those will be enough
#include <sys/types.h>
#include <unistd.h>

#include "sys_helpers.h"
#include <progwrap.h>

#define PROGNAME "MozXDG"

#define VARSION  "0.8.5"

const char* xdg_data_home;

void handleMozillaDir();
void genArgs(char* args[], int argc, char* argv[]);
void createProfileDir(const char* path);
void condExit(int f);
void tempLinking(const char* dir);
char toUpper(char x); // too simple to include another header only for it

int main(int argc, char* argv[])
{
    /* Init XDG_DATA_HOME */ {
        char fallback[PATH_MAX];
        snprintf(fallback, PATH_MAX, "%s%s", getenv("HOME"), "/.local/share");
        xdg_data_home = helper_getenv("XDG_DATA_HOME", fallback);
    }

    int p = 0; // profile position in arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--profile") == 0) {
            p = i+1 - (i == argc);
        }
    }

    pid_t pid = fork();

    condExit(pid < 0);

    if (pid) {
        if (p == 0) {
            char* args[ARG_MAX];
            genArgs(args, argc, argv);
            createProfileDir(args[2]);
            condExit(progwrap_exec(args) < 0);
        } else {
            createProfileDir(argv[p]);
            condExit(progwrap_exec(argv) < 0);
        }
    } else {
        handleMozillaDir();
    }

    return 0;
}

void condExit(int f)
{
    if (f) {
        perror(PROGNAME);
        exit(1);
    }
}

void genArgs(char* args[], int argc, char* argv[])
{
    const char* temp = getenv("MOZXDG_PROFILE");
    if (temp == NULL) {
        if (strstr(argv[0], "firefox") != NULL) {
            temp = "firefox";
        } else if (strstr(argv[0], "thunderbird") != NULL) {
            temp = "thunderbird";
        } else {
            fprintf(stderr, "%s%s%s%s",
                    PROGNAME,
                    " : Error\n",
                    " Could not determine what program is being wrapped to select profile location.\n",
                    " Please specify it via MOZXDG_PROFILE env variable or by '--profile' option\n"
            );
            exit(1);
        }
    }

    char profile[PATH_MAX];
    snprintf(profile, PATH_MAX, "%s%s%s", xdg_data_home, "/", temp);

    int n = argc + 2 + 1; // number of args: passed to executable + profile + NULL

    args[0] = strdup(argv[0]);
    args[1] = strdup("--profile");
    args[2] = strdup(profile);
    args[n-1] = NULL;

    for (int i = 1; i < argc; ++i) {
        args[i+2] = strdup(argv[i]);
    }
}

void createProfileDir(const char* path)
{
    // Create dir for profile if doesn't already exist
    if (access(path, F_OK) != 0) {
        condExit(helper_mkdir_p(path) < 0);
    }
}

void handleMozillaDir()
{
    char dir[BUFSIZ]; // "~/.mozilla"
    snprintf(dir, BUFSIZ, "%s%s", getenv("HOME"), "/.mozilla");

    char mode = helper_getenv("MOZXDG_MODE", "1")[0];

    switch (mode) {
        case '1': // temporary symlinking
            tempLinking(dir);
            break;
        case '2': // constant removing
            while (getppid() != 1) {
                sleep(1);
                if (access(dir, F_OK) == 0) {
                    helper_rm('r', "%s", dir);
                }
            }
            break;
        default:
            break;
    }
}

void tempLinking(const char* dir)
{
    char foo[PATH_MAX];
    snprintf(foo, PATH_MAX, "%s%s", xdg_data_home, "/mozilla");
    helper_mkdir_p(foo);

    // Check if symlink already exists
    char temp[PATH_MAX];
    readlink(dir, temp, PATH_MAX);
    if (strcmp(temp, foo) == 0) {
        return;
    }
    temp[0] = '\0';

    symlink(foo, dir);

    while (getppid() != 1) {
        sleep(1);
    }

    // Check again if symlink still points to correct location, then remove it
    readlink(dir, temp, PATH_MAX);
    if (strcmp(temp, foo) == 0) {
        helper_rm('r', "%s", dir);
    }
}

char toUpper(char x)
{
    if (x >= 65 && x <= 122) {
        return x-32;
    }
    return x;
}
