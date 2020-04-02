/* sim.cpp - the simulation of a RISCV cpp
 * Author: Ye Zilingfeng
 */

#include "macro.h"
#include"cpu.h"
#include"loader.h"



bool single_step = true;
bool print_log = true;
bool verbose = true;

int main(int argc, char ** argv){
  // Get cmd arguments
  // get file name from cmd args.
  FILE* elf_file=fopen(argv[1],"r");
  if(elf_file==NULL){
    printf("Failed to open elf file\n");
    return 0;
  }


  // set up cpu
  CPU * cpu = new CPU();

  if(loadELF(cpu,elf_file) == ERROR){
    printf("Load ELF file error!\n");
    return 0;
  };
  int cnt = 1;
  sscanf(argv[2],"%d",&cnt);
  #define DEBUG_ROUND 0x20000
  for(int i = 0 ;i < DEBUG_ROUND;++i){// into loop, each loop means one tick of the clock
  //for(;;){
    printf("CPU TICK: %d\n",i);
    int status = cpu->pl_step();
    
    cpu->printPL();
    cpu->printReg();
    if(i == DEBUG_ROUND-1) printf("End of debug\n");
    if(status==ERROR){
      printf("CPU error!\n");
     // cpu->printReg();
      break;
    }else if(status==HALT){
      printf("CPU halts. Simulation finished!\n");
      // print stats
      break;
    }
    if(verbose){
      //cpu->print_status();
    }
    if(cnt>0){
      if(single_step){
        char op;
        if(scanf("%c",&op)==1){
          if(op=='n'){
            scanf("%d",&cnt);
          }
          if(op=='p'){
          // scanf("%c",&op);
            //if(op == 'r'){
              int regnum=0;
              scanf("%d",&regnum);
              printf("reg[%d]=%ld \n",regnum,cpu->regfile->greg[regnum]);

            //}
          }
          if(op == 'r'){
          }
          if(op=='c'){
            break;
          }
        };
      }
    }
  }
  return 0;
}