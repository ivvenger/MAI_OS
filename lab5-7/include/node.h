#pragma once

#include <zmq.h>
#include <stdbool.h>

#define MAX_TEXT_LENGTH 1024
#define MAX_BUFFER_SIZE 1024

typedef struct {
    int id;
    pid_t pid;
    void* socket;
    char endpoint[64];
