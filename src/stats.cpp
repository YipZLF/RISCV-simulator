#include "stats.h"

uint64_t clk_cnt=0;
uint64_t instr_cnt=0;
uint64_t mem_access=0;
uint64_t b_pred_crt=0, b_total=0,b_pred_err = 0;
uint64_t str_hzds=0, data_hzds=0, ctrl_hzds=0;

void printStats(){
  printf("-----------------Statistics of Simulation--------------------\n");
  printf("Number of ..\n");
  printf(" - clocks:\t%lu\n",clk_cnt);
  printf(" - instrs:\t%lu\n",instr_cnt);
  printf(" - mem access:\t%lu\n",mem_access);
  printf(" - branch instrs:\t%lu\n",b_total);// B*, excluding jal instrs
  printf(" - correct branch prediction:\t%lu(acc: %.3lf)\n",
        b_pred_crt, ( (double)b_pred_crt/b_total ) );
  printf(" - structural hazards:\t%lu\n",str_hzds);
  printf(" - data hazards:\t%lu\n",data_hzds);
  printf(" - control hazards:\t%lu\n",ctrl_hzds);
  printf("--------------------------------------------------------------\n");
}