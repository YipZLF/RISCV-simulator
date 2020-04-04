/* sim.cpp - the simulation of a RISCV cpp
 * Author: Ye Zilingfeng
 */

#include "macro.h"
#include"cpu.h"
#include"loader.h"



bool single_step = false;
bool use_pl = false;
bool verbose = false;

void printHelp(){
  printf("RISC-V CPU simulation program.\n");
  printf("Author: Ye Zilingfeng, Peking Uni.\n");
  printf("Usage: sim [args] filename\n");
  printf("Supported files are RISCV ELF executables, compiled with myclib(NOT Standard C lib!).\n");
  printf("Supported arguments are:\n");
  printf(" -h,-?:  print this help information\n");
  printf(" -s:  single step mode, the program pauses at every step(default: single step disabled)\n");
  printf(" -v:  verbose mode, print register and pipeline information\n");
  printf(" -p:  pipeline implementation(default: pipeline disabled)\n");
}

int main(int argc, char ** argv){
  FILE* elf_file;

  // Get cmd arguments
  if(argc <= 1){
    printHelp();
    return 0;
  }
  for(int i = 1; i<argc;++i){
    if(argv[i][0]=='-'){
      argv[i]++;
      for(;*argv[i] ; ++argv[i]){
        if(*argv[i]=='h' || *argv[i]=='?'){
          printHelp();
          return 0;
        }else if(*argv[i]=='s'){
          single_step = true;
        }else if(*argv[i]=='v'){
          verbose = true;
        }else if(*argv[i]=='p'){
          use_pl =true;
        }else{
          printf("ERROR: Invalid arguments.\n");
          printHelp();
          return 0;
        }
      }
    }else{
      elf_file=fopen(argv[i],"r");
      if(elf_file==NULL){
        printf("ERROR: Invalid filename.\n");
        printHelp();
        return 0;
      }
    }
  }
  

  // set up cpu
  CPU * cpu = new CPU(verbose);

  if(loadELF(cpu,elf_file) == ERROR){
    printf("Load ELF file error!\n");
    return 0;
  };
  for(;;){
    int status;
    if(use_pl)
      status = cpu->pl_step();
    else
      status = cpu->step();
    
    if(verbose){//print regs and pipeline states
      if(use_pl)
        cpu->printPL();
      cpu->printReg();
    }
    if(status==ERROR){
      printf("CPU error!\n");
      break;
    }else if(status==HALT){
      printf("\n-----------------------------------");
      printf("\n| CPU halts. Simulation finished! |");
      printf("\n-----------------------------------\n");
      break;
    }
    if(single_step){
      char c[10];
      while(true){
        scanf("%c",&c[0]);
        if(c[0]=='?'||c[0]=='h'){//help
          printf("Single step mode:\n");
          printf(" h, ?: print this help information\n");
          printf(" x: show memory\n   - usage: x [address in HEX]\n");
          printf(" r: show register\n   - usage: r [reg index]\n");
          printf(" p: show pc value\n   - usage: p\n");
          printf(" s: show statistics so far\n  - usage: s\n");
          printf(" c: continue single step\n");
          printf(" q: quit single step. run without stopping.\n");
        }else if(c[0]=='x'){
          uint64_t addr;
          scanf("%lx",&addr);
          printf("%lx\n",addr);
          printf(" 0x%lx : %lx\n",addr, *(uint64_t *)&cpu->memory->mem[addr]);
        }else if(c[0]== 'r'){
          int reg;
          scanf("%d",&reg);
          printf(" reg x%d : %lx\n",reg, *(uint64_t *)&cpu->regfile->greg[reg]);
        }else if(c[0]== 'p'){
          printf(" PC: %lx\n", cpu->getPC());
        }else if(c[0]== 's'){
          printStats();
        }else if(c[0]=='q'){
          single_step = false;
          break;
        }else{
          break;
        }
      }
    }
  }
  if(use_pl)
    printStats();
  return 0;
}