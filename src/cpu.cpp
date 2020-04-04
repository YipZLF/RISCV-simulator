/* cpu.cpp - the simulated RISCV cpu
 * Author: Ye Zilingfeng
 */
#include "cpu.h"

//utils:
unsigned int getBits(uint_t instr, int s,int e){
  int shift = sizeof(uint_t) * 8 - 1 - e;
  return ((instr<<shift)>>(shift + s));
}
int64_t signExtend(uint_t imm,uint_t imm_size){
  int64_t tmp = (int64_t)imm; 
  int shift = sizeof(long unsigned int) * 8 - imm_size;
  tmp = ((tmp << shift)>>shift);
  return tmp;
}
#define zeroExtend(imm) ((uint64_t)imm)

unsigned int swapEndian32(uint_t value){
  return ((value & 0x000000FF) << 24) |
        ((value & 0x0000FF00) << 8) |
        ((value & 0x00FF0000) >> 8) |
        ((value & 0xFF000000) >> 24) ;
}
bool toBranch(uint_t b_op, uint64_t alu_d){
  switch(b_op){
    case NOP: return false;
    case JALR: return true;
    case JAL: return true;
    case BEQ: return (alu_d == 0)? true: false;
    case BNE: return (alu_d != 0)? true: false;
    case BLTU: return ((int64_t)alu_d < 0)? true: false;
    case BLT: return ((int64_t)alu_d < 0)? true: false;
    case BGEU: return ((int64_t)alu_d >= 0)? true: false;
    case BGE: return ((int64_t)alu_d >= 0)? true: false;
    default: printf("Not implemented: branch logic.\n");break;
  } 
}

