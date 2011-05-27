#ifndef _OPCODES_H
#define _OPCODES_H

#include <stdio.h>
#include <rom.h>

typedef enum {
	AM_NONE,
	AM_REL,
	AM_IMM,
	AM_ZERO,
	AM_ABS,
	AM_ZEROX,
	AM_ZEROY,
	AM_ABSX,
	AM_ABSY,
	AM_INDX,
	AM_INDY
}addr_mode;

typedef enum {
    I_UNK,
    I_ADC,
    I_AND,
    I_ASL,
    I_BCC,
    I_BCS,
    I_BEQ,
    I_BIT,
    I_BMI,
    I_BNE,
    I_BPL,
    I_BRK,
    I_BVC,
    I_BVS,
    I_CLC,
    I_CLD,
    I_CLI,
    I_CLV,
    I_CMP,
    I_CPX,
    I_CPY,
    I_DEC,
    I_DEX,
    I_DEY,
    I_EOR,
    I_INC,
    I_INX,
    I_INY,
    I_JMP,
    I_JSR,
    I_LDA,
    I_LDX,
    I_LDY,
    I_LSR,
    I_NOP,
    I_ORA,
    I_PHA,
    I_PHP,
    I_PLA,
    I_PLP,
    I_ROL,
    I_ROR,
    I_RTI,
    I_RTS,
    I_SBC,
    I_SEC,
    I_SED,
    I_SEI,
    I_STA,
    I_STX,
    I_STY,
    I_TAX,
    I_TAY,
    I_TSX,
    I_TXA,
    I_TXS,
    I_TYA
}Instruction_Index;

typedef void(*op_handler)(addr_mode);

typedef struct {
    char* name;
    op_handler handler;
}Instruction;

typedef struct {
    unsigned char op_code;
    addr_mode mode;
    Instruction_Index inst;
}OpCode;

addr_mode get_addr_mode(unsigned char opcode);
void exec_instruction(unsigned char opcode);
void disassemble();

#endif /* _ROM_H */
