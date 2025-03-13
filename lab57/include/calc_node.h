#pragma once

#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"
#include "node.h"
#include "comp_utils.h"

void start_calc_node(int id, int parent_id);