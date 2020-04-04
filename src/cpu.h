/* cpu.h - the simulated RISCV cpu
 * Author: Ye Zilingfeng
 */
#ifndef CPU_H
#define CPU_H
#include "macro.h"
#include "stats.h"
#include "memory.h"

struct RegFile{
  uint64_t greg[REG_CNT];
};

struct ALU{
  void ALUOp(){};
};

class CPU{
  // Register files
  ALU* alu;
  unsigned int cpu_status,clocks_to_finish;
  uint64_t  pc;

  bool verbose;
  int IF();
  int ID();
  int EX();
  int MEM();
  int WB();
  uint64_t branchPred(uint64_t pc_next,uint64_t pc_branch,uint64_t &pc_recover,bool &taken){
    
    //always taken
    #ifdef DEBUG
    printf("    - IF: branch prediction: taken.\n");
    #endif
    taken = true;
    pc_recover = pc_next;
    return pc_branch; 
    
/*
    //always not taken
    #ifdef DEBUG
    printf("    - IF: branch prediction: not taken.\n");
    #endif
    taken = false;
    pc_recover = pc_branch;
    return pc_next; */
  }
  void wrongPred(uint64_t pc_recover){
    _f_bub = _fd_bub = _e_bub =  _de_bub = true;
    pc = pc_recover;
  }
  void correctPred(){
    // do nothing;
  }
  bool hasPredicted(int b_op){
    return (b_op & 0x1);// b_op that needed prediction is encoded with odd number.
  }
  bool hzdDetect(int rs,int64_t & s);

/* Internal CPU states register*/
bool _fetch_bub;
uint64_t _pc_from_ecall;

bool _f_stall,_f_bub,_f_taken;
int _f_st, _f_clk,_f_rs1;
uint64_t _f_pc,_f_pc_recover;
int64_t _f_offset,_f_s1;
uint32_t _f_instr;

bool _fd_valid, _fd_stall,_fd_bub,_fd_taken;
uint64_t _fd_pc,_fd_pc_recover;
uint32_t _fd_instr;

bool _d_flush,_d_rs1_ok,_d_rs2_ok,_d_bub,_d_taken;
int _d_st, _d_clk,_d_rd,_d_alu_op,_d_b_op,_d_mem_op,_d_operand_type,_d_rs1,_d_rs2,_d_opcode;
int64_t _d_s1,_d_s2,_d_data;
uint64_t _d_pc,_d_pc_recover;
uint32_t _d_instr;

bool _de_valid,_de_stall,_de_bub,_de_taken;
int64_t _de_data,_de_s1,_de_s2;
uint64_t _de_pc_recover;
int _de_rd,_de_b_op,_de_alu_op,_de_mem_op,_de_operand_type,_de_opcode;

bool _e_flush,_e_b_check,_e_bub,_e_taken;
int _e_st, _e_clk;
int64_t _e_odata,_e_odata2,_e_idata,_e_s1,_e_s2;
uint64_t _e_pc_recover;
int _e_rd,_e_b_op,_e_alu_op,_e_mem_op,_e_operand_type,_e_opcode;

bool _em_valid,_em_stall,_em_bub;
int64_t _em_data,_em_data2;
int _em_rd,_em_mem_op,_em_operand_type,_em_opcode;

bool _m_flush,_m_bub;
int _m_st, _m_clk;
int64_t _m_idata,_m_odata;
uint64_t _m_addr;
int _m_rd,_m_mem_op,_m_operand_type,_m_opcode;

bool _mw_valid,_mw_stall,_mw_bub;
int64_t _mw_data;
int _mw_rd,_mw_opcode;

bool _w_flush,_w_bub;
int _w_st, _w_clk;
int64_t _w_data;
int _w_rd,_w_opcode;

public:
  // MEM
  Memory * memory;
  RegFile* regfile;
  CPU(bool _verbose = false){
    verbose = _verbose;
    regfile = new RegFile;
    alu = new ALU;
    memory = new Memory;
    memory->resetMemory();
    pc = 0;
    _fetch_bub = false;
    _fd_bub = _de_bub = _em_bub = _mw_bub = true;
    _f_st = _d_st = _e_st = _m_st = _w_st = READY;
    if(verbose){
      for(int i = 0; i < 8;++i)
        sprintf(instr[i],"\n");
    }
  }
  ~CPU(){
    delete regfile;
    delete alu;
    delete memory;
  }
  uint64_t setEntry(uint64_t addr){return pc = addr;}
  uint64_t setStackPtr(uint64_t addr){return regfile->greg[SP] = addr;}
  uint64_t getPC(){return pc;}
  int step();
  int pl_step();

