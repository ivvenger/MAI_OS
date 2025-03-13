#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode {
    int id;
    pid_t pid;
    char endpoint[64];
    bool available;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

extern TreeNode* root;

TreeNode* create_node(int node_id, pid_t process_id, const char* endpoint);
TreeNode* insert_node(TreeNode* root, int node_id, pid_t process_id, const char* endpoint);
TreeNode* find_node(TreeNode* root, int node_id);
bool is_node_exists(TreeNode* root, int node_id);
void kill_tree(TreeNode* root);
void free_tree(TreeNode* root);
void print_tree_recursive(TreeNode* node, int level);
void print_tree(TreeNode* root);
TreeNode* find_node_by_pid(TreeNode* root, pid_t pid);
int determine_child_to_forward(TreeNode* root, int current_id, int target_id);
int find_path(TreeNode* root, int current_id, int target_id, int* path);