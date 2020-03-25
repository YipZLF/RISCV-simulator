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
  uint64_t branchPred(uint64_t pc_next,uint64_t pc_branch){
    /*always taken*/
    #ifdef DEBUG
    printf("    - IF: branch prediction: taken.\n");
    #endif
    is_b_taken_pred.push_back(true);
    pc_recover.push_back(pc_next);
    return pc_branch; 
  }
  std::deque<uint64_t> pc_recover;
  std::deque<bool> is_b_taken_pred;
  void wrongPred(){
    _d_flush = true;
    _e_flush = true;
    _f_st = READY;
    pc = pc_recover.front(); 
    pc_recover.clear();
    is_b_taken_pred.clear();
  }
  void correctPred(){
    pc_recover.pop_front();
    is_b_taken_pred.pop_front();
  }
  bool hasPredicted(int b_op){
    return (b_op & 0x1);// b_op that needed prediction is encoded with odd number.
  }
/* Internal CPU states register*/

bool _f_b_pred_error,_f_s1_ok;
int _f_st, _f_clk,_f_rs1;
uint64_t _f_pc;
int64_t _f_offset,_f_s1;
uint32_t _f_instr;

bool _fd_valid;
uint64_t _fd_pc;
uint32_t _fd_instr;

bool _d_flush,_d_rs1_ok,_d_rs2_ok;
int _d_st, _d_clk,_d_rd,_d_alu_op,_d_b_op,_d_mem_op,_d_operand_type,_d_rs1,_d_rs2;
int64_t _d_s1,_d_s2,_d_data;
uint64_t _d_pc;
uint32_t _d_instr;

bool _de_valid;
int64_t _de_data,_de_s1,_de_s2;
int _de_rd,_de_b_op,_de_alu_op,_de_mem_op,_de_operand_type;

bool _e_flush;
int _e_st, _e_clk;
int64_t _e_odata,_e_odata2,_e_idata,_e_s1,_e_s2;
int _e_rd,_e_b_op,_e_alu_op,_e_mem_op,_e_operand_type;

bool _em_valid;
int64_t _em_data,_em_data2;
int _em_rd,_em_mem_op,_em_operand_type;

bool _m_flush;
int _m_st, _m_clk;
int64_t _m_idata,_m_odata;
uint64_t _m_addr;
int _m_rd,_m_mem_op,_m_operand_type;

bool _mw_valid;
int64_t _mw_data;
int _mw_rd;

bool _w_flush;
int _w_st, _w_clk;
int64_t _w_data;
int _w_rd;



