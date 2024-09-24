
// AI prompt to generate the following struct
// {
//     void* memory_address : [Display display1, Display display2, ...],
//     void* memory_address : [Display display1, Display display2, ...],
//     ...
// }

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

entry_map *map = NULL;

entry_map* get_global_map() {
    return map;
}

void add_display_to_memory_address(void *memory_address, Display *display) {
    if (map == NULL) {
        map = (entry_map*)malloc(sizeof(entry_map));
        map->memory_address = memory_address;
        map->displays = (display_node*)malloc(sizeof(display_node));
        map->displays->display = display;
        map->displays->next = NULL;
        map->next = NULL;
    } else {
        entry_map *current = map;
        while (current) {
            if (current->memory_address == memory_address) {
                display_node *current_display = current->displays;
                while (current_display) {
                    if (current_display->display == display) {
                        return;
                    }
                    current_display = current_display->next;
                }
                current_display->next = (display_node*)malloc(sizeof(display_node));
                current_display->next->display = display;
                current_display->next->next = NULL;
                return;
            }
            current = current->next;
        }
        current->next = (entry_map*)malloc(sizeof(entry_map));
        current->next->memory_address = memory_address;
        current->next->displays = (display_node*)malloc(sizeof(display_node));
        current->next->displays->display = display;
        current->next->displays->next = NULL;
        current->next->next = NULL;
    }
}

void remove_display_from_memory_address(Display *display) {
    if (map == NULL) {
        return;
    }
    entry_map *current = map;
    entry_map *previous = NULL;
    while (current) {
        display_node *current_display = current->displays;
        display_node *previous_display = NULL;
        while (current_display) {
            if (current_display->display == display) {
                if (previous_display == NULL) {
                    current->displays = current_display->next;
                } else {
                    previous_display->next = current_display->next;
                }
                free(current_display);
                if (current->displays == NULL) {
                    if (previous == NULL) {
                        map = current->next;
                    } else {
                        previous->next = current->next;
                    }
                    free(current);
                }
                return;
            }
            previous_display = current_display;
            current_display = current_display->next;
        }
        previous = current;
        current = current->next;
    }
}