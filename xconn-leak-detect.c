#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <signal.h>
#include <execinfo.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include "entry_map.h"


#define CALL_STACK_BUF_SIZE 1024

typedef Display *(*XOpenDisplay_t)(const char *);
typedef int (*XCloseDisplay_t)(Display *display);

typedef xcb_connection_t *(*xcb_connect_t)(const char *, int *);
typedef void (*xcb_disconnect_t)(xcb_connection_t *);

void get_addr2line(void *addr, char *output) {
    Dl_info info;
    dladdr(addr, &info);
    if (info.dli_sname) {
        strcpy(output, info.dli_sname);
    } else {
        strcpy(output, "unknown");
    }
}


void print_map() {
    mem_dis_map *current = get_global_map();
    printf("\n");
    if (current == NULL) {
        printf("no leaks detected.\n");
        return;
    } else {
        printf("leaks detected:\n");
    }
    while (current) {
        display_node *current_display = current->display_refs;
        printf("    open connections:\n");
        while (current_display) {
            printf("        connection: %p\n", current_display->conn_ref);
            printf("        calltrace:\n");
            char **strings = (char **)current_display->call_trace;
            for (int i = 0; i < current_display->call_trace_size; i++) {
                printf("            %s\n", strings[i]);
            }
            current_display = current_display->next;
        }
        current = current->next;
    }
}

void print_leak_info(void) {
    print_map();
}

xcb_connection_t *xcb_connect(const char *displayname, int *screenp) {
    xcb_connect_t xcb_connect_func = (xcb_connect_t)dlsym(RTLD_NEXT, "xcb_connect");

    xcb_connection_t *connection_ret = xcb_connect_func(displayname, screenp);

    printf("xcb_connect called with displayname %s, return xcb_connection_t* %p\n", displayname, connection_ret);

    void *buffer[CALL_STACK_BUF_SIZE];
    int nptrs = backtrace(buffer, CALL_STACK_BUF_SIZE);

    // skip current function which is also in the backtrace
    if (nptrs > 1) {
        void *call_address = buffer[1];
        // free in remove_display_from_memory_address
        char **strings = backtrace_symbols(buffer, nptrs);
        add_display_to_memory_address(call_address, connection_ret, strings, nptrs);
    }

    return connection_ret;
}


void xcb_disconnect(xcb_connection_t *c) {
    xcb_disconnect_t xcb_disconnect_func = (xcb_disconnect_t)dlsym(RTLD_NEXT, "xcb_disconnect");

    printf("xcb_disconnect called with connection %p\n", c);
    remove_display_from_memory_address(c);

    xcb_disconnect_func(c);
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