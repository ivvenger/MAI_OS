#include "../include/manage_node.h"
#include "../include/upcoming_operations.h"
#include "../include/tree.h"
#include "zmq.h"
#include <sys/wait.h>
#include <errno.h>
#define MAX_TREE_DEPTH 1024

void check_process_status(TreeNode* root) {
    if (root == NULL) return;
    if (!root->available) {
        check_process_status(root->left);
        check_process_status(root->right);
        return;
    }

    int status;
    pid_t result = waitpid(root->pid, &status, WNOHANG);

    if (result == root->pid) {
        mark_node_unavailable(root->id);
    } else if (result == 0) {
        root->available = true;
    } else if (errno == ECHILD) {
        mark_node_unavailable(root->id);
    } else {
        perror("waitpid");
    }

    check_process_status(root->left);
    check_process_status(root->right);
}

int create_calc_node_recursive(TreeNode* node, int node_id, int parent_id) {
    if (node == NULL) {
        return -1;
    }

    if (node->id == parent_id) {
        if (node->left == NULL || node->right == NULL) {
            pid_t process_id = fork();
            if (process_id == 0) {
                char id_str[16];
                sprintf(id_str, "%d", node_id);
                execl("/home/vboxuser/OS_labs/build/LW5-7/calc_node", "calc_node", id_str, "-1", NULL);
                perror("Error starting calc node");
                exit(1);
            } else if (process_id > 0) {
                char endpoint[64];
                sprintf(endpoint, "tcp://localhost:%d", 5555 + node_id);
                insert_node(root, node_id, process_id, endpoint); // BST insertion
                printf("Ok: %d\n", process_id);
                return process_id;
            } else {
                printf("Error: Failed to create process\n");
                return -1;
            }
        } else {
            printf("Error: Parent node %d already has two children\n", parent_id);
            return -1;
        }
    }

    int result = create_calc_node_recursive(node->left, node_id, parent_id);
    if (result != -1) {
        return result;
    }

    return create_calc_node_recursive(node->right, node_id, parent_id);
}

int create_calc_node(int node_id, int parent_id) {
    if (is_node_exists(root, node_id)) {
        printf("Error: Node already exists\n");
        return -1;
    }
    if (parent_id == -1) {
        pid_t process_id = fork();
        if (process_id == 0) {
            char id_str[16];
            sprintf(id_str, "%d", node_id);
            execl("/home/vboxuser/OS_labs/build/LW5-7/calc_node", "calc_node", id_str, "-1", NULL);
            perror("Error starting calc node");
            exit(1);
        } else if (process_id > 0) {
            char endpoint[64];
            sprintf(endpoint, "tcp://localhost:%d", 5555 + node_id);
            root = insert_node(root, node_id, process_id, endpoint);
            printf("Ok: %d\n", process_id);
            return process_id;
        } else {
            printf("Error: Failed to create process\n");
            return -1;
        }
    }

    TreeNode* parent_node = find_node(root, parent_id);
    if (parent_node == NULL) {
        printf("Error: Parent node not found\n");
        return -1;
    }

    return create_calc_node_recursive(parent_node, node_id, parent_id);
}

bool is_node_available(TreeNode* node) {
    if (!node || !node->available) return false;
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REQ);
    int timeout = 1000;
    int linger = 0;
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger));

    if (zmq_connect(socket, node->endpoint) != 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return false;
    }

    Message msg = {0};
    strcpy(msg.command, CMD_PING);
    msg.source_id = 0;
    msg.target_id = node->id;
    if (send_message(socket, &msg) <= 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return false;
    }

    zmq_pollitem_t items[1];
    items[0].socket = socket;
    items[0].events = ZMQ_POLLIN;
    if (zmq_poll(items, 1, timeout) <= 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return false;
    }

    if (receive_message(socket, &msg) <= 0) {
        zmq_close(socket);
        zmq_ctx_destroy(context);
        return false;
    }

    zmq_close(socket);
    zmq_ctx_destroy(context);
    return true;
}

int ping_node(int node_id) {
    TreeNode* node = find_node(root, node_id);
    if (!node) {
        printf("Error: Node not found\n");
        return -1;
    }

    if (is_node_available(node)) {
        return 1;
    } else {
        return 0;
    }
}

void ping_all() {
    if (root == NULL) {
        printf("Ok: -1\n");
        return;
    }

    char unavailable_nodes[MAX_TEXT_LENGTH] = "";
    int all_available = 1;

    check_nodes(root, &all_available, unavailable_nodes);

    if (all_available) {
        printf("Ok: -1\n");
    } else {
        printf("Ok: %s\n", unavailable_nodes);
    }
}

void check_nodes(TreeNode* node, int* all_available, char* unavailable_nodes) {
    if (node == NULL) return;
    if (!ping_node(node->id)) {
        if (strlen(unavailable_nodes) > 0) {
            strcat(unavailable_nodes, ";");
        }
        char id_str[16];
        sprintf(id_str, "%d", node->id);
        strcat(unavailable_nodes, id_str);
        *all_available = 0;
    }

    check_nodes(node->left, all_available, unavailable_nodes);
    check_nodes(node->right, all_available, unavailable_nodes);
}

void mark_node_unavailable(int node_id) {
    TreeNode* node = find_node(root, node_id);
    if (node) {
        node->available = false;
        //printf("Debug: Node %d marked as unavailable\n", node_id);
        mark_children_unavailable(node->left);
        mark_children_unavailable(node->right);
    }
}

void mark_children_unavailable(TreeNode* node) {
    if (!node) return;
    node->available = false;
    mark_children_unavailable(node->left);
    mark_children_unavailable(node->right);
}

int exec_recursive(TreeNode* node, int target_id, const char* text, const char* pattern) {
    if (node == NULL) {
        printf("Error: Node not found\n");
        return -1;
    }

    if (node->id == target_id) {
        if (!node->available) {
            printf("Error:%d: Node is unavailable\n", node->id);
            return -1; // Узел недоступен, завершаем выполнение
        }

        void* context = zmq_ctx_new();
        void* socket = zmq_socket(context, ZMQ_REQ);
        char endpoint[64];
        sprintf(endpoint, "tcp://localhost:%d", 5555 + node->id);
        zmq_connect(socket, endpoint);

        Message msg = {0};
        strcpy(msg.command, CMD_EXEC);
        msg.source_id = 0; 
        msg.target_id = node->id;
        strcpy(msg.data, text);
        strcpy(msg.data + strlen(text) + 1, pattern);

        add_upcoming_operation(node->id, socket, context, CMD_EXEC);

        send_message(socket, &msg);
        //printf("Debug: Sent message to node %d\n", node->id);

        return 0;
    }

    int result = -1;
    if (node->left && target_id < node->id) {
        result = exec_recursive(node->left, target_id, text, pattern);
    } else if (node->right && target_id > node->id) {
        result = exec_recursive(node->right, target_id, text, pattern);
    }

    /*if (result == -1) {
        printf("Error: Target node %d not found in subtree of node %d\n", target_id, node->id);
    }*/

    return result;
}

int exec(int node_id, const char* text, const char* pattern) {
    return exec_recursive(root, node_id, text, pattern);
}