bool CPU::hzdDetect(int rs,int64_t & s){
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




int CPU::step(){
  
  /*
  // if instruction is not done; tick once;
  if(cpu_status!=DONE){
    if( (--clocks_to_finish) == 0 )
      cpu_status==DONE;
    return NULL;
  }
  */
  // if done:
  // IF: fetch the instruction;
  uint_t cur_instr;
  memcpy((void *)&cur_instr, (memory->mem + pc) , INSTR_SIZE);
  #ifdef DEBUG
  printf("\nPC:%lx,  Instr:%x\n",this->pc,cur_instr);
  #endif
  if(cur_instr==0) return -1;
  //cur_instr = swapEndian32(cur_instr);

  uint64_t pc_next = pc + 4,pc_branch;

  
  // ID : decode
  // Control signals:
  uint_t opcode = getBits(cur_instr,0,6);
  uint_t alu_op = 0,// alu operation
         mem_op = 0, //memory operation
         wb_op = 0, // wb operation
         b_op= 0, // branch operation
         use_imm = 0,//
         pc_source = 0, // if to write pc back to regs, determine the source
         load_type = 0, 
         store_type = 0,
         operand_type = 0;
  int64_t s1= 0,s2 = 0,imm = 0; // source data 1 and 2
  uint_t funct3=0,funct7=0,rs1=0,rs2=0,rd=0;
  uint_t imm_size = 0;
  switch(opcode){
    case OP_R:{
      funct3 = getBits(cur_instr,12,14);
      funct7 = getBits(cur_instr,25,31);
      rs1 = getBits(cur_instr,15,19);
      rs2 = getBits(cur_instr,20,24);
      rd = getBits(cur_instr,7,11);
      mem_op = NOP;
      wb_op = WB_ALU;
      b_op = NOP;
      operand_type = DOUBLE;
      use_imm = FALSE;
      s1 = regfile->greg[rs1];
      s2 = regfile->greg[rs2];
      switch(funct3){
        case 0x0:{
          switch(funct7){
            case 0x00: alu_op = ALU_ADD;
              #ifdef DEBUG
              printf(" - ID: add %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x01: alu_op = ALU_MUL;
              #ifdef DEBUG
              printf(" - ID: mul %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x20: alu_op = ALU_SUB;
              #ifdef DEBUG
              printf(" - ID: sub %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
          }
          break;
        }
        case 0x1:{
          switch(funct7){
            case 0x00: alu_op = ALU_SLL;
              #ifdef DEBUG
              printf(" - ID: sll %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x01: alu_op = ALU_MULH;
              #ifdef DEBUG
              printf(" - ID: mulh %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        case 0x2:{
          switch(funct7){
            case 0x00: alu_op = ALU_SLT;
              #ifdef DEBUG
              printf(" - ID: slt %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        case 0x4:{
          switch(funct7){
            case 0x00: alu_op = ALU_XOR;
              #ifdef DEBUG
              printf(" - ID: xor %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x01: alu_op = ALU_DIV;
              #ifdef DEBUG
              printf(" - ID: div %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        case 0x5:{
          switch(funct7){
            case 0x00: alu_op = ALU_SRL;
              #ifdef DEBUG
              printf(" - ID: srl %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x20: alu_op = ALU_SRA;
              #ifdef DEBUG
              printf(" - ID: sra %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        case 0x6:{
          switch(funct7){
            case 0x00: alu_op = ALU_OR;
              #ifdef DEBUG
              printf(" - ID: or %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x01: alu_op = ALU_REM;
              #ifdef DEBUG
              printf(" - ID: rem %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        case 0x7:{
          switch(funct7){
            case 0x00: alu_op = ALU_AND;
              #ifdef DEBUG
              printf(" - ID: and %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        default: 
        printf("Instruction not supported!\n");
        return -1;
        break;
      }
      break;
    }
    case OP_R_W:{
      funct3 = getBits(cur_instr,12,14);
      funct7 = getBits(cur_instr,25,31);
      rs1 = getBits(cur_instr,15,19);
      rs2 = getBits(cur_instr,20,24);
      rd = getBits(cur_instr,7,11);
      mem_op = NOP;
      wb_op = WB_ALU;
      b_op = NOP;
      operand_type = WORD;
      use_imm = FALSE;
      s1 = regfile->greg[rs1];
      s2 = regfile->greg[rs2];
      switch(funct3){
        case 0x0:{
          switch(funct7){
            case 0x00: alu_op = ALU_ADD;
              #ifdef DEBUG
              printf(" - ID: addw %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x20: alu_op = ALU_SUB;
              #ifdef DEBUG
              printf(" - ID: subw %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
          }
        break;
        }
        case 0x1:{
          switch(funct7){
            case 0x00: alu_op = ALU_SLL;
              #ifdef DEBUG
              printf(" - ID: sllw %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        case 0x5:{
          switch(funct7){
            case 0x00: alu_op = ALU_SRL;
              #ifdef DEBUG
              printf(" - ID: srlw %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            case 0x20: alu_op = ALU_SRA;
              #ifdef DEBUG
              printf(" - ID: sraw %u, %u, %u\n",rd,rs1,rs2);
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
            }
          break;
        }
        default: 
          printf("Instruction not supported!\n");
          return -1;
        break;
      }
      break;
    }
    case OP_IMM:{
      funct3 = getBits(cur_instr,12,14);
      rs1 = getBits(cur_instr,15,19);
      rd = getBits(cur_instr,7,11);
      imm = getBits(cur_instr,20,31);
      imm_size = 12;
      mem_op = NOP;
      wb_op = WB_ALU;
      b_op = NOP;
      use_imm = TRUE;
      s1 = regfile->greg[rs1];
      s2 = imm;
      switch(funct3){
        case 0x0:{ //addi rd,rs1,imm
          alu_op = ALU_ADD;
          #ifdef DEBUG
          printf(" - ID: addi %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x1:{
          funct7 = getBits(cur_instr,26,31);
          switch(funct7){
            case 0x00: alu_op = ALU_SLL;
              s2 = imm & 0x3f; // only imm[5:0]
              imm_size = 6;
              #ifdef DEBUG
              printf(" - ID: slli %u, %u,$%ld\n",rd,rs1,signExtend(imm,imm_size));
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
          }
          break;
        }
        case 0x2:{
          alu_op = ALU_SLT;
          #ifdef DEBUG
          printf(" - ID: slti %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x4:{
          alu_op = ALU_XOR;
          #ifdef DEBUG
          printf(" - ID: xori %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x5:{
          funct7 = getBits(cur_instr,26,31);
          switch(funct7){
            case 0x00: alu_op = ALU_SRL;
              s2 = imm & 0x3f; // only imm[5:0]
              imm_size = 6;
              #ifdef DEBUG
              printf(" - ID: srli %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
              #endif
              break;
            case 0x10: alu_op = ALU_SRA;
              s2 = imm & 0x3f; // only imm[5:0]
              imm_size = 6;
              #ifdef DEBUG
              printf(" - ID: srai %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
          }
          break;
        }
        case 0x6:{
          alu_op = ALU_OR;
          #ifdef DEBUG
          printf(" - ID: ori %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x7:{
          alu_op = ALU_AND;
          #ifdef DEBUG
          printf(" - ID: andi %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
          #endif
          break;
        }
      }
      break;
    }
    case OP_IMM_W:{
      funct3 = getBits(cur_instr,12,14);
      rs1 = getBits(cur_instr,15,19);
      rd = getBits(cur_instr,7,11);
      imm = getBits(cur_instr,20,31);
      imm_size = 12;
      operand_type = WORD;
      mem_op = NOP;
      b_op = NOP;
      wb_op = WB_ALU;
      use_imm = TRUE;
      s1 = regfile->greg[rs1];
      s2 = imm;
      switch(funct3){
        case 0x0:{ //addi rd,rs1,imm
          alu_op = ALU_ADD;
          #ifdef DEBUG
          printf(" - ID: addiw %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x1:{
          funct7 = getBits(cur_instr,26,31);
          switch(funct7){
            case 0x00: alu_op = ALU_SLL;
              s2 = imm & 0x3f; // only imm[5:0]
              imm_size = 6;
              #ifdef DEBUG
              printf(" - ID: slliw %u, %u,$%ld\n",rd,rs1,signExtend(imm,imm_size));
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
          }
          break;
        }
        case 0x5:{
          funct7 = getBits(cur_instr,26,31);
          switch(funct7){
            case 0x00: alu_op = ALU_SRL;
              s2 = imm & 0x3f; // only imm[5:0]
              imm_size = 6;
              #ifdef DEBUG
              printf(" - ID: srliw %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
              #endif
              break;
            case 0x10: alu_op = ALU_SRA;
              s2 = imm & 0x3f; // only imm[5:0]
              #ifdef DEBUG
              printf(" - ID: sraiw %u, %u, $%ld\n",rd,rs1,signExtend(imm,imm_size));
              #endif
              break;
            default:
              printf("Instruction not supported!\n");
              return -1;
          }
          break;
        }
      }
      break;
    }
    case OP_LOAD:{
      funct3 = getBits(cur_instr,12,14);
      rs1 = getBits(cur_instr,15,19);
      rd = getBits(cur_instr,7,11);
      imm = getBits(cur_instr,20,31);
      imm_size = 12;
      alu_op = ALU_ADD;
      mem_op = MREAD;
      b_op = NOP;
      wb_op = WB_MEM;
      use_imm = TRUE;
      s1 = regfile->greg[rs1];
      s2 = imm;
      switch(funct3){
        case 0x6:{
          load_type = LWORDU;
          #ifdef DEBUG
          printf(" - ID: lwu %u, $0x%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        case 0x5:{
          load_type = LHALFU;
          #ifdef DEBUG
          printf(" - ID: lhu %u, $0x%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        case 0x4:{
          load_type = LBYTEU;
          #ifdef DEBUG
          printf(" - ID: lbu %u, $0x%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        case 0x3:{ 
          load_type = LDOUBLE;
          #ifdef DEBUG
          printf(" - ID: ld %u, $0x%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        case 0x2:{
          load_type = LWORD;
          #ifdef DEBUG
          printf(" - ID: lw %u, $0x%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        case 0x1:{
          load_type = LHALF;
          #ifdef DEBUG
          printf(" - ID: lh %u, $0x%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        case 0x0:{
          load_type = LBYTE;
          #ifdef DEBUG
          printf(" - ID: lb %u, $%lx(%u)\n",rd,imm,rs1);
          #endif
          break;
        }
        default: printf("Not implemented instruction\n");return -1;
      }
      break;
    }
    case OP_SB:{
      funct3 = getBits(cur_instr,12,14);
      rs1 = getBits(cur_instr,15,19);
      rs2 = getBits(cur_instr,20,24);
      imm = (getBits(cur_instr,8,11) << 1) + 
            (getBits(cur_instr,25,30) << 5) + 
            (getBits(cur_instr,7,7) << 11) + 
            (getBits(cur_instr,31,31) << 12);
      imm_size = 13;
      mem_op = NOP;
      wb_op = NOP;
      use_imm = FALSE;
      s1 = regfile->greg[rs1];
      s2 = regfile->greg[rs2];
      switch(funct3){
        case 0x0:{ 
          alu_op = ALU_SUB;
          b_op = BEQ;
          #ifdef DEBUG
          printf(" - ID: beq %u, %u, %lx\n",rs1,rs2,this->pc+signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x1:{ 
          alu_op = ALU_SUB;
          b_op = BNE;
          #ifdef DEBUG
          printf(" - ID: bne %u, %u, %lx\n",rs1,rs2,this->pc+signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x4:{ 
          alu_op = ALU_SUB;
          b_op = BLT;
          #ifdef DEBUG
          printf(" - ID: blt %u, %u, %lx\n",rs1,rs2,this->pc+signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x5:{ 
          alu_op = ALU_SUB;
          b_op = BGE;
          #ifdef DEBUG
          printf(" - ID: bge %u, %u, %lx\n",rs1,rs2,this->pc+signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x6:{ 
          alu_op = ALU_SUB_U;
          b_op = BLTU;
          #ifdef DEBUG
          printf(" - ID: bltu %u, %u, %lx\n",rs1,rs2,this->pc+signExtend(imm,imm_size));
          #endif
          break;
        }
        case 0x7:{ 
          alu_op = ALU_SUB_U;
          b_op = BGEU;
          #ifdef DEBUG
          printf(" - ID: bgeu %u, %u, %lx\n",rs1,rs2,this->pc+signExtend(imm,imm_size));
          #endif
          break;
        }
        default: 
          printf("Instruction not supported!\n");
          return -1;
          break;
      }
      break;
    }
    case OP_STORE:{
      funct3 = getBits(cur_instr,12,14);
      rs1 = getBits(cur_instr,15,19);
      rs2 = getBits(cur_instr,20,24);
      imm = (getBits(cur_instr,7,11)) + 
            (getBits(cur_instr,25,31) <<5);
      imm_size = 12;
      alu_op = ALU_ADD;
      mem_op = MWRITE;
      wb_op = NOP;
      b_op = NOP;
      use_imm = TRUE;
      s1 = regfile->greg[rs1];
      s2 = regfile->greg[rs2];
      switch(funct3){
        case 0x3:{
          store_type = SDOUBLE;
          #ifdef DEBUG
          printf(" - ID: sd %u $%lx(%u)\n",rs2,imm,rs1);
          #endif
          break;
        }
        case 0x2:{
          store_type = SWORD;
          #ifdef DEBUG
          printf(" - ID: sw %u $%lx(%u)\n",rs2,imm,rs1);
          #endif
          break;
        }
        case 0x1:{
          store_type = SHALF;
          #ifdef DEBUG
          printf(" - ID: sh %u $%lx(%u)\n",rs2,imm,rs1);
          #endif
          break;
        }
        case 0x0:{
          store_type = SBYTE;
          #ifdef DEBUG
          printf(" - ID: sb %u $%lx(%u)\n",rs2,imm,rs1);
          #endif
          break;
        }
        default: printf("Instruction not implemented.\n"); return -1;
      }
      break;
    }
    case OP_AUIPC:{// auipc
      rd = getBits(cur_instr,7,11);
      imm = (getBits(cur_instr,12,31) << 12);
      imm_size = 32;
      alu_op = ALU_ADD;
      mem_op = NOP;
      wb_op = WB_PC;
      b_op = NOP;
      pc_source = PC_IMM;
      use_imm = TRUE;
      #ifdef DEBUG
      printf(" - ID: auipc %u, $%ld\n",rd, (long)getBits(cur_instr,12,31));
      #endif
      break;
    } 
    case OP_LUI:{
      rd = getBits(cur_instr,7,11);
      imm = (getBits(cur_instr,12,31) << 12);
      imm_size = 32;
      alu_op = NOP;
      mem_op = NOP;
      wb_op = WB_IMM;
      b_op = NOP;
      use_imm = TRUE;
      #ifdef DEBUG
      printf(" - ID: lui %u, $%ld\n",rd, (long)getBits(cur_instr,12,31));
      #endif
      break;
    }
    case OP_UJ:{
      rd = getBits(cur_instr,7,11);
      imm = (getBits(cur_instr,21,30) << 1) + 
            (getBits(cur_instr,20,20) << 11) + 
            (getBits(cur_instr,12,19) << 12) + 
            (getBits(cur_instr,31,31) << 20);
      imm_size = 21;
      mem_op = NOP;
      alu_op = NOP;
      wb_op = WB_PC;
      pc_source = PC_NEXT;
      b_op = JAL;
      #ifdef DEBUG
      printf(" - ID: jal %u, %lx\n",rd, (long)imm+this->pc);
      #endif
      break;
    }
    case OP_JALR:{
      rd = getBits(cur_instr,7,11);
      rs1 = getBits(cur_instr,15,19);
      imm = getBits(cur_instr,20,31);
      imm_size = 12;
      wb_op = WB_PC;
      pc_source = PC_NEXT;
      b_op = JALR;
      alu_op = ALU_ADD;
      use_imm = TRUE;
      mem_op = NOP;

      s1 = regfile->greg[rs1];
      #ifdef DEBUG
      printf(" - ID: jalr %u,%lx(%u)\n",rd,imm,rs1);
      #endif
      break;
    }
    case OP_ECALL:{      
      #ifdef DEBUG
      printf(" - ID: ecall\n");
      #endif
      if( cur_instr!= 0x73 ) return -1;
      if( this->regfile->greg[A7]==1) return -2;

      break;
    }
    default: { 
      printf("Instruction not supported!\n");
      return -1;
      break;
    }
  }

  imm = signExtend(imm,imm_size);

  // EX
  if(use_imm) s2 = imm; else s2 = s2;
  int64_t alu_d = 0;
  switch(alu_op){
    case ALU_ADD: alu_d = (int64_t)s1 + (int64_t)s2; break;
    case ALU_SUB: alu_d = (int64_t)s1 - (int64_t)s2; break;
    case ALU_SUB_U: alu_d = (uint64_t)s1 - (uint64_t)s2;break;
    case ALU_MUL: {
      __int128_t tmp = s1* s2;
      alu_d = tmp & 0xffffffffffffffff; break;}
    case ALU_SLL: alu_d = s1 << s2; break;
    case ALU_MULH: {
      __int128_t tmp = s1* s2;
      alu_d = tmp >> 64; break;}
    case ALU_SLT: alu_d = ((int64_t)s1 < (int64_t)s2)? 1: 0; break;
    case ALU_XOR: alu_d = s1 ^ s2; break;
    case ALU_DIV: alu_d = s1 / s2; break;
    case ALU_SRL: alu_d = (uint64_t)s1 >> (int64_t)s2; break;
    case ALU_SRA: alu_d = (int64_t) s1 >> (int64_t)s2; break;
    case ALU_OR: alu_d = s1 | s2; break;
    case ALU_REM: alu_d = s1 % s2; break;
    case ALU_AND: alu_d = s1 & s2; break;
    case NOP: break;
    default: 
      printf("ALU op error!"); return -1;
  }
  #ifdef DEBUG
  if(alu_op != NOP) printf(" - EX: s1: 0x%lx, s2: 0x%lx, alu_d: 0x%lx\n",s1,s2,alu_d);
  #endif
  int64_t pc_d = 0;
  if(pc_source==PC_NEXT){
    pc_d = pc_next;
  }else if(pc_source==PC_IMM){
    pc_d = pc+imm;
  }
  
  #ifdef DEBUG
  if(b_op != NOP) printf("    - tobranch?: %d %ld %ld\n",toBranch(b_op,alu_d),(uint64_t)b_op,alu_d);
  #endif
  if(toBranch(b_op,alu_d)) {
    if(b_op == JALR) pc = (alu_d & (~((uint64_t)1)));
    else pc = pc + imm; 
  }else{ 
    pc = pc_next;
  }

  

  // MEM
  #ifdef DEBUG
  if(mem_op != NOP){
    printf("    - Mem content check(before): \n      ");
    for(int i = 0 ;i < 8 ;++i){
      printf("%02x ", memory->mem[(uint64_t)alu_d+i]);
    }
    printf("\n");
  }
  #endif
  uint64_t mem_d = 0;
  int64_t mem_s = 0;
  switch(mem_op){
    case MREAD:{
      //mem_d = memory->mem[(uint64_t)alu_d];
      memcpy(&mem_d, &memory->mem[(uint64_t)alu_d], sizeof(uint64_t));
      #ifdef DEBUG
      printf(" - MEM: read data: 0x%lx at addr: 0x%lx\n",(uint64_t)mem_d,(uint64_t)alu_d);
      #endif
      break;
    }
    case MWRITE:{
      mem_s = regfile->greg[rs2];
      int store_size= 0;
      switch(store_type){
        case SBYTE: store_size = 1; break;
        case SHALF: store_size = 2; break;
        case SWORD: store_size = 4; break;
        case SDOUBLE: store_size = 8;break;
        /*case SBYTE: mem_s &= 0xff; break;
        case SHALF: mem_s &= 0xffff; break;
        case SWORD: mem_s &= 0xffffffff; break;
        case SDOUBLE:break;*/
        default: printf(" store type error!\n"); return -1;
      }
      memcpy( &memory->mem[(uint64_t)alu_d],&mem_s, store_size);
      //memory->mem[alu_d] = mem_s;
      #ifdef DEBUG
      printf(" - MEM: store data: 0x%lx at addr: 0x%lx\n",(uint64_t)mem_s,(uint64_t)alu_d);
      #endif
      break;
    }
    case NOP:{
      break;
    }
  }
  #ifdef DEBUG
  if(mem_op != NOP){
    printf("    - Mem content check(after): \n      ");
    for(int i = 0 ;i < 8 ;++i){
      printf("%02x ", memory->mem[(uint64_t)alu_d+i]);
    }
    printf("\n");
  }
  #endif
  // WB
  if(rd != ZERO){
    switch(wb_op){
      case WB_ALU:  {
        switch(operand_type){
          case WORD: alu_d = signExtend(alu_d & 0xffffffff, 32); break;
          default:break;
        }
        regfile->greg[rd] = alu_d; break;
      }
      case WB_MEM: {
        switch(load_type){
          case LBYTE: mem_d = signExtend(mem_d, 8); break;
          case LHALF: mem_d = signExtend(mem_d, 16);break;
          case LWORD: mem_d = signExtend(mem_d, 32);break;
          case LBYTEU: mem_d = zeroExtend(mem_d & 0xff); break;
          case LHALFU: mem_d = zeroExtend(mem_d & 0xffff);break;
          case LWORDU: mem_d = zeroExtend(mem_d & 0xffffffff); break;
          case LDOUBLE:break;
          default: printf(" load type error!\n"); return -1;
        }
        regfile->greg[rd] = mem_d; break;}
      case WB_PC: regfile->greg[rd] = pc_d; break;
      case WB_IMM: regfile->greg[rd] = imm; break;
      case NOP: break;
      default:printf(" Write back logic error!\n");
    }
  }
  #ifdef DEBUG
  else if(wb_op != NOP && rd == ZERO){
    printf(" - WB: Got rd = ZERO\n");
  }
  #endif
  return 0;
}

int CPU::pl_step(){
  int ret = 0;
  ret = WB();
  if(ret != 0) return ret;
  ret = MEM();
  if(ret != 0) return ret;
  ret = EX();
  if(ret != 0) return ret;
  ret = ID();
  if(ret != 0) return ret;
  ret = IF();
  if(ret != 0) return ret;
  statsInc(clk_cnt);
  return ret = 0;
}

int CPU::IF(){
  if(_f_st == DONE){
    if( !_fd_stall && !_fd_valid && !_fd_bub){
      if( _f_bub ){//pass down the bubble
        _fd_bub = _f_bub;
        _f_bub = false;
      }
      _fd_instr = _f_instr;
      _fd_pc = _f_pc;
      _fd_taken = _f_taken;
      _fd_pc_recover = _f_pc_recover;
      
      _f_pc = _f_pc_recover = _f_taken = _f_instr = 0;
      
      _fd_valid = true;
      _f_st = READY;
    }
  }
  if(_f_st == READY){
    if(_fetch_bub){
      sprintf(instr[IF_D],"Bubble\n");
      #ifdef DEBUG
      printf("\n - IF: %s",instr[IF_D]);
      #endif
      _f_instr = 0;
      _f_pc = 0;
      _f_bub = _fetch_bub;
      if(_pc_from_ecall==0)
        _fetch_bub = false;
    }else{
      _f_pc = this->pc;
      memcpy((void *)&_f_instr, (memory->mem + _f_pc) , INSTR_SIZE);
      statsInc(instr_cnt);
      sprintf(instr[IF_D],"PC:%lx,  Instr:0x%x\n",this->pc,_f_instr);
      #ifdef DEBUG
      printf("\n - IF: %s",instr[IF_D]);
      #endif
      if(_f_instr==0) {
        printf(" - IF: failure when reading from 0x%lx\n",pc);
      }
      uint64_t _pc_branch,_pc_next = this->pc+4;
      uint_t opcode = getBits(_f_instr,0,6);
      _f_offset = (getBits(_f_instr,8,11) << 1) + 
                (getBits(_f_instr,25,30) << 5) + 
                (getBits(_f_instr,7,7) << 11) + 
                (getBits(_f_instr,31,31) << 12);
      _f_offset = signExtend(_f_offset,13);
      switch(opcode){
        case OP_SB:{ 
          _pc_branch = (uint64_t)pc + _f_offset; 
          //branch prediction
          this->pc = branchPred(_pc_next,_pc_branch,_f_pc_recover,_f_taken); 
          #ifdef DEBUG
          printf("   - IF: SB: pc = 0x%lx\n",pc);
          #endif 
          break;
        }
        case OP_UJ:{ 
          _f_offset = (getBits(_f_instr,21,30) << 1) + 
                      (getBits(_f_instr,20,20) << 11) + 
                      (getBits(_f_instr,12,19) << 12) + 
                      (getBits(_f_instr,31,31) << 20);
          _f_offset = signExtend(_f_offset,21);
          _pc_branch = (uint64_t)this->pc + _f_offset; 
          this->pc = _pc_branch;
          #ifdef DEBUG
          printf("   - IF: UJ:pc = 0x%lx\n",this->pc);
          #endif 
          break;
        }
        case OP_JALR:{
          this->pc = _pc_next;
          _fetch_bub = true;
          #ifdef DEBUG
          printf("   - IF: JALR:insert bubble to next IF\n",pc);
          #endif 

          break;
        } 
        case OP_AUIPC:{
          this->pc = _pc_next;
          #ifdef DEBUG
          printf("   - IF: AUIPC:pc = 0x%lx\n",pc);
          #endif 
          break;
        }
        default: {
          pc = _pc_next;
          break;
        }
      }
    }
    _f_st = TICKING;
    simClocksToFinish(_f_clk,CLK_IF);
  }
  if(_f_st == TICKING){
    simTick(_f_clk);
    if(simTickDone(_f_clk)) {
      _f_st = DONE;
    }
  }
  return 0;
}

int CPU::ID(){
  if( _d_st == DONE){
    if(!_de_stall && !_de_valid && !_de_bub){
      if(_d_bub){
        _de_bub = true;
        _d_bub = false;
      }
      _de_s1 = _d_s1;
      _de_s2 = _d_s2;
      _de_rd = _d_rd;
      _de_alu_op = _d_alu_op;
      _de_b_op = _d_b_op;
      _de_mem_op = _d_mem_op;
      _de_data = _d_data;
      _de_operand_type = _d_operand_type;
      _de_opcode = _d_opcode;
      _de_taken = _d_taken;
      _de_pc_recover = _d_pc_recover;
      if(_d_opcode == OP_JALR){
        _fetch_bub = true;
        _fd_bub = true;
      }
      _d_s1 = 0;
      _d_s2 = 0;
      _d_rd = 0;
      _d_alu_op = 0;
      _d_b_op = 0;
      _d_mem_op = 0;
      _d_data = 0;
      _d_operand_type = 0;
      _d_opcode = 0;
      _d_pc_recover = 0;
      _d_taken = 0;

      _de_valid = true;
      _d_st = READY;
      if(verbose)
        strcpy(instr[DE_D],instr[ID_D]);
      sprintf(instr[ID_D],"\n");
    }
  }
  if( _d_st == READY){
    if(_fd_bub){
      _d_bub = _fd_bub;
      _fd_bub = false;
      _fd_valid = false;
      _d_opcode = OP_BUBBLE;
      simClocksToFinish(_d_clk,CLK_ONE);
      _d_st = TICKING;
      sprintf(instr[ID_D],"Bubble\n");
      #ifdef DEBUG
      printf("\n - ID: %s",instr[ID_D]);
      #endif

    }else{
      if( ! _fd_valid )
        return 0;
      
      _d_instr = _fd_instr;
      _d_pc = _fd_pc;
      _d_taken = _fd_taken;
      _d_pc_recover = _fd_pc_recover;
      #ifdef DEBUG
      printf("    - ID: at 0x%lx fetched 0x%x \n",_d_pc,_d_instr);
      #endif

      if(_d_instr==0) printf(" - ID: failure when decoding 0x%lx\n",_d_instr);
      
      _d_opcode = getBits(_d_instr,0,6);
      uint_t funct3 = 0,funct7 = 0;
      switch(_d_opcode){
        case OP_R:{
          funct3 = getBits(_d_instr,12,14);
          funct7 = getBits(_d_instr,25,31);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_rs2 = getBits(_d_instr,20,24);
          _d_rd = getBits(_d_instr,7,11);
          _d_mem_op = NOP;
          _d_b_op = NOP;
          _d_operand_type = DOUBLE;
          switch(funct3){
            case 0x0:{
              switch(funct7){
                case 0x00: {_d_alu_op = ALU_ADD;
                  
                  sprintf(instr[ID_D],"add %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;}
                case 0x01: {_d_alu_op = ALU_MUL;
                  
                  sprintf(instr[ID_D],"mul %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;}
                case 0x20: {_d_alu_op = ALU_SUB;
                  
                  sprintf(instr[ID_D],"sub %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;}
                default:
                  printf("Instruction %x not supported!\n",_d_instr);
                  return ERROR;
              }
              break;
            }
            case 0x1:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SLL;
                  
                  sprintf(instr[ID_D],"sll %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                case 0x01: _d_alu_op = ALU_MULH;
                  
                  sprintf(instr[ID_D],"mulh %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return ERROR;
                }
              break;
            }
            case 0x2:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SLT;
                  
                  sprintf(instr[ID_D],"slt %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
                }
              break;
            }
            case 0x4:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_XOR;
                  
                  sprintf(instr[ID_D],"xor %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                case 0x01: _d_alu_op = ALU_DIV;
                  
                  sprintf(instr[ID_D],"div %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return ERROR;
                }
              break;
            }
            case 0x5:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SRL;
                  
                  sprintf(instr[ID_D],"srl %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                case 0x20: _d_alu_op = ALU_SRA;
                  
                  sprintf(instr[ID_D],"sra %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
                }
              break;
            }
            case 0x6:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_OR;
                  
                  sprintf(instr[ID_D],"or %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                case 0x01: _d_alu_op = ALU_REM;
                  
                  sprintf(instr[ID_D],"rem %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
                }
              break;
            }
            case 0x7:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_AND;
                  
                  sprintf(instr[ID_D],"and %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
                }
              break;
            }
            default: 
              printf("Instruction not supported!\n");
              return ERROR;
            }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          _d_rs2_ok = hzdDetect(_d_rs2,_d_s2);
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          break;
        }
        case OP_R_W:{
          funct3 = getBits(_d_instr,12,14);
          funct7 = getBits(_d_instr,25,31);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_rs2 = getBits(_d_instr,20,24);
          _d_rd = getBits(_d_instr,7,11);
          _d_mem_op = NOP;
          _d_b_op = NOP;
          _d_operand_type = WORD;
          switch(funct3){
            case 0x0:{
              switch(funct7){
                case 0x00: {_d_alu_op = ALU_ADD;
                  
                  sprintf(instr[ID_D],"addw %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;}
                case 0x20: {_d_alu_op = ALU_SUB;
                  
                  sprintf(instr[ID_D],"subw %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;}
                default:
                  printf("Instruction %x not supported!\n",_d_instr);
                  return ERROR;
              }
              break;
            }
            case 0x1:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SLL;
                  
                  sprintf(instr[ID_D],"sllw %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return ERROR;
                }
              break;
            }
            case 0x5:{
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SRL;
                  
                  sprintf(instr[ID_D],"srlw %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                case 0x20: _d_alu_op = ALU_SRA;
                  
                  sprintf(instr[ID_D],"sraw %u, %u, %u\n",_d_rd,_d_rs1,_d_rs2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
                }
              break;
            }
            default: 
              printf("Instruction not supported!\n");
              return ERROR;
            }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          _d_rs2_ok = hzdDetect(_d_rs2,_d_s2);
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          break;
        }
        case OP_IMM:{
          funct3 = getBits(_d_instr,12,14);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_rd = getBits(_d_instr,7,11);
          _d_s2 = signExtend(getBits(_d_instr,20,31),12);
          _d_rs2_ok = true;
          _d_mem_op = NOP;
          _d_b_op = NOP;
          _d_operand_type = DOUBLE;
          switch(funct3){
            case 0x0:{ _d_alu_op = ALU_ADD;
              
              sprintf(instr[ID_D],"addi %u, %u, $%ld\n",_d_rd,_d_rs1,_d_s2);
              
              break;
            } 
            case 0x1:{
              funct7 = getBits(_d_instr,26,31);
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SLL;
                  _d_s2 = signExtend(0x3f & _d_s2,6); // only imm[5:0]
                  
                  sprintf(instr[ID_D],"slli %u, %u,$%ld\n",_d_rd,_d_rs1,_d_s2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
              }
              break;
            }
            case 0x2:{ _d_alu_op = ALU_SLT;
              
              sprintf(instr[ID_D],"slti %u, %u, $%ld\n",_d_rd,_d_rs1,_d_s2);
              
              break;
            } 
            case 0x4:{ _d_alu_op = ALU_XOR;
              
              sprintf(instr[ID_D],"xori %u, %u, $%ld\n",_d_rd,_d_rs1,_d_s2);
              
              break;
            } 
            case 0x5:{
              funct7 = getBits(_d_instr,26,31);
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SRL;
                  _d_s2 = signExtend(0x3f & _d_s2,6); // only imm[5:0]
                  
                  sprintf(instr[ID_D],"srli %u, %u,$%ld\n",_d_rd,_d_rs1,_d_s2);
                  
                  break;
                case 0x10: _d_alu_op = ALU_SRA;
                  _d_s2 = signExtend(0x3f & _d_s2,6); // only imm[5:0]
                  
                  sprintf(instr[ID_D],"srai %u, %u,$%ld\n",_d_rd,_d_rs1,_d_s2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
              }
              break;
            }
            case 0x6:{ _d_alu_op = ALU_OR;
              
              sprintf(instr[ID_D],"ori %u, %u, $%ld\n",_d_rd,_d_rs1,_d_s2);
              
              break;
            }
            case 0x7:{ _d_alu_op = ALU_AND;
              
              sprintf(instr[ID_D],"andi %u, %u, $%ld\n",_d_rd,_d_rs1,_d_s2);
              
              break;
            }  
            default:{
              printf("Instruction %x not supported!\n",_d_instr);
              return ERROR;
            }
          }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
            return 0;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          
          break;
        }
        case OP_IMM_W:{
          funct3 = getBits(_d_instr,12,14);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_rd = getBits(_d_instr,7,11);
          _d_s2 = signExtend(getBits(_d_instr,20,31),12);
          _d_rs2_ok = true;
          _d_mem_op = NOP;
          _d_b_op = NOP;
          _d_operand_type = WORD;
          switch(funct3){
            case 0x0:{ _d_alu_op = ALU_ADD;
              
              sprintf(instr[ID_D],"addiw %u, %u, $%ld\n",_d_rd,_d_rs1,_d_s2);
              
              break;
            } 
            case 0x1:{
              funct7 = getBits(_d_instr,26,31);
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SLL;
                  _d_s2 = signExtend(0x3f & _d_s2,6); // only imm[5:0]
                  
                  sprintf(instr[ID_D],"slliw %u, %u,$%ld\n",_d_rd,_d_rs1,_d_s2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
              }
              break;
            }
            case 0x5:{
              funct7 = getBits(_d_instr,26,31);
              switch(funct7){
                case 0x00: _d_alu_op = ALU_SRL;
                  _d_s2 = signExtend(0x3f & _d_s2,6); // only imm[5:0]
                  
                  sprintf(instr[ID_D],"srliw %u, %u,$%ld\n",_d_rd,_d_rs1,_d_s2);
                  
                  break;
                case 0x10: _d_alu_op = ALU_SRA;
                  _d_s2 = signExtend(0x3f & _d_s2,6); // only imm[5:0]
                  
                  sprintf(instr[ID_D],"sraiw %u, %u,$%ld\n",_d_rd,_d_rs1,_d_s2);
                  
                  break;
                default:
                  printf("Instruction not supported!\n");
                  return -1;
              }
              break;
            }
            default:{
              printf("Instruction %x not supported!\n",_d_instr);
              return ERROR;
            }
          }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          
          break;
        }
        case OP_LOAD:{
          funct3 = getBits(_d_instr,12,14);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_s2 = signExtend(getBits(_d_instr,20,31),12);
          _d_rs2_ok = true;
          _d_rd = getBits(_d_instr,7,11);
          _d_alu_op = ALU_ADD;
          _d_mem_op = MREAD;
          _d_b_op = NOP;
          switch(funct3){
            case 0x6:{_d_operand_type = WORDU;
              
              sprintf(instr[ID_D],"lwu %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            case 0x5:{_d_operand_type = HALFU;
              
              sprintf(instr[ID_D],"lhu %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            case 0x4:{_d_operand_type = BYTEU;
              
              sprintf(instr[ID_D],"lbu %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            case 0x3:{_d_operand_type = DOUBLE;
              
              sprintf(instr[ID_D],"ld %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            case 0x2:{_d_operand_type = WORD;
              
              sprintf(instr[ID_D],"lw %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            case 0x1:{_d_operand_type = HALF;
              
              sprintf(instr[ID_D],"lh %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            case 0x0:{_d_operand_type = LBYTE;
              
              sprintf(instr[ID_D],"lb %u, $0x%lx(%u)\n",_d_rd,_d_s2,_d_rs1);
              
              break;
            }
            default: printf("Not implemented instruction\n");return ERROR;
          }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          
          break;
        }
        case OP_STORE:{
          funct3 = getBits(_d_instr,12,14);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_rs2 = getBits(_d_instr,20,24);
          _d_s2 = signExtend((getBits(_d_instr,7,11)) + (getBits(_d_instr,25,31) <<5)
                              , 12);
          _d_alu_op = ALU_ADD;
          _d_mem_op = MWRITE;
          _d_b_op = NOP;
          _d_rd = 0;// 0 for NONE
          switch(funct3){
            case 0x3:{_d_operand_type = DOUBLE;
              
              sprintf(instr[ID_D],"sd %u, $0x%lx(%u)\n",_d_rs2,_d_s2,_d_rs1);
              
              break;
            }
            case 0x2:{_d_operand_type = WORD;
              
              sprintf(instr[ID_D],"sw %u, $0x%lx(%u)\n",_d_rs2,_d_s2,_d_rs1);
              
              break;
            }
            case 0x1:{_d_operand_type = HALF;
              
              sprintf(instr[ID_D],"sh %u, $0x%lx(%u)\n",_d_rs2,_d_s2,_d_rs1);
              
              break;
            }
            case 0x0:{_d_operand_type = BYTE;
              
              sprintf(instr[ID_D],"sb %u, $0x%lx(%u)\n",_d_rs2,_d_s2,_d_rs1);
              
              break;
            }
            default: printf("Instruction not implemented.\n"); return ERROR;
          }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          _d_rs2_ok = hzdDetect(_d_rs2,_d_data); // served as data to store
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          
          break;
        }
        case OP_SB:{
          funct3 = getBits(_d_instr,12,14);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_rs2 = getBits(_d_instr,20,24);
          _d_rd = 0;
          _d_mem_op = NOP;
          _d_operand_type = DOUBLE;
          switch(funct3){
            case 0x0:{_d_b_op = BEQ;
              _d_alu_op = ALU_SUB;
              
              int64_t imm = _d_pc + signExtend((getBits(_d_instr,8,11) << 1) + 
                                        (getBits(_d_instr,25,30) << 5) + 
                                        (getBits(_d_instr,7,7) << 11) + 
                                        (getBits(_d_instr,31,31) << 12),13);
              sprintf(instr[ID_D],"beq %u, %u, %lx\n",_d_rs1,_d_rs2,imm);
              
              break;
            }
            case 0x1:{_d_b_op = BNE;
              _d_alu_op = ALU_SUB;
              
              int64_t imm = _d_pc + signExtend((getBits(_d_instr,8,11) << 1) + 
                                        (getBits(_d_instr,25,30) << 5) + 
                                        (getBits(_d_instr,7,7) << 11) + 
                                        (getBits(_d_instr,31,31) << 12),13);
              sprintf(instr[ID_D],"bne %u, %u, %lx\n",_d_rs1,_d_rs2,imm);
              
              break;
            }
            case 0x4:{_d_b_op = BLT;
              _d_alu_op = ALU_SUB;
              
              int64_t imm = _d_pc + signExtend((getBits(_d_instr,8,11) << 1) + 
                                        (getBits(_d_instr,25,30) << 5) + 
                                        (getBits(_d_instr,7,7) << 11) + 
                                        (getBits(_d_instr,31,31) << 12),13);
              sprintf(instr[ID_D],"blt %u, %u, %lx\n",_d_rs1,_d_rs2,imm);
              
              break;
            }
            case 0x5:{_d_b_op = BGE;
              _d_alu_op = ALU_SUB;
              
              int64_t imm = _d_pc + signExtend((getBits(_d_instr,8,11) << 1) + 
                                        (getBits(_d_instr,25,30) << 5) + 
                                        (getBits(_d_instr,7,7) << 11) + 
                                        (getBits(_d_instr,31,31) << 12),13);
              sprintf(instr[ID_D],"bge %u, %u, %lx\n",_d_rs1,_d_rs2,imm);
              
              break;
            }
            case 0x6:{_d_b_op = BLTU;
              _d_alu_op = ALU_SUB_U;
              
              int64_t imm = _d_pc + signExtend((getBits(_d_instr,8,11) << 1) + 
                                        (getBits(_d_instr,25,30) << 5) + 
                                        (getBits(_d_instr,7,7) << 11) + 
                                        (getBits(_d_instr,31,31) << 12),13);
              sprintf(instr[ID_D],"bltu %u, %u, %lx\n",_d_rs1,_d_rs2,imm);
              
              break;
            }
            case 0x7:{_d_b_op = BGEU;
              _d_alu_op = ALU_SUB_U;
              
              int64_t imm = _d_pc + signExtend((getBits(_d_instr,8,11) << 1) + 
                                        (getBits(_d_instr,25,30) << 5) + 
                                        (getBits(_d_instr,7,7) << 11) + 
                                        (getBits(_d_instr,31,31) << 12),13);
              sprintf(instr[ID_D],"bgeu %u, %u, %lx\n",_d_rs1,_d_rs2,imm);
              
              break;
            }
            default: 
              printf("Instruction not supported!\n");
              return -1;
              break;
          }
          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          _d_rs2_ok = hzdDetect(_d_rs2,_d_s2);
          if( !(_d_rs1_ok && _d_rs2_ok) ) {
            _fd_stall = true;
          }else{
            _fd_stall = false;
            _d_st = TICKING;
          }
          #ifdef DEBUG
          printf(" - ID:%s",instr[ID_D]);
          #endif
          break;
        }
        case OP_AUIPC:{// auipc
          _d_rd = getBits(_d_instr,7,11);
          _d_s1 = 0;
          _d_s2 = 0;
          _d_rs1_ok = _d_rs2_ok = true;
          _d_data = signExtend((getBits(_d_instr,12,31) << 12),32) + _d_pc;
          _d_operand_type = DOUBLE;
          _d_alu_op = NOP;
          _d_mem_op = NOP;
          _d_b_op = NOP;
          
          _d_st = TICKING;
          
          sprintf(instr[ID_D],"auipc %u, $0x%x\n",_d_rd, getBits(_d_instr,12,31));
          
          #ifdef DEBUG
          printf(" - ID: %s",instr[ID_D]);
          #endif
          break;
        }
        case OP_UJ:{
          _d_rd = getBits(_d_instr,7,11);
          _d_s1 = 0;
          _d_s2 = 0;
          _d_rs1_ok = _d_rs2_ok = true;
          _d_data = _d_pc + 4;
          _d_operand_type = DOUBLE;
          _d_alu_op = NOP;
          _d_mem_op = NOP;
          _d_b_op = NOP;

          _d_st = TICKING;

          int64_t imm = (getBits(_d_instr,21,30) << 1) + 
            (getBits(_d_instr,20,20) << 11) + 
            (getBits(_d_instr,12,19) << 12) + 
            (getBits(_d_instr,31,31) << 20);
          imm = signExtend(imm,21) + _d_pc;
          sprintf(instr[ID_D],"jal %u, %lx \n",_d_rd, imm);
          #ifdef DEBUG
          printf(" - ID: %s",instr[ID_D]);
          #endif
          break;
        }
        case OP_LUI:{
          _d_rd = getBits(_d_instr,7,11);
          _d_s1 = 0;
          _d_s2 = 0;
          _d_rs1_ok = _d_rs2_ok = true;
          _d_data = signExtend((getBits(_d_instr,12,31) << 12),32);
          _d_operand_type = DOUBLE;
          _d_alu_op = NOP;
          _d_mem_op = NOP;
          _d_b_op = NOP;
          
          _d_st = TICKING;
          
          sprintf(instr[ID_D],"lui %u, $%ld\n",_d_rd, _d_data);
          #ifdef DEBUG
          printf(" - ID: %s",instr[ID_D]);
          #endif
          break;
        }
        case OP_JALR:{
          _d_rd = getBits(_d_instr,7,11);
          _d_rs1 = getBits(_d_instr,15,19);
          _d_s1 = 0;
          _d_s2 = signExtend(getBits(_d_instr,20,31),12);
          _d_rs2_ok = true;
          _d_data = _d_pc + 4;
          _d_operand_type = DOUBLE;
          _d_alu_op = ALU_ADD;
          _d_mem_op = NOP;
          _d_b_op = NOP;
          // JALR!
          _fetch_bub = true;

          _d_rs1_ok = hzdDetect(_d_rs1,_d_s1);
          if(_d_rs1_ok == true){
            _fd_stall = false;
            _d_st = TICKING;
          }else{
            _fd_stall = true;
          }
          sprintf(instr[ID_D],"jalr %u,%lx(%u)[off(rs1)]\n",_d_rd, _d_s2,_d_rs1);
          #ifdef DEBUG
          printf(" - ID: %s",instr[ID_D]);

          #endif
          break;
        }
        case OP_ECALL:{
          _d_rd = 0;
          _d_rs1 = 0;
          _d_s1 = 0;
          _d_s2 = 0;
          _d_rs2_ok = true;
          _d_rs1_ok = true;
          _d_data = 0;
          _d_operand_type = DOUBLE;
          _d_alu_op = NOP;
          _d_mem_op = NOP;
          _d_b_op = NOP;
          _d_st = TICKING;
          
          //to return from ecall
          _pc_from_ecall = _d_pc + 4;
          _fetch_bub = _f_bub = _fd_bub = true;

          sprintf(instr[ID_D],"ecall\n");
          #ifdef DEBUG
          printf(" - ID: %s",instr[ID_D]);
          #endif
          break;
        }
        default:{
          printf("Instruction %x not supported!\n",_d_instr);
          return ERROR;
        }
      }
      if(_fd_stall) {
        #ifdef DEBUG
        printf("    - ID: _fd_stall \n");
        #endif
        return 0;
      }else{
        _fd_valid = false;
        _fd_instr = 0;
        _fd_pc = 0;
      }

      simClocksToFinish(_d_clk,CLK_ID);
    }
  }
  if( _d_st == TICKING){
    simTick(_d_clk);
    if(simTickDone(_d_clk)) _d_st = DONE;
  }
  return 0;
}

int CPU::EX(){
  if(_e_st == DONE){
    if( !_em_stall && !_em_valid && !_em_bub){
      if(_e_bub){
        _em_bub = true;
        _e_bub = false;
      }
      _em_data = _e_odata;
      _em_data2 = _e_odata2;
      _em_rd = _e_rd;
      _em_mem_op = _e_mem_op;
      _em_operand_type = _e_operand_type;
      _em_opcode = _e_opcode;
      _em_valid = TRUE;
      

      _e_odata = _e_odata2 = _e_rd = _e_mem_op = _e_operand_type = _e_opcode = _e_taken = _e_pc_recover = 0;

      _e_st = READY;
      if(verbose)
        strcpy(instr[EM_D],instr[EX_D]);
      
      sprintf(instr[EX_D],"\n");
      
    }
  }
  if(_e_st == READY){
    if( _de_bub ){
      _e_opcode = OP_BUBBLE;
      _e_bub = _de_bub;
      _de_bub = false;
      _de_valid = false;
      simClocksToFinish(_e_clk,CLK_ONE);
      _e_st = TICKING;
      
      sprintf(instr[EX_D],"Bubble\n");
      sprintf(instr[DE_D],"\n");
      
    }else{
      if(_de_valid){
        _e_s1 = _de_s1;
        _e_s2 = _de_s2;
        _e_b_op = _de_b_op;
        _e_alu_op = _de_alu_op;
        _e_idata = _de_data;
        _e_mem_op = _de_mem_op;
        _e_rd = _de_rd;
        _e_operand_type = _de_operand_type;
        _e_opcode = _de_opcode;
        _e_b_check = false;
        _e_pc_recover = _de_pc_recover;
        _e_taken = _de_taken;

        _de_valid = FALSE;
        _de_s1 = _de_s2 = _de_b_op = _de_alu_op = _de_data = _de_mem_op = _de_rd = _de_operand_type = _de_opcode = _de_pc_recover = _de_taken = 0;

        if(verbose)
        strcpy(instr[EX_D],instr[DE_D]);
        
        sprintf(instr[DE_D],"\n");
        

        switch(_e_alu_op){
          case ALU_ADD: _e_odata = (int64_t)_e_s1 + (int64_t)_e_s2;simClocksToFinish(_e_clk,CLK_S_OP); break;
          case ALU_SUB: _e_odata = (int64_t)_e_s1 - (int64_t)_e_s2;simClocksToFinish(_e_clk,CLK_S_OP); break;
          case ALU_SUB_U: _e_odata = (uint64_t)_e_s1 - (uint64_t)_e_s2;simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_MUL: {
            __int128_t tmp = _e_s1* _e_s2;
            _e_odata = tmp & 0xffffffffffffffff; 
            simClocksToFinish(_e_clk,CLK_M_OP);
            break;}
          case ALU_SLL: _e_odata = _e_s1 << _e_s2; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_MULH: {
            __int128_t tmp = _e_s1* _e_s2;
            _e_odata = tmp >> 64; 
            simClocksToFinish(_e_clk,CLK_M_OP);
            break;}
          case ALU_SLT: _e_odata = ((int64_t)_e_s1 < (int64_t)_e_s2)? 1: 0; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_XOR: _e_odata = _e_s1 ^ _e_s2; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_DIV: _e_odata = _e_s1 / _e_s2; simClocksToFinish(_e_clk,CLK_L_OP);break;
          case ALU_SRL: _e_odata = (uint64_t)_e_s1 >> (int64_t)_e_s2; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_SRA: _e_odata = (int64_t) _e_s1 >> (int64_t)_e_s2; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_OR: _e_odata = _e_s1 | _e_s2; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case ALU_REM: _e_odata = _e_s1 % _e_s2; simClocksToFinish(_e_clk,CLK_L_OP);break;
          case ALU_AND: _e_odata = _e_s1 & _e_s2; simClocksToFinish(_e_clk,CLK_S_OP);break;
          case NOP: _e_odata = _e_idata; simClocksToFinish(_e_clk,CLK_S_OP);break;
          default: 
            printf("ALU op error!"); return ERROR;
          }
        _e_odata2 = _e_idata;
        #ifdef DEBUG
        if(_e_alu_op != NOP) printf(" - EX: s1: 0x%lx, s2: 0x%lx, _e_odata: 0x%lx\n",_e_s1,_e_s2,_e_odata);
        #endif
        _e_st = TICKING;
      }
    }
  }
  if(_e_st == TICKING){
    simTick(_e_clk);
    if(simTickDone(_e_clk)) {
      _e_st = DONE;
      if(_e_opcode == OP_JALR){
        _f_bub = _fd_bub = _e_bub =  _de_bub = true;
        this->pc = _e_odata;
        #ifdef DEBUG
        printf(" - EX: JALR: fwd %x to pc.\n",_e_odata);
        #endif
      }
      if(_e_b_op != NOP && !_e_b_check && hasPredicted(_e_b_op) ){// deal with branch prediction
        statsInc(b_total);
        _e_b_check = true;
        #ifdef DEBUG
          printf("    - EX:tobranch?: %d %d %ld==0x%lx\n",toBranch(_e_b_op,_e_odata),_e_b_op,_e_odata,(uint64_t)_e_odata);
        #endif
        bool _e_toBranch = toBranch(_e_b_op,_e_odata);
        if(_e_toBranch != _e_taken){ // wrong prediction
          #ifdef DEBUG
          printf(" - EX: branch prediction wrong. recovering to PC=0x%lx\n",_e_pc_recover);
          #endif
          statsInc(b_pred_err);
          wrongPred(_e_pc_recover);
        }else{
          #ifdef DEBUG
          printf(" - EX: branch prediction correct.\n");
          #endif
          statsInc(b_pred_crt);
          correctPred();
          }
      }
    }
  }
  return 0;
}

int CPU::MEM(){
  if(_m_st == DONE){
    if( !_mw_stall && !_mw_valid && !_mw_bub){
      if(_m_bub){
        _mw_bub = true;
        _m_bub = false;
      }
      _mw_data = _m_odata;
      _mw_rd = _m_rd;
      _mw_opcode = _m_opcode;
      _mw_valid = TRUE;
      _m_st = READY;

      _m_odata = _m_rd = _m_opcode = 0;
      
    if(verbose)
      strcpy(instr[MW_D],instr[MEM_D]);
    
    sprintf(instr[MEM_D],"\n");
    
    }
  }
  if(_m_st == READY){
    if(_em_bub){
      _m_opcode = OP_BUBBLE;
      _m_bub = _em_bub;
      _em_bub = false;
      _em_valid = false;
      simClocksToFinish(_m_clk,CLK_ONE);
      _m_st = TICKING;
      
      sprintf(instr[MEM_D],"Bubble\n");
      sprintf(instr[EM_D],"\n");
      
    }else{
      if(_em_valid){
        _m_idata = _em_data2;
        _m_addr = _em_data;
        _m_rd = _em_rd;
        _m_mem_op = _em_mem_op;
        _m_operand_type = _em_operand_type;
        _m_opcode = _em_opcode;

        _em_valid = FALSE; // invalidate the former data
        _em_data2= _em_data= _em_rd = _em_mem_op = _em_operand_type = _em_opcode = 0;

        if(verbose)
          strcpy(instr[MEM_D],instr[EM_D]);
        
        sprintf(instr[EM_D],"\n");
        
        
        #ifdef DEBUG
        if(_m_mem_op != NOP){
          printf("    - Mem content check(before): \n      ");
          for(int i = 0 ;i < 8 ;++i){
            printf("%02x ", memory->mem[(uint64_t)_m_addr+i]);
          }
          printf("\n");
        }
        #endif
        switch(_m_mem_op){
          case MREAD:{
            memcpy(&_m_odata, &memory->mem[(uint64_t)_m_addr], sizeof(uint64_t));
            mem_read ++;
            switch(_m_operand_type){
              case BYTE: _m_odata = signExtend(_m_odata, 8); break;
              case HALF: _m_odata = signExtend(_m_odata, 16);break;
              case WORD: _m_odata = signExtend(_m_odata, 32);break;
              case BYTEU: _m_odata = zeroExtend(_m_odata & 0xff); break;
              case HALFU: _m_odata = zeroExtend(_m_odata & 0xffff);break;
              case WORDU: _m_odata = zeroExtend(_m_odata & 0xffffffff); break;
              case DOUBLE:break;
              default: printf(" load type error!\n"); return ERROR;
            }
            
            #ifdef DEBUG
            printf(" - MEM: read data: 0x%lx at addr: 0x%lx\n",(uint64_t)_m_odata,(uint64_t)_m_addr);
            #endif
            
            break;
          }
          case MWRITE:{
            int store_size = 0;
            switch(_m_operand_type){
              case BYTE: store_size = 1; break;
              case HALF: store_size = 2; break;
              case WORD: store_size = 4; break;
              case DOUBLE: store_size = 8;break;
              default: printf(" store type error!\n"); return ERROR;
            }
            memcpy( &memory->mem[(uint64_t)_m_addr],&_m_idata, store_size);
            mem_write ++;
            #ifdef DEBUG
            printf(" - MEM: store data: 0x%lx at addr: 0x%lx\n",(uint64_t)_m_idata,(uint64_t)_m_addr);
            #endif

            break;
          }
          case NOP:{
            // if not load or store , "_m_addr" is the outcome of ALU
            switch(_m_operand_type){
              case WORD: _m_addr = signExtend(_m_addr & 0xffffffff, 32); break;
              case DOUBLE:break;
              default: printf(" mem_op type error!\n"); return ERROR;
            }
            _m_odata = _m_addr;
            break;
          }
        }
        #ifdef DEBUG
        if(_m_mem_op != NOP){
          printf("    - Mem content check(after): \n      ");
          for(int i = 0 ;i < 8 ;++i){
            printf("%02x ", memory->mem[(uint64_t)_m_addr+i]);
          }
          printf("\n");
        }
        #endif
        simClocksToFinish(_m_clk,CLK_MEM);
        _m_st = TICKING;
      }
    }
  }
  if(_m_st == TICKING){
    simTick(_m_clk);
    if(simTickDone(_m_clk)) {_m_st = DONE;}
  }
  return 0;
}

int CPU::WB(){
  if(_w_st == DONE){
    if(_w_opcode != OP_BUBBLE){
      if(_w_rd != 0){
        regfile->greg[_w_rd] = _w_data;
        #ifdef DEBUG
        printf(" - WB: write data 0x%lx back to reg:%d\n",_w_data,_w_rd);
        #endif
      }else{
        #ifdef DEBUG
        printf(" - WB: write nothing\n");
        #endif
      }        
      
    }
    _w_st = READY;
  }
  if(_w_st == READY){
    if(_mw_bub){
      _w_opcode = OP_BUBBLE;
      _w_bub = _mw_bub ;
      _mw_bub = false;
      _mw_valid = false;
      
      sprintf(instr[WB_D],"Bubble\n");
      sprintf(instr[MW_D],"\n");
      
    }else{
      if(_mw_valid){
        if(verbose)
          strcpy(instr[WB_D],instr[MW_D]);
        
        sprintf(instr[MW_D],"\n");
        
        _w_rd = _mw_rd;
        _w_data = _mw_data;
        _w_opcode = _mw_opcode;
        _mw_opcode = _mw_rd = _mw_data = 0;
        
        _mw_valid = FALSE; // Next round data is not ready.
      }//else if _mw_valid == false, do nothing.
    }
    simClocksToFinish(_w_clk,CLK_WB);
    _w_st = TICKING;
  }
  if(_w_st == TICKING){
    simTick(_w_clk);
    if(simTickDone(_w_clk)) {
      _w_st = DONE;
      if(_w_opcode == OP_ECALL){
        _fd_stall = false;
        _fetch_bub = false;
        _fd_bub = _de_bub = _em_bub = _mw_bub = true;
        _f_bub = _d_bub = _e_bub = _m_bub = true;
        this->pc = _pc_from_ecall;
        _pc_from_ecall = 0;
        switch(this->regfile->greg[A7]){
          case 1: return HALT;break;
          case 2: printf("%s",(char*)&this->memory->mem[this->regfile->greg[A0]]);break;
          case 3: printf("%ld ",(long long int)this->regfile->greg[A0]);break;
          default: break;
        }
      }
    }
  }
  return 0;
}