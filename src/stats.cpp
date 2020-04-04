#include "stats.h"

uint64_t clk_cnt=0;
uint64_t instr_cnt=0;
uint64_t mem_read=0,mem_write=0;
uint64_t b_pred_crt=0, b_total=0,b_pred_err = 0;

void printStats(){
  printf("-----------------Statistics of Simulation--------------------\n");
  printf("Number of ..\n");
  printf(" - clocks:\t%lu\n",clk_cnt);
  printf(" - instrs:\t%lu\n",instr_cnt);
  printf(" - mem read:\t%lu\n",mem_read);
  printf(" - mem write:\t%lu\n",mem_write);
  printf(" - branch instrs:\t%lu\n",b_total);// B*, excluding jal instrs
  printf(" - correct branch prediction:\t%lu(acc: %.3lf)\n",
        b_pred_crt, ( (double)b_pred_crt/b_total ) );
  printf("--------------------------------------------------------------\n");
}