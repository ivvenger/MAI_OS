#include "include/manage_node.h"
#include "include/tree.h"
#include "include/n_op.h"
#include <stdio.h>


int create_result = 0;
int ping_result = 0;
int exec_result = 0;

void test_create_calc_node() {
    create_result = create_calc_node(1, -1);
    printf("create_calc_node result: %d\n", create_result);
}

void test_ping_node() {
    ping_result = ping_node(1);
    printf("ping_node result: %d\n", ping_result);
}

void test_exec() {
    exec_result = exec(1, "abracadabra", "abra");
    printf("exec result: %d\n", exec_result);
}

void test_ping_all() {
    ping_all();
}

void mock_main() {
    test_create_calc_node();
    test_ping_node();
    test_exec();
    test_ping_all();
}