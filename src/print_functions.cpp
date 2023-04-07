#include "../include/global.h"
#include "../include/logger.h"

void print_log_success(string command)
{

    cse4589_print_and_log("[%s:SUCCESS]\n", command.c_str());
}

void print_log_end(string command)
{

    cse4589_print_and_log("[%s:END]\n", command.c_str());
    // printf("\n");
}