/*int if_clk, id_clk, ex_clk, mem_clk, wb_clk;

uint64_t _pc_next,_pc_branch;
std::queue<bool> _b_pred_flags;
bool _toBranch;

bool _f_b_pred_err = false;  // Is former branch predict wrong?
uint64_t _f_b_crt_pc; // If yes, then what should the correct branch PC be?

int _f_state,_f_clk;
uint64_t _f_pc = 0,_f_instr = 0;

uint_t _fd_state;
uint64_t _fd_pc,_fd_instr;// deliver pc relative value to decode stage


int _d_state,_d_clk;
bool _d_s1_ok,_d_s2_ok;
int64_t _d_s1,_d_s2;
uint_t _d_alu_op,
       _d_mem_op, 
       _d_wb_op, 
       _d_b_op,
       _d_pc_source,
       _d_operand_type,
       _d_rs1,
       _d_rs2,
       _d_rd;

int64_t _de_s1,_de_s2;
bool _de_valid;
uint_t _de_alu_op,
       _de_mem_op, 
       _de_wb_op, 
       _de_b_op,
       _de_pc_source,
       _de_operand_type,
       _de_rd;
    
int _e_state,_e_clk;
int64_t _e_alu_d;

uint_t _em_alu_d;
uint_t _em_rd;
int _m_state;
uint_t _mw_mem_d;
uint_t _mw_rd;
int _w_state;
uint_t _w_wb_d;

*/
bool hzdDetect(int rs,int64_t & s,bool fromIF = false){
  #ifdef DEBUG
  printf("    - HzdDect: rs = %d\n",rs);
  #endif
  if(rs == ZERO){
    #ifdef DEBUG
    printf("    - HzdDect: fwd ZERO\n");
    #endif
    s = 0; return true;
  }
  if(fromIF){
    if(_fd_valid){
      #ifdef DEBUG
      printf("    - HzdDect: wait until 0x%x decode \n",_fd_instr);
      #endif
      return false;
    }
    if(rs == _d_rd){
      #ifdef DEBUG
      printf("    - HzdDect: hzd with _d_rd, stall %lx \n");
      #endif
      return false;
    }
  }
  
    if(rs == _de_rd && _de_valid){ //hzd detected
      if( _de_mem_op == MREAD){
        #ifdef DEBUG
        printf("    - HzdDect: with memory read _m_odata but not ready\n");
        #endif
        return false;
      }else if( _de_mem_op == MWRITE ){
        printf("!!!!!! ERROR! _em_rd should be ZERO but is now: %d\n", _em_rd);
        return false;
      }else if( _de_mem_op == NOP){
        if(_de_alu_op == NOP){
          s = _de_data;
          #ifdef DEBUG
          printf("    - HzdDect: fwd _de_data %lx \n",_de_data);
          #endif
          return true;
        }else{
          #ifdef DEBUG
          printf("    - HzdDect: not ready until ALU is done\n");
          #endif
          return false;
        }
      }
    }else if( rs == _e_rd ){ //hzd detected
      if(_e_mem_op == MREAD){
        #ifdef DEBUG
        printf("    - HzdDect: with memory read _m_odata but not ready\n");
        #endif
        return false;
      }else if(_e_mem_op == MWRITE){
        printf("!!!!!! ERROR! _em_rd should be ZERO but is now: %d\n", _em_rd);
        return false;
      }else if( _e_mem_op == NOP ){
        if(_e_alu_op == NOP){
          s = _e_idata;
          #ifdef DEBUG
          printf("    - HzdDect: fwd _e_idata %lx \n",_e_idata);
          #endif
          return true;
        }else{ //_e_alu_op != NOP
          if( (!_e_flush && (_e_st == DONE )) ){
            s = _e_odata;
            #ifdef DEBUG
            printf("    - HzdDect: fwd _e_odata %lx \n",_e_odata);
            #endif
            return true; // s is ready
          }else{ // not ready
            #ifdef DEBUG
            printf("    - HzdDect: with _e_odata but not ready\n");
            #endif
            return false;
          }
        }
      }
    }else if( rs == _em_rd && _em_valid){
      if( _em_mem_op == NOP ){ // no operation of memory. so rd must have been ready
        s = _em_data;
        #ifdef DEBUG
        printf("    - HzdDect: fwd _em_data %lx \n",_m_odata);
        #endif
        return true;
      }else if( _em_mem_op == MREAD){
        #ifdef DEBUG
        printf("    - HzdDect: with memory read _m_odata but not ready\n");
        #endif
        return false;
      }else if( _em_mem_op == MWRITE){
        printf("!!!!!! ERROR! _em_rd should be ZERO but is now: %d\n", _em_rd);
        return false;
      }
    }else if( rs == _m_rd){
      if( !_m_flush && (_m_st == DONE ) ){
        s = _m_odata;
        #ifdef DEBUG
        printf("    - HzdDect: fwd _m_odata %lx \n",_m_odata);
        #endif
        return true;
      }else{ // not ready
        #ifdef DEBUG
        printf("    - HzdDect: with _m_odata but not ready\n");
        #endif
        return false;
      }
    }else if( rs == _mw_rd && _em_valid){// must have been ready
      s =  _mw_data;
      #ifdef DEBUG
      printf("    - HzdDect: fwd _mw_data %lx \n",_mw_data);
      #endif
      return true;
    }else if( rs == _w_rd){ //hzd detected
      if(!_w_flush && (_w_st == DONE ) ){
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
    }else{ // no hzd
      s = this->regfile->greg[rs];
      #ifdef DEBUG
      printf("    - HzdDect: no hzd, read %lx from greg\n",(uint64_t)s);
      #endif
      return true;
    }
  
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
    _d_flush = _e_flush = _m_flush = _w_flush = true;
    _f_st = _d_st = _e_st = _m_st = _w_st = READY;
    _f_s1_ok = false;
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
      printf("  - fd : pc:0x%lx, instr:0x%x\n",_fd_pc,_fd_instr);
      printf(" - ID = %s: %s",pl_st[_d_st],instr[ID_D]);
      printf("  - de : %s",instr[DE_D]);
      printf(" - EX = %s: %s",pl_st[_e_st],instr[EX_D]);
      printf("  - em : %s",instr[EM_D]);
      printf(" - MEM = %s: %s",pl_st[_m_st],instr[MEM_D]);
      printf("  - mw : %s",instr[MW_D]);
      printf(" - WB = %s: %s",pl_st[_w_st],instr[WB_D]);
    }
  const char pl_st[5][10] = {"","RDY","DNE","STL","TCK"};
  char instr[8][40];
  #endif
};
#endif