  public:
    void printReg(){

      printf("-------Reg file stage---------\n");
      printf(" x0(zero) 0x%lx\t x1(ra): 0x%lx\t x2(sp): 0x%lx\t x3(gp): 0x%lx\n",
        regfile->greg[0],regfile->greg[1],regfile->greg[2],regfile->greg[3]);
      printf(" x4(tp) 0x%lx\t x5(t0): 0x%lx\t x6(t1): 0x%lx\t x7(t2): 0x%lx\n",
        regfile->greg[4],regfile->greg[5],regfile->greg[6],regfile->greg[7]);
      printf(" x8(s0) 0x%lx\t x9(s1): 0x%lx\t x10(a0): 0x%lx\t x11(a1): 0x%lx\n",
        regfile->greg[8],regfile->greg[9],regfile->greg[10],regfile->greg[11]);
      printf(" x12(a2) 0x%lx\t x13(a3): 0x%lx\t x14(a4): 0x%lx\t x15(a5): 0x%lx\n",
        regfile->greg[12],regfile->greg[13],regfile->greg[14],regfile->greg[15]);
      printf(" x16(a6) 0x%lx\t x17(a7): 0x%lx\t x18(s2): 0x%lx\t x19(s19): 0x%lx\n",
        regfile->greg[16],regfile->greg[17],regfile->greg[18],regfile->greg[19]);
      printf(" x20(s4) 0x%lx\t x21(s5): 0x%lx\t x22(s6): 0x%lx\t x23(s7): 0x%lx\n",
        regfile->greg[20],regfile->greg[21],regfile->greg[22],regfile->greg[23]);
      printf(" x24(s8) 0x%lx\t x25(s9): 0x%lx\t x26(s10): 0x%lx\t x27(s11): 0x%lx\n",
        regfile->greg[24],regfile->greg[25],regfile->greg[26],regfile->greg[27]);
      printf(" x28(t3) 0x%lx\t x29(t4): 0x%lx\t x30(t5): 0x%lx\t x31(t6): 0x%lx\n",
        regfile->greg[28],regfile->greg[29],regfile->greg[30],regfile->greg[31]);
      printf("------------------------------\n\n");
    }
    void printPL(){
      printf("-------Pipeline stage---------\n");
      printf(" - IF = %s: %s",pl_st[_f_st],instr[IF_D]);
      printf("  - fd [VALID=%d BUBBLE=%d STALL=%d]: pc:0x%lx, instr:0x%x\n",_fd_valid,_fd_bub,_fd_stall, _fd_pc,_fd_instr);
      printf(" - ID = %s: %s",pl_st[_d_st],instr[ID_D]);
      printf("  - de [VALID=%d BUBBLE=%d STALL=%d]: %s",_de_valid,_de_bub,_de_stall,instr[DE_D]);
      printf(" - EX = %s: %s",pl_st[_e_st],instr[EX_D]);
      printf("  - em [VALID=%d BUBBLE=%d STALL=%d]: %s",_em_valid,_em_bub,_em_stall,instr[EM_D]);
      printf(" - MEM = %s: %s",pl_st[_m_st],instr[MEM_D]);
      printf("  - mw [VALID=%d BUBBLE=%d STALL=%d]: %s",_mw_valid,_mw_bub,_mw_stall,instr[MW_D]);
      printf(" - WB = %s: %s",pl_st[_w_st],instr[WB_D]);
    }
  const char pl_st[5][10] = {"","RDY","DNE","TCK","STL"};
  char instr[8][40];
};
#endif