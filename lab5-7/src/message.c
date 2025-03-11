#include "../include/message.h"

int send_message(void* socket, Message* msg) {
    return zmq_send(socket, msg, sizeof(Message), 0);
}

int receive_message(void* socket, Message* msg) {
    return zmq_recv(socket, msg, sizeof(Message), 0);
}