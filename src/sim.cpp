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
  int cnt = 0;
  for(;;){// into loop, each loop means one tick of the clock
    unsigned int status = cpu->step();
    
    cpu->printReg();
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
    if(cnt>1){
      cnt --;
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