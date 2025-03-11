#pragma once

#include <zmq.h>
#include <string.h>
#include "tree.h"
#include "calc_node.h"

#define MAX_BUFFER_SIZE 1024
#define CMD_CREATE "CREATE"
#define CMD_EXEC "EXEC"
#define CMD_PING "PING"

typedef struct {
    char command[16];
    int source_id;
    int target_id;
    char data[MAX_BUFFER_SIZE];
    unsigned long request_id;
} Message;


int send_message(void* socket, Message* msg);
int receive_message(void* socket, Message* msg);