
// AI prompt to generate the following struct
// {
//     void* memory_address : [Display display1, Display display2, ...],
//     void* memory_address : [Display display1, Display display2, ...],
//     ...
// }

#ifndef ENTRY_MAP_H
#define ENTRY_MAP_H

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>


typedef struct display_node {
    Display *display_ref;
    void *call_trace;
    int call_trace_size;
    struct display_node *next;
} display_node;

typedef struct mem_dis_map {
    void *memory_address;
    display_node *display_refs;
    struct mem_dis_map *next;
} mem_dis_map;

mem_dis_map* get_global_map();

void add_display_to_memory_address(void *memory_address, Display *display_ref, void *call_trace, int call_trace_size);

void remove_display_from_memory_address(Display *display_ref);

#endif