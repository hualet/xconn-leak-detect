#include "entry_map.h"

entry_map *map = NULL;

inline entry_map* get_global_map() {
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
        entry_map *previous = NULL;
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
            previous = current;
            current = current->next;
        }
        previous->next = (entry_map*)malloc(sizeof(entry_map));
        previous->next->memory_address = memory_address;
        previous->next->displays = (display_node*)malloc(sizeof(display_node));
        previous->next->displays->display = display;
        previous->next->displays->next = NULL;
        previous->next->next = NULL;
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