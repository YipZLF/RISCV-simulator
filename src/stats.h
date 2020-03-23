/* stats.h
 * Statistics of the simulation.
 * Author: Ye Zilingfeng
 */

#ifndef STATS_H
#define STATS_H
#include "macro.h"

extern uint64_t clk_cnt;
extern uint64_t instr_cnt;
extern uint64_t mem_access;
extern uint64_t b_pred_crt, b_total,b_pred_err;
extern uint64_t str_hzds, data_hzds, ctrl_hzds;

void printStats();

#endif