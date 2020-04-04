/* stats.h
 * Statistics of the simulation.
 * Author: Ye Zilingfeng
 */

#ifndef STATS_H
#define STATS_H
#include "macro.h"

extern uint64_t clk_cnt;
extern uint64_t instr_cnt;
extern uint64_t mem_read,mem_write;
extern uint64_t b_pred_crt, b_total,b_pred_err;

void printStats();

#endif