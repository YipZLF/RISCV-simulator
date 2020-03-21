/* memory.h - the simulated RISCV memory
 * Author: Ye Zilingfeng
 */
#ifndef MACRO_H
#define MACRO_H
  #include "macro.h"
#endif

struct Memory{
  char mem[MEMORY_SIZE];
  public:
    void resetMemory(){
    /*
    * contemporary implementation here!!
    */
      memset(mem,0,sizeof(mem));
    }
};