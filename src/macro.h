/* macro.h
 * the necessary macro is defined here.
 * Author: Ye Zilingfeng
 */

#ifndef MACRO_H
#define MACRO_H
#include<iostream>
#include<cstdio>
#include<cstring>
#include<string>

typedef unsigned int uint_t;
typedef unsigned long int uint64_t;
typedef long int int64_t;

// CPU Status
#define ERROR -1
#define HALT -2

// ELF file program header type
#define PT_LOAD 1

// Memory configuration
#define MEMORY_SIZE 1<<20

// reg file name
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


// CPU internal signal
// - nop: no operation
#define NOP 0x0

// - opcodes
#define OP_R 0x33
#define OP_LOAD 0x03
#define OP_SB 0x63
#define OP_STORE 0x23
#define OP_IMM 0x13
#define OP_JALR 0x67
#define OP_AUIPC 0x17
#define OP_UJ 0x6f // unconditional jump
#define OP_LUI 0x37
#define OP_R_W 0x3b // rv64i only
#define OP_IMM_W 0x1b //rv64i only
#define OP_ECALL 0x73 // system call

// - alu_op: ALU control
#define ALU_ADD 0x1
#define ALU_SUB 0x2
#define ALU_MUL 0x3
#define ALU_SLL 0x4
#define ALU_MULH 0x5
#define ALU_SLT 0x6
#define ALU_XOR 0x7
#define ALU_DIV 0x8
#define ALU_SRA 0x9
#define ALU_SRL 0xa
#define ALU_OR 0xb
#define ALU_REM 0xc
#define ALU_AND 0xd
#define ALU_SUB_U 0xe

// - b_op: branch control
#define TAKE_BRANCH 0x1
#define BNE 0x1
#define JAL 0x2
#define BEQ 0x3
#define BLT 0x4
#define BGE 0x5
#define BLTU 0x6
#define BGEU 0x7
#define JALR 0x8

// - pc_source
#define PC_NEXT 0x1
#define PC_IMM 0x2

// - mem_op: memory control
#define MREAD 0x1
#define MWRITE 0x2

// - wb_op: write back control
#define WB_ALU 0x1
#define WB_MEM 0x2
#define WB_PC 0x8
#define WB_IMM 0x4

// - load type
#define LDOUBLE 0x1
#define LWORD 0x2
#define LHALF 0x3
#define LBYTE 0x4
#define LWORDU 0x5
#define LHALFU 0x6
#define LBYTEU 0x7

// - store type
#define SDOUBLE 0x1
#define SWORD 0x2
#define SHALF 0x3
#define SBYTE 0x4
#define SWORDU 0x5
#define SHALFU 0x6
#define SBYTEU 0x7

// - instruction operands type
#define DOUBLE 0x1
#define WORD 0x2
#define HALF 0x3
#define BYTE 0x4

#define FALSE 0
#define TRUE 1

#define DELAY 0x0


// regfile configuration
#define REG_CNT 32


#define INSTR_SIZE 4


#define DONE 1
#define EXECUTE 2

#endif