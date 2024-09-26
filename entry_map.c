#include "entry_map.h"

mem_dis_map *map = NULL;

inline mem_dis_map* get_global_map() {
    return map;
}

void add_display_to_memory_address(void *memory_address, xcb_connection_t *conn_ref, char **call_trace, int call_trace_size) {
    if (map == NULL) {
        map = (mem_dis_map*)malloc(sizeof(mem_dis_map));
        map->memory_address = memory_address;
        map->display_refs = (display_node*)malloc(sizeof(display_node));
        map->display_refs->conn_ref = conn_ref;
        map->display_refs->call_trace = call_trace;
        map->display_refs->call_trace_size = call_trace_size;
        map->display_refs->next = NULL;
        map->next = NULL;
    } else {
        mem_dis_map *current = map;
        while (current->next) {
            if (current->memory_address == memory_address) {
                display_node *current_display = current->display_refs;
                while (current_display->next) {
                    if (current_display->conn_ref == conn_ref) {
                        return;
                    }
                    current_display = current_display->next;
                }
                current_display->next = (display_node*)malloc(sizeof(display_node));
                current_display->next->conn_ref = conn_ref;
                current_display->next->call_trace = call_trace;
                current_display->next->call_trace_size = call_trace_size;
                current_display->next->next = NULL;
                return;
            }
            current = current->next;
        }
        current->next = (mem_dis_map*)malloc(sizeof(mem_dis_map));
        current->next->memory_address = memory_address;
        current->next->display_refs = (display_node*)malloc(sizeof(display_node));
        current->next->display_refs->conn_ref = conn_ref;
        current->next->display_refs->call_trace = call_trace;
        current->next->display_refs->call_trace_size = call_trace_size;
        current->next->display_refs->next = NULL;
        current->next->next = NULL;
    }
}

void remove_display_from_memory_address(xcb_connection_t *conn_ref) {
    if (map == NULL) {
        return;
    }
    mem_dis_map *current = map;
    mem_dis_map *previous = NULL;
    while (current) {
        display_node *current_display = current->display_refs;
        display_node *previous_display = NULL;
        while (current_display) {
            if (current_display->conn_ref == conn_ref) {
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