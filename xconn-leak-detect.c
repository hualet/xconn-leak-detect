#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <signal.h>
#include <execinfo.h>
#include <X11/Xlib.h>

#include "entry_map.h"


typedef Display *(*XOpenDisplay_t)(const char *);
typedef int (*XCloseDisplay_t)(Display *display);

void print_call_stack(void) {
    void *buffer[1024];
    int nptrs = backtrace(buffer, 1024);
    char **strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    // call stack example:
    // ./libxconn_leak_detect.so(print_call_stack+0x1f) [0x7fe89a73f21a]
    // ./libxconn_leak_detect.so(XOpenDisplay+0x46) [0x7fe89a73f327]
    // glxgears(+0x30f2) [0x55858f3bd0f2]
    // /lib/x86_64-linux-gnu/libc.so.6(+0x27b8a) [0x7fe89a2bcb8a]
    // /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x85) [0x7fe89a2bcc45]
    // glxgears(+0x3161) [0x55858f3bd161]

    Dl_info info;
    for (int i = 0; i < nptrs; i++) {
        if (dladdr(buffer[i], &info)) {
            printf("Call stack --  %p:\n", buffer[i]);
            printf("File: %s\n", info.dli_fname);
            printf("Base address: %p\n", info.dli_fbase);
            printf("Nearest symbol: %s\n", info.dli_sname);
            printf("Symbol address: %p\n", info.dli_saddr);
        }
    }


    printf("Call stack:\n");
    for (int i = 0; i < nptrs; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
}

void *get_call_address(void) {
    void *buffer[1024];
    int nptrs = backtrace(buffer, 1024);
    // call stack example:
    //
    // ./libxconn_leak_detect.so(print_call_stack+0x1f) [0x7fe89a73f21a]
    // ./libxconn_leak_detect.so(XOpenDisplay+0x46) [0x7fe89a73f327]
    // glxgears(+0x30f2) [0x55858f3bd0f2]
    // /lib/x86_64-linux-gnu/libc.so.6(+0x27b8a) [0x7fe89a2bcb8a]
    // /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x85) [0x7fe89a2bcc45]
    // glxgears(+0x3161) [0x55858f3bd161]
    //
    // so we want to return buffer[2]

    if (nptrs < 3) {
        return NULL;
    }

    return buffer[2];
}

void get_addr2line(void *addr, char *output) {
    Dl_info info;
    dladdr(addr, &info);
    if (info.dli_sname) {
        strcpy(output, info.dli_sname);
    } else {
        strcpy(output, "Unknown");
    }
}

void print_map() {
    entry_map *current = get_global_map();
    if (current == NULL) {
        printf("no leaks detected.\n");
        return;
    } else {
        printf("leaks detected:\n");
    }
    while (current) {
        char addr2line_output[1024];
        get_addr2line(current->memory_address, addr2line_output);
        printf("  call locations: %p(%s)\n", current->memory_address, addr2line_output);
        display_node *current_display = current->displays;
        printf("    open displays:\n");
        while (current_display) {
            printf("      - display: %p\n", current_display->display);
            current_display = current_display->next;
        }
        current = current->next;
    }
}

void print_leak_info(void) {
    print_map();
}

Display *XOpenDisplay(const char *name) {
    XOpenDisplay_t XOpenDisplay_func = (XOpenDisplay_t)dlsym(RTLD_NEXT, "XOpenDisplay");

    Display *display_ret = XOpenDisplay_func(name);

    printf("XOpenDisplay called with name %s, return Display %p\n", name, display_ret);

    void *call_address = get_call_address();
    if (call_address) {
        add_display_to_memory_address(call_address, display_ret);
    }

    return display_ret;
}

int XCloseDisplay(Display *display) {
    XCloseDisplay_t XCloseDisplay_func = (XCloseDisplay_t)dlsym(RTLD_NEXT, "XCloseDisplay");

    printf("XCloseDisplay called with display %p\n", display);
    remove_display_from_memory_address(display);

    return XCloseDisplay_func(display);
}

void handle_signal(int sig) {
    printf("Received signal %d, printing leak info.\n", sig);
    print_leak_info();
    // Reset the signal handler to default and re-raise the signal
    signal(sig, SIG_DFL);
    raise(sig);
}

void setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
}

__attribute__((constructor))
void on_load(void) {
    atexit(print_leak_info);
    setup_signal_handlers();
}