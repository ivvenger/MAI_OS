#pragma once

#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "tree.h"
#include "message.h"
#include "n_op.h"
#include "node.h"

int create_calc_node(int node_id, int parent_id);
int ping_node(int node_id);
int exec(int node_id, const char* text, const char* pattern);
void ping_all();
void check_nodes(TreeNode* node, int* all_available, char* unavailable_nodes);
void mark_node_unavailable(int node_id);
bool is_node_available(TreeNode* node);
void mark_children_unavailable(TreeNode* node);
void check_process_status(TreeNode* root);
int exec_recursive(TreeNode* node, int target_id, const char* text, const char* pattern);