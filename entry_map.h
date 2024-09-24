
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
    Display *display;
    struct display_node *next;
} display_node;

typedef struct entry_map {
    void *memory_address;
    display_node *displays;
    struct entry_map *next;
} entry_map;

entry_map* get_global_map();

void add_display_to_memory_address(void *memory_address, Display *display);

void remove_display_from_memory_address(Display *display);

#endif