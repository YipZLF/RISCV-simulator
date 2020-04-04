// IF stage cycles
#define CLK_IF 1

// ID stage cycles
#define CLK_ID 1

// EX stage cycles

// for add, sub, subu
#define CLK_ADD_OP 1
// for mul,mulh
#define CLK_MUL_OP 2
#define CLK_DIV_OP 4
#define CLK_REM_OP 4
#define CLK_XOR_OP 1
#define CLK_OR_OP 1
#define CLK_AND_OP 1
// for sll,srl,sra
#define CLK_SHIFT_OP 2

// WB stage cycles
#define CLK_WB 1

// MEM stage cycles
// interface for Cache implementation
#define CLK_MEM 2

#define CLK_ONE 1
