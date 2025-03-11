#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "include/calc_node.h"
#include "include/manage_node.h"
#include "include/tree.h"
#include "include/message.h"
#include "include/upcoming_operations.h"
#include <sys/wait.h>


#define MAX_CMD_LENGTH 1024

//TreeNode* root = NULL;
//UpcomingOperation* upcoming_operations = NULL;

void handle_signal(int signal) {
    if (root != NULL) {
        kill_tree(root);
        free_tree(root);
    }
    exit(0);
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    char command[MAX_CMD_LENGTH];
    char text[MAX_TEXT_LENGTH];
    char pattern[MAX_TEXT_LENGTH];
    int node_id, parent_id;
    pid_t process_id;

    zmq_pollitem_t poll_items[1];
    poll_items[0].socket = NULL;
    poll_items[0].fd = STDIN_FILENO;
    poll_items[0].events = ZMQ_POLLIN;

    printf("> ");
    fflush(stdout);
    while (1) {
        int poll_result = zmq_poll(poll_items, 1, 100);
        cleanup_operations();
        check_responses();
        check_process_status(root);

        if (poll_result == 0) {
            continue;
        }

        if (poll_items[0].revents & ZMQ_POLLIN) {
            if (fgets(command, MAX_CMD_LENGTH, stdin) == NULL) {
                break;
            }
            command[strcspn(command, "\n")] = 0;

            if (sscanf(command, "create %d %d", &node_id, &parent_id) == 2) {
                create_calc_node(node_id, parent_id);
            } else if (sscanf(command, "create %d", &node_id) == 1) {
                create_calc_node(node_id, -1);
            } else if (strcmp(command, "print") == 0) {
                print_tree(root);
            } else if (strcmp(command, "pingall") == 0) {
                ping_all();
            } else if (sscanf(command, "exec %d", &node_id) == 1) {
                printf("> ");
                if (fgets(text, MAX_TEXT_LENGTH, stdin) == NULL) {
                    break;
                }
                text[strcspn(text, "\n")] = 0;

                printf("> ");
                if (fgets(pattern, MAX_TEXT_LENGTH, stdin) == NULL) {
                    break;
                }
                pattern[strcspn(pattern, "\n")] = 0;

                exec(node_id, text, pattern);
            } else if (sscanf(command, "kill %d", &process_id) == 1) {
                if (kill(process_id, SIGTERM) == 0) {
                    printf("Ok: Process %d killed\n", process_id);
                    TreeNode* node = find_node_by_pid(root, process_id);
                    if (node) {
                        mark_node_unavailable(node->id);
                    } else {
                        printf("Error: Node with PID %d not found\n", process_id);
                    }
                } else {
                    printf("Error: Failed to kill process %d\n", process_id);
                }
            } else if (strcmp(command, "exit") == 0) {
                break;
            } else {
                printf("Error: Unknown command\n");
            }
            printf("> ");
            fflush(stdout);
        }
    }

    return 0;
}