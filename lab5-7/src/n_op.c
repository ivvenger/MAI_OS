#include "../include/upcoming_operations.h"
#include "../include/tree.h"

UpcomingOperation* upcoming_operations = NULL;

void add_upcoming_operation(int node_id, void* socket, void* context, const char* command) {
    UpcomingOperation* op = (UpcomingOperation*)malloc(sizeof(UpcomingOperation));
    op->id = node_id;
    op->socket = socket;
    op->context = context;
    op->timestamp = time(NULL);
    strncpy(op->command, command, sizeof(op->command) - 1);
    op->command[sizeof(op->command) - 1] = '\0';
    op->next = upcoming_operations;
    upcoming_operations = op;
}

void cleanup_operations(void) {
    UpcomingOperation* current = upcoming_operations;
    UpcomingOperation* prev = NULL;
    time_t now = time(NULL);

    while (current != NULL) {
        if (now - current->timestamp > 5) {
            UpcomingOperation* to_delete = current;
            if (prev) {
                prev->next = current->next;
            } else {
                upcoming_operations = current->next;
            }
            zmq_close(to_delete->socket);
            zmq_ctx_destroy(to_delete->context);
            free(to_delete);
            if (prev) { current = prev->next; }
            else { current = upcoming_operations; }
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void check_responses(void) {
    UpcomingOperation* current = upcoming_operations;
    UpcomingOperation* prev = NULL;

    while (current != NULL) {
        TreeNode* node = find_node(root, current->id);
        if (node && !node->available) {
            if (strcmp(current->command, CMD_PING) == 0) {
                printf("Ok: 0\n");
            } else if (strcmp(current->command, CMD_EXEC) == 0) {
                printf("Error:%d: Node is unavailable\n", current->id);
            }
            if (prev) { prev->next = current->next; }
            else { upcoming_operations = current->next; }
            zmq_close(current->socket);
            zmq_ctx_destroy(current->context);
            free(current);
            current = prev ? prev->next : upcoming_operations;
            //printf("Debug: node->available = %d, @\n", node->available);
            continue;
        }

        zmq_pollitem_t items[1];
        items[0].socket = current->socket;
        items[0].events = ZMQ_POLLIN;

        int poll_result = zmq_poll(items, 1, 0);
        if (poll_result == 1) {
            Message msg = {0};
            int recv_result = receive_message(current->socket, &msg);
            if (recv_result > 0) {
                if (strcmp(current->command, CMD_PING) == 0) {
                    printf("Ok: 1\n");
                } else {
                    printf("Ok: %s\n", msg.data);
                }
                if (prev) { prev->next = current->next; }
                else { upcoming_operations = current->next; }
                zmq_close(current->socket);
                zmq_ctx_destroy(current->context);
                free(current);
                current = prev ? prev->next : upcoming_operations;
                printf("> ");
                fflush(stdout);
                continue;
            }
        }

        time_t now = time(NULL);
        if (now - current->timestamp > 5) {
            if (strcmp(current->command, CMD_PING) == 0) {
                printf("Ok: 0\n");
            } else if (strcmp(current->command, CMD_EXEC) == 0) {
                printf("Error:%d: Node is unavailable\n", current->id);
            }
            if (prev) { prev->next = current->next; }
            else { upcoming_operations = current->next; }
            zmq_close(current->socket);
            zmq_ctx_destroy(current->context);
            free(current);
            current = prev ? prev->next : upcoming_operations;
            //printf("Debug: node->available = %d, @\n", node->available);
            continue;
        }

        prev = current;
        current = current->next;
    }
}