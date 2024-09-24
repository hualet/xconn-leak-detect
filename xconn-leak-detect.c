#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <dlfcn.h>
#include <X11/Xlib.h>


typedef Display *(*XOpenDisplay_t)(const char *);
typedef int (*XCloseDisplay_t)(Display *display);


static unsigned int xOpenDisplayCount = 0;
static unsigned int xCloseDisplayCount = 0;

void add_xopendisplay_count(pid_t pid) {
    xOpenDisplayCount++;
}

void add_xclosedisplay_count(pid_t pid) {
    xCloseDisplayCount++;
}

void print_process_info(void) {
    printf("Process %d: xOpenDisplayCount=%d, xCloseDisplayCount=%d\n",
            getpid(),
            xOpenDisplayCount,
            xCloseDisplayCount);
}

Display *XOpenDisplay(const char *name) {
    XOpenDisplay_t XOpenDisplay_func = (XOpenDisplay_t)dlsym(RTLD_NEXT, "XOpenDisplay");

    printf("XOpenDisplay called with name %s\n", name);
    add_xopendisplay_count(getpid());

    return XOpenDisplay_func(name);
}

int XCloseDisplay(Display *display) {
    XCloseDisplay_t XCloseDisplay_func = (XCloseDisplay_t)dlsym(RTLD_NEXT, "XCloseDisplay");

    printf("XCloseDisplay called with display %p\n", display);
    add_xclosedisplay_count(getpid());

    return XCloseDisplay_func(display);
}

__attribute__((constructor))
void on_load(void) {
    atexit(print_process_info);
}