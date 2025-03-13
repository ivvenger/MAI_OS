#include "../include/comp_utils.h"
#include "../include/manage_node.h"
#include "../include/calc_node.h"
#include "../include/tree.h"
#include "../include/upcoming_operations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* find_substring(const char* text, const char* pattern) {
    static char result[MAX_BUFFER_SIZE];
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    int found = 0;
    result[0] = '\0';
    if (pattern_len == 0) {
        strcpy(result, "-1");
        return result;
    }
    for (int i = 0; i <= text_len - pattern_len; ++i) {
        if (strncmp(&text[i], pattern, pattern_len) == 0) {
            char pos[16];
            if (found > 0) {
                strcat(result, ";");
            }
            sprintf(pos, "%d", i);
            strcat(result, pos);
            found++;
        }
    }
    if (!found) {
        strcpy(result, "-1");
    }
    return result;
}