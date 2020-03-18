/* cpu.h - the simulated RISCV cpu
 * Author: Ye Zilingfeng
 */

#include<memory.h>
#include<cstdio>
#include<cstring>

#define ZERO 0 // hard-wired zero
#define RA 1 // return address
#define SP 2 // stack ptr
#define GP 3 // global ptr
#define TP 4 // thread ptr
#define T0 5 // tmp/alternate link reg
#define T1 6 // tmp
#define T2 7 // tmp
#define S0 8 //saved reg, frame pointer
#define S1 9 //saved reg
#define A0 10 //func args / return value
#define A1 11 // func args / return value
#define A2 12 //func args
#define A3 13 //func args
#define A4 14 //func args
#define A5 15 //func args
#define A6 16 //func args
#define A7 17 //func args
#define S2 18 
#define S3 19
#define S4 20
#define S5 21
#define S6 22
#define S7 23
#define S8 24
#define S9 25
#define S10 26
#define S11 27
#define T3 28
#define T4 29
#define T5 30
#define T6 31


#define REG_CNT 32
#define INSTR_SIZE 4
typedef unsigned int uint_t;
typedef unsigned long int uint64_t;
typedef long int int64_t;


#define DONE 1
#define EXECUTE 2


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
    memory->loadMemory();
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