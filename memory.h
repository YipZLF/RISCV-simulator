/* memory.h - the simulated RISCV memory
 * Author: Ye Zilingfeng
 */
#include<cstring>
#define MEMORY_SIZE 1<<20
struct Memory{
  char mem[MEMORY_SIZE];
  public:
    void loadMemory(){
    /*
    * contemporary implementation here!!
    */
    memset(mem,0,sizeof(mem));
    }
};