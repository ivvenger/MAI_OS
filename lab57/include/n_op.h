#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <zmq.h>
#include "message.h"


typedef struct UpcomingOperation {
    int id;
    void* socket;
    void* context;
    time_t timestamp;
    char command[16];
    char result[MAX_BUFFER_SIZE]; // Добавляем поле для результата
    struct UpcomingOperation* next;
} UpcomingOperation;

extern UpcomingOperation* upcoming_operations;

void add_upcoming_operation(int node_id, void* socket, void* context, const char* command);
void cleanup_operations(void);
void check_responses(void);