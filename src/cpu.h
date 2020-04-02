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

  
  int IF();
  int ID();
  int EX();
  int MEM();
  int WB();
  uint64_t branchPred(uint64_t pc_next,uint64_t pc_branch,uint64_t &pc_recover,bool &taken){
    /*always taken*/
    #ifdef DEBUG
    printf("    - IF: branch prediction: taken.\n");
    #endif
    taken = true;
    pc_recover = pc_next;
    return pc_branch; 
  }
  std::deque<uint64_t> pc_recover;
  std::deque<bool> is_b_taken_pred;
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
/* Internal CPU states register*/

bool _fetch_bub;

bool _f_b_pred_error,_f_jalr,_f_stall,_f_bub,_f_taken;
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




bool hzdDetect(int rs,int64_t & s){
  #ifdef DEBUG
  printf("    - HzdDect: rs = %d\n",rs);
  #endif
  if(rs == ZERO){
    #ifdef DEBUG
    printf("    - HzdDect: fwd ZERO\n");
    #endif
    s = 0; return true;
  }
  if(rs == _de_rd && _de_valid){ 
    switch(_de_opcode){
      case OP_JALR:
      case OP_UJ:
      case OP_AUIPC:
      case OP_LUI: {
        s = _de_data; 
        #ifdef DEBUG
        printf("    - HzdDect: fwd _de_data %lx \n",_de_data);
        #endif
        return true;
        break;}
      default: return false;
    }
  }
  if( rs == _e_rd ){
    switch(_e_opcode){
      case OP_JALR:
      case OP_UJ:
      case OP_AUIPC:
      case OP_LUI:{
        s = _e_idata;
        #ifdef DEBUG
        printf("    - HzdDect: fwd _e_idata %lx \n",_e_idata);
        #endif
        return true;
      }
      case OP_R:
      case OP_R_W:
      case OP_IMM:
      case OP_IMM_W:{
        if(_e_st == DONE){
          s = _e_odata;
          #ifdef DEBUG
          printf("    - HzdDect: fwd _e_odata %lx \n",_e_odata);
          #endif
          return true; // s is ready
        }else{
          #ifdef DEBUG
          printf("    - HzdDect: with _e_odata but not ready\n");
          #endif
          return false;
        }
      }
      default:{          
        #ifdef DEBUG
        printf("    - HzdDect: wait till mem\n");
        #endif
        return false;
      }
    }
  }
  if( rs == _em_rd && _em_valid){
    switch(_em_opcode){
      case OP_JALR:
      case OP_UJ:
      case OP_AUIPC:
      case OP_LUI:
      case OP_R:
      case OP_R_W:
      case OP_IMM:
      case OP_IMM_W:{
        s = _em_data;
        #ifdef DEBUG
        printf("    - HzdDect: fwd _em_data %lx \n",_em_data);
        #endif
        return true;
      }
      case OP_LOAD:{
        #ifdef DEBUG
        printf("    - HzdDect: with memory read _m_odata but not ready\n");
        #endif
        return false;
      }
      case OP_STORE:{
        printf("!!!!!! ERROR! _em_rd should be ZERO but is now: %d\n", _em_rd);
        return false;
      }
      default:{          
        #ifdef DEBUG
        printf("    - HzdDect: wait\n");
        #endif
        return false;
      }
    }
  }
  if( rs == _m_rd ){
    switch(_m_opcode){
      case OP_JALR:
      case OP_UJ:
      case OP_AUIPC:
      case OP_LUI:
      case OP_R:
      case OP_R_W:
      case OP_IMM:
      case OP_IMM_W:{
        s = _m_odata;
        #ifdef DEBUG
        printf("    - HzdDect: fwd _m_odata %lx \n",_m_odata);
        #endif
        return true;
      }
      case OP_LOAD:{
        if(_m_st == DONE){
          s = _m_odata;
          #ifdef DEBUG
          printf("    - HzdDect: fwd _m_odata %lx \n",_m_odata);
          #endif
          return true;
        }else{
          #ifdef DEBUG
          printf("    - HzdDect: with _m_odata but not ready\n");
          #endif
          return false;
        }
      }
      case OP_STORE:{
        printf("!!!!!! ERROR! _em_rd should be ZERO but is now: %d\n", _m_rd);
        return false;
      }
      default:{          
        #ifdef DEBUG
        printf("    - HzdDect: wait\n");
        #endif
        return false;
      }
    }
  }
  if( rs == _mw_rd && _mw_valid){// must have been ready
    s =  _mw_data;
    #ifdef DEBUG
    printf("    - HzdDect: fwd _mw_data %lx \n",_mw_data);
    #endif
    return true;
  }
  if( rs == _w_rd){ //hzd detected
    if(_w_st == DONE ){
      s =  _w_data;
      #ifdef DEBUG
      printf("    - HzdDect: fwd _w_data %lx \n",_w_data);
      #endif
      return true;
    }else{ // not ready
      #ifdef DEBUG
      printf("    - HzdDect: with _m_odata but not ready\n");
      #endif
      return false;
    }
  }
  // no hzd
  s = this->regfile->greg[rs];
  #ifdef DEBUG
  printf("    - HzdDect: no hzd, read %lx from greg\n",(uint64_t)s);
  #endif
  return true;
}

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
    _fetch_bub = false;
    _fd_bub = _de_bub = _em_bub = _mw_bub = true;
    _f_st = _d_st = _e_st = _m_st = _w_st = READY;
    #ifdef DEBUG
    for(int i = 0; i < 8;++i)
      sprintf(instr[i],"\n");
    #endif
  }
  ~CPU(){
    delete regfile;
    delete alu;
    delete memory;
  }
  uint64_t setEntry(uint64_t addr){return pc = addr;}
  uint64_t setStackPtr(uint64_t addr){return regfile->greg[SP] = addr;}
  int step();
  int pl_step();

  #ifdef DEBUG
  public:
    void printReg(){

      printf("-------Reg file stage---------\n");
      for(int i = 0 ;i < REG_CNT ;i+=4){
        printf(" x%d: 0x%lx\t x%d: 0x%lx\t x%d: 0x%lx\t x%d: 0x%lx\n",
        i,regfile->greg[i],i+1,regfile->greg[i+1],i+2,regfile->greg[i+2],i+3,regfile->greg[i+3]);
      }
      printf("------------------------------\n\n");
    }
    void printPL(){
      printf("-------Pipeline stage---------\n");
      printf(" - IF = %s: %s",pl_st[_f_st],instr[IF_D]);
      printf("  - fd v=%d b=%d st=%d: pc:0x%lx, instr:0x%x\n",_fd_valid,_fd_bub,_fd_stall, _fd_pc,_fd_instr);
      printf(" - ID = %s: %s",pl_st[_d_st],instr[ID_D]);
      printf("  - de v=%d b=%d st=%d: %s",_de_valid,_de_bub,_de_stall,instr[DE_D]);
      printf(" - EX = %s: %s",pl_st[_e_st],instr[EX_D]);
      printf("  - em v=%d b=%d st=%d: %s",_em_valid,_em_bub,_em_stall,instr[EM_D]);
      printf(" - MEM = %s: %s",pl_st[_m_st],instr[MEM_D]);
      printf("  - mw v=%d b=%d st=%d: %s",_mw_valid,_mw_bub,_mw_stall,instr[MW_D]);
      printf(" - WB = %s: %s",pl_st[_w_st],instr[WB_D]);
    }
  const char pl_st[5][10] = {"","RDY","DNE","TCK","STL"};
  char instr[8][40];
  #endif
};
#endif