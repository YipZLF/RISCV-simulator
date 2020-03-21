/* cpu.h - the simulated RISCV cpu
 * Author: Ye Zilingfeng
 */
#ifndef MACRO_H
#define MACRO_H
  #include "macro.h"
#endif

#include "memory.h"

struct RegFile{
  uint64_t greg[REG_CNT];

  void RegOp(){};
};

struct ALU{
  void ALUOp(){};
};

class CPU{
  // Register files
  ALU* alu;
  unsigned int cpu_status,clocks_to_finish;
  uint64_t  pc;

  void IF();
  void ID();
  void EX();
  void MEM();
  void WB();
/*
  void pipeline_step();
*/
public:
  // MEM
  Memory * memory;
  RegFile* regfile;
  CPU(){
    regfile = new RegFile;
    alu = new ALU;
    memory = new Memory;
    memory->resetMemory();
    pc = 0;
    
  }
  ~CPU(){
    delete regfile;
    delete alu;
    delete memory;
  }
  uint64_t setEntry(uint64_t addr){return pc = addr;}
  uint64_t setStackPtr(uint64_t addr){return regfile->greg[SP] = addr;}
  int step();

  #ifdef DEBUG
  public:
    void printReg(){
      for(int i = 0 ;i < REG_CNT ;i+=4){
        printf(" x%d: 0x%lx\t x%d: 0x%lx\t x%d: 0x%lx\t x%d: 0x%lx\n",
        i,regfile->greg[i],i+1,regfile->greg[i+1],i+2,regfile->greg[i+2],i+3,regfile->greg[i+3]);
      }
    }
  #endif
};