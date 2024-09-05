#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <X11/Xlib.h>

#define MAX_PROCESS_INFO 256

typedef Display *(*XOpenDisplay_t)(const char *);
typedef int (*XCloseDisplay_t)(Display *display);

// 定义一个动态array数据结构
typedef struct {
    pid_t pid;
    int xOpenDisplayCount;
    int xCloseDisplayCount;
} process_info;

process_info *process_infos = NULL;

void add_xopendisplay_count(pid_t pid) {
    if (process_infos == NULL) {
        process_infos = calloc(MAX_PROCESS_INFO, sizeof(process_info));
    }

    int index = 0;
    bool found = false;
    for (; index < sizeof(process_infos); index++) {
        if (!process_infos[index].pid) {
            break;
        }
        if (process_infos[index].pid == pid) {
            process_infos[index].xOpenDisplayCount++;
            found = true;
            break;
        }
    }
    if (!found) {
        process_info info = (process_info){
            .pid = pid,
            .xOpenDisplayCount = 1,
            .xCloseDisplayCount = 0
        };
        process_infos[index] = info;
    }
}

void add_xclosedisplay_count(pid_t pid) {
    if (process_infos == NULL) {
        process_infos = calloc(MAX_PROCESS_INFO, sizeof(process_info));
    }

    int index = 0;
    bool found = false;
    for (; index < sizeof(process_infos); index++) {
        if (!process_infos[index].pid) {
            break;
        }
        if (process_infos[index].pid == pid) {
            process_infos[index].xCloseDisplayCount++;
            found = true;
            break;
        }
    }
    if (!found) {
        process_info info = (process_info){
            .pid = pid,
            .xOpenDisplayCount = 0,
            .xCloseDisplayCount = 1
        };
        process_infos[index] = info;
    }
}

void print_process_info() {
    if (process_infos != NULL) {
        for (int i = 0; i < sizeof(process_infos); i++) {
            printf("Process %d: xOpenDisplayCount=%d, xCloseDisplayCount=%d\n",
                    process_infos[i].pid,
                    process_infos[i].xOpenDisplayCount,
                    process_infos[i].xCloseDisplayCount);
        }
    }
}

Display *XOpenDisplay(const char *name) {
    XOpenDisplay_t XOpenDisplay_func = (XOpenDisplay_t)dlsym(RTLD_NEXT, "XOpenDisplay");

    printf("XOpenDisplay called with name %s\n", name);
    add_xopendisplay_count(getpid());
    print_process_info();

    return XOpenDisplay_func(name);
}

int XCloseDisplay(Display *display) {
    XCloseDisplay_t XCloseDisplay_func = (XCloseDisplay_t)dlsym(RTLD_NEXT, "XCloseDisplay");

    printf("XCloseDisplay called with display %p\n", display);
    add_xclosedisplay_count(getpid());

    return XCloseDisplay_func(display);
}