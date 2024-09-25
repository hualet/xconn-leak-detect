#include "entry_map.h"

mem_dis_map *map = NULL;

inline mem_dis_map* get_global_map() {
    return map;
}

void add_display_to_memory_address(void *memory_address, Display *display_ref, void *call_trace, int call_trace_size) {
    if (map == NULL) {
        map = (mem_dis_map*)malloc(sizeof(mem_dis_map));
        map->memory_address = memory_address;
        map->display_refs = (display_node*)malloc(sizeof(display_node));
        map->display_refs->display_ref = display_ref;
        map->display_refs->call_trace = call_trace;
        map->display_refs->call_trace_size = call_trace_size;
        map->display_refs->next = NULL;
        map->next = NULL;
    } else {
        mem_dis_map *current = map;
        mem_dis_map *previous = NULL;
        while (current) {
            if (current->memory_address == memory_address) {
                display_node *current_display = current->display_refs;
                while (current_display) {
                    if (current_display->display_ref == display_ref) {
                        return;
                    }
                    current_display = current_display->next;
                }
                current_display->next = (display_node*)malloc(sizeof(display_node));
                current_display->next->display_ref = display_ref;
                current_display->next->call_trace = call_trace;
                current_display->next->call_trace_size = call_trace_size;
                current_display->next->next = NULL;
                return;
            }
            previous = current;
            current = current->next;
        }
        previous->next = (mem_dis_map*)malloc(sizeof(mem_dis_map));
        previous->next->memory_address = memory_address;
        previous->next->display_refs = (display_node*)malloc(sizeof(display_node));
        previous->next->display_refs->display_ref = display_ref;
        previous->next->display_refs->call_trace = call_trace;
        previous->next->display_refs->call_trace_size = call_trace_size;
        previous->next->display_refs->next = NULL;
        previous->next->next = NULL;
    }
}

void remove_display_from_memory_address(Display *display_ref) {
    if (map == NULL) {
        return;
    }
    mem_dis_map *current = map;
    mem_dis_map *previous = NULL;
    while (current) {
        display_node *current_display = current->display_refs;
        display_node *previous_display = NULL;
        while (current_display) {
            if (current_display->display_ref == display_ref) {
                if (previous_display == NULL) {
                    current->display_refs = current_display->next;
                } else {
                    previous_display->next = current_display->next;
                }
                free(current_display->call_trace);
                free(current_display);
                if (current->display_refs == NULL) {
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