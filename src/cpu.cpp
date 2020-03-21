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
              printf(" - ID: slli %u, %u,$%ld\n",rd,rs1,signExtend(imm,imm_size));
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
  if(b_op != NOP) printf("    - tobranch?: %d %d %d\n",toBranch(b_op,alu_d),b_op,alu_d);
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
void CPU::IF()
{
}

//译码
void CPU::ID()
{

} 

//执行
void CPU::EX()
{
   //.....
}

//访问存储器
void CPU::MEM()
{
	//read EX_MEM

	//complete Branch instruction PC change

	//read / write memory

	//write MEM_WB_old
}


//写回
void CPU::WB()
{
	//read MEM_WB

	//write reg
}
/*
//取指令
void CPU::IF()
{
	//write IF_ID_old
	IF_ID_old.inst=memory[PC];
	PC=PC+1;
	IF_ID_old.PC=PC;
}

//译码
void CPU::ID()
{
	//Read IF_ID
	unsigned int inst=IF_ID.inst;
	int EXTop=0;
	unsigned int EXTsrc=0;

	char RegDst,ALUop,ALUSrc;
	char Branch,MemRead,MemWrite;
	char RegWrite,MemtoReg;

	rd=getbit(inst,7,11);
	fuc3=getbit(inst,0,0);
	//....


	if(OP==OP_R)
	{
		if(fuc3==F3_ADD&&fuc7==F7_ADD)
		{
            EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
		}
		else
		{
		   
		}
	}
	else if(OP==OP_I)
    {
        if(fuc3==F3_ADDI)
        {
            
        }
        else
        {
           
        }
    }
    else if(OP==OP_SW)
    {
        if(fuc3==F3_SB)
        {
           
        }
        else
        {
           
        }
    }
    else if(OP==OP_LW)
    {
        if(fuc3==F3_LB)
        {
			
        }
        else
        {

        }
    }
    else if(OP==OP_BEQ)
    {
        if(fuc3==F3_BEQ)
        {
			
        }
        else
        {
           
        }
    }
    else if(OP==OP_JAL)
    {
        
    }
    else
    {
		
    }

	//write ID_EX_old
	ID_EX_old.Rd=rd;
	ID_EX_old.Rt=rt;
	ID_EX_old.Imm=ext_signed(EXTsrc,EXTop);
	//...

	ID_EX_old.Ctrl_EX_ALUOp=ALUop;
	//....

}

//执行
void CPU::EX()
{
	//read ID_EX
	int temp_PC=ID_EX.PC;
	char RegDst=ID_EX.Ctrl_EX_RegDst;
	char ALUOp=ID_EX.Ctrl_EX_ALUOp;

	//Branch PC calulate
	//...

	//choose ALU input number
	//...

	//alu calculate
	int Zero;
	REG ALUout;
	switch(ALUOp){
	default:;
	}

	//choose reg dst address
	int Reg_Dst;
	if(RegDst)
	{

	}
	else
	{

	}

	//write EX_MEM_old
	EX_MEM_old.ALU_out=ALUout;
	EX_MEM_old.PC=temp_PC;
    //.....
}

//访问存储器
void CPU::MEM()
{
	//read EX_MEM

	//complete Branch instruction PC change

	//read / write memory

	//write MEM_WB_old
}


//写回
void CPU::WB()
{
	//read MEM_WB

	//write reg
}*/