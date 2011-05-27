#include <stdio.h>
#include <stdlib.h>

#include <memory.h>
#include <opcodes.h>
#include <cpu.h>

#define read_memory16(addr) (((unsigned short int)read_memory(addr+1)<<8) | ((unsigned short int)read_memory(addr)&0xFF))

void bcc(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(!GET_CARRY(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void bcs(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(GET_CARRY(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void beq(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(GET_ZERO(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void bit(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	unsigned char data;
	
	switch(mode) {
	    case AM_ZERO:
			data = read_memory((int)read_memory(state->pc+1));

			state->P &= 0x3f;
			state->P |= (data & 0xC0); /* D7->SIGN, D6->OVERF */

			if((data & state->A)==0) {
				SET_ZERO(state->P);
			}
			else {
				CLEAR_ZERO(state->P);
			}
			
			state->pc += 2;
			state->cycle += 3;
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void bne(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(!GET_ZERO(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void bpl(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(!GET_SIGN(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void bvc(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(!GET_OVERF(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void bvs(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_REL:
			if(GET_OVERF(state->P)) {
				state->pc += 2+(signed char)read_memory(state->pc+1);
			}
			else {
				state->pc += 2;
			}
			state->cycle += 2; /* +1 si on saute dans la meme page, +2 si on saute sur la page suivante */
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void clc(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    CLEAR_CARRY(state->P);
    state->pc += 1;
    state->cycle += 2;
}
void cld(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    CLEAR_BCD(state->P);
    state->pc += 1;
    state->cycle += 2;
}
void cli(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    CLEAR_INTERRUPT(state->P);
    state->pc += 1;
    state->cycle += 2;
}
void lda(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_IMM:    
			state->A = read_memory(state->pc+1);
			state->pc += 2;
			state->cycle += 2;
			break;
	    case AM_ZERO:    
			state->A = read_memory((int)read_memory(state->pc+1));
			state->pc += 2;
			state->cycle += 3;
			break;
	    case AM_ZEROX:    
			state->A = read_memory(((int)read_memory(state->pc+1) + state->X)%0x100);
			state->pc += 2;
			state->cycle += 4;
			break;
		case AM_ABS:
			state->A = read_memory( read_memory16(state->pc+1) );
			state->pc += 3;
			state->cycle += 4;
			break;
	    case AM_ABSX:
			state->A = read_memory( read_memory16(state->pc+1) + state->X );
			state->pc += 3;
			state->cycle += 4; /* *** "Add 1 when page boundary is crossed." *** */
			break;
		case AM_ABSY:
			state->A = read_memory( read_memory16(state->pc+1) + state->Y );
			state->pc += 3;
			state->cycle += 4; /* *** "Add 1 when page boundary is crossed." *** */
			break;
		case AM_INDX:
			state->A = read_memory(read_memory16((read_memory(state->pc+1)+state->X)));
			state->pc += 2;
			state->cycle += 6;
			break;
		case AM_INDY:
			state->A = read_memory(read_memory16(read_memory(state->pc+1))+state->Y);
			state->pc += 2;
			state->cycle += 5; /* *** "Add 1 when page boundary is crossed." *** */
			break;
	    default:
		printf("invalid addressing mode");
	}

	if(state->A==0)
		SET_ZERO(state->P);
	else
		CLEAR_ZERO(state->P);
		
	if(state->A & 0x80)
	    SET_SIGN(state->P);
	else
		CLEAR_SIGN(state->P);
}
void ldx(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_IMM:    
			state->X = read_memory(state->pc+1);
			state->pc += 2;
			state->cycle += 2;
			break;
	    case AM_ZERO:    
			state->X = read_memory((int)read_memory(state->pc+1));
			state->pc += 2;
			state->cycle += 3;
			break;
	    case AM_ZEROY:    
			state->X = read_memory(((int)read_memory(state->pc+1) + state->Y)%0x100);
			state->pc += 2;
			state->cycle += 4;
			break;
		case AM_ABS:
			state->X = read_memory( read_memory16(state->pc+1) );
			state->pc += 3;
			state->cycle += 4;
			break;
	    case AM_ABSY:
			state->X = read_memory( read_memory16(state->pc+1) + state->Y );
			state->pc += 3;
			state->cycle += 4; /* *** "Add 1 when page boundary is crossed." *** */
			break;
	    default:
		printf("invalid addressing mode");
	}

	if(state->X==0)
		SET_ZERO(state->P);
	else
		CLEAR_ZERO(state->P);
		
	if(state->X<0)
	    SET_SIGN(state->P);
	else
		CLEAR_SIGN(state->P);
}
void jmp(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_ABS:    
		state->pc = read_memory16(state->pc+1);
		state->cycle += 3;
		break;
	    case AM_REL:
		printf("AM_REL not implemented yet.");
		state->cycle += 5;
		break;
	    default:
		printf("invalid addressing mode");
	}
}
void jsr(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();

	stack_push((state->pc+2) & 0x00FF);
	stack_push(((state->pc+2) & 0xFF00) >> 8);
	state->pc = read_memory16(state->pc+1);
	state->cycle += 6;
}
void php(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();

	stack_push(state->P);
	state->pc += 1;
	state->cycle += 3;
}
void pla(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();

	state->A = stack_pop();
	state->pc += 1;
	state->cycle += 4;
}
void nop(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    state->pc += 1;
    state->cycle += 2;
}
void rts(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();

	state->pc = (stack_pop()<<8)+stack_pop()+1;
	state->cycle += 6;
}
void sec(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    SET_CARRY(state->P);
    state->pc += 1;
    state->cycle += 2;
}
void sed(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    SET_BCD(state->P);
    state->pc += 1;
    state->cycle += 2;
}
void sei(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();

    SET_INTERRUPT(state->P);
    state->pc += 1;
    state->cycle += 2;
}
void sta(addr_mode mode) {
	cpu_state* state = get_current_cpu_state();
	switch(mode) {
	    case AM_ZERO:    
			write_memory((int)read_memory(state->pc+1), state->A);
			state->pc += 2;
			state->cycle += 3;
			break;
	    case AM_ZEROX:
			write_memory(((int)read_memory(state->pc+1) + state->X)%0x100, state->A);
			state->pc += 2;
			state->cycle += 4;
			break;
		case AM_ABS:
			write_memory( read_memory16(state->pc+1), state->A);
			state->pc += 3;
			state->cycle += 4;
			break;
	    case AM_ABSX:
			write_memory( read_memory16(state->pc+1) + state->X, state->A);
			state->pc += 3;
			state->cycle += 5;
			break;
		case AM_ABSY:
			write_memory( read_memory16(state->pc+1) + state->Y, state->A);
			state->pc += 3;
			state->cycle += 5;
			break;
		case AM_INDX:
			write_memory(read_memory16((read_memory(state->pc+1)+state->X)), state->A);
			state->pc += 2;
			state->cycle += 6;
			break;
		case AM_INDY:
			write_memory(read_memory16(read_memory(state->pc+1))+state->Y, state->A);
			state->pc += 2;
			state->cycle += 6;
			break;
	    default:
			printf("invalid addressing mode");
	}
}
void stx(addr_mode mode) {
    cpu_state* state = get_current_cpu_state();
    switch(mode) {
    	case AM_ZERO:    
	    write_memory((int)read_memory(state->pc+1), state->X);
	    state->pc += 2;
	    state->cycle += 3;
	    break;
    	case AM_ZEROY:    
	    write_memory(((int)read_memory(state->pc+1) + state->Y)%0x100, state->X);
	    state->pc += 2;
	    state->cycle += 4;
	    break;
	case AM_ABS:
	    write_memory( read_memory16(state->pc+1), state->X);
	    state->pc += 3;
	    state->cycle += 4;
	default:
	    printf("invalid addressing mode");
    }
}

Instruction instruction_list[57]={
	{"UNK", NULL},
	{"ADC", NULL},{"AND", NULL},{"ASL", NULL},{"BCC", bcc},{"BCS", bcs},
	{"BEQ", beq},{"BIT", bit},{"BMI", NULL},{"BNE", bne},{"BPL", bpl},
	{"BRK", NULL},{"BVC", bvc},{"BVS", bvs},{"CLC", clc},{"CLD", cld},
	{"CLI", cli},{"CLV", NULL},{"CMP", NULL},{"CPX", NULL},{"CPY", NULL},
	{"DEC", NULL},{"DEX", NULL},{"DEY", NULL},{"EOR", NULL},{"INC", NULL},
	{"INX", NULL},{"INY", NULL},{"JMP", jmp},{"JSR", jsr},{"LDA", lda},
	{"LDX", ldx},{"LDY", NULL},{"LSR", NULL},{"NOP", nop},{"ORA", NULL},
	{"PHA", NULL},{"PHP", php},{"PLA", pla},{"PLP", NULL},{"ROL", NULL},
	{"ROR", NULL},{"RTI", NULL},{"RTS", rts},{"SBC", NULL},{"SEC", sec},
	{"SED", sed},{"SEI", sei},{"STA", sta},{"STX", stx},{"STY", NULL},
	{"TAX", NULL},{"TAY", NULL},{"TSX", NULL},{"TXA", NULL},{"TXS", NULL},
	{"TYA", NULL}

};

OpCode opcode_list[0x100]={
	{0x0, AM_NONE, I_BRK},
	{0x1, AM_INDX, I_ORA},
	{0x2,0,0}, /* UNUSED */
	{0x3,0,0}, /* UNUSED */
	{0x4,0,0}, /* UNUSED */
	{0x5, AM_ZERO, I_ORA},
	{0x6, AM_ZERO, I_ASL},
	{0x7,0,0}, /* UNUSED */
	{0x8, AM_NONE, I_PHP},
	{0x9, AM_IMM, I_ORA},
	{0xa, AM_NONE, I_ASL},
	{0xb,0,0}, /* UNUSED */
	{0xc,0,0}, /* UNUSED */
	{0xd, AM_ABS, I_ORA},
	{0xe, AM_ABS, I_ASL},
	{0xf,0,0}, /* UNUSED */
	{0x10, AM_REL, I_BPL},
	{0x11, AM_INDY, I_ORA},
	{0x12,0,0},/* UNUSED */
	{0x13,0,0},/* UNUSED */
	{0x14,0,0},/* UNUSED */
	{0x15, AM_ZEROX, I_ORA},
	{0x16, AM_ZEROX, I_ASL},
	{0x17,0,0},/* UNUSED */
	{0x18, AM_NONE, I_CLC},
	{0x19, AM_ABSY, I_ORA},
	{0x1a,0,0},/* UNUSED */
	{0x1b,0,0},/* UNUSED */
	{0x1c,0,0},/* UNUSED */
	{0x1d, AM_ABSX, I_ORA},
	{0x1e, AM_ABSX, I_ASL},
	{0x1f,0,0},/* UNUSED */
	{0x20, AM_ABS, I_JSR},
	{0x21, AM_INDX, I_AND},
	{0x22,0,0},/* UNUSED */
	{0x23,0,0},/* UNUSED */
	{0x24, AM_ZERO, I_BIT},
	{0x25, AM_ZERO, I_AND},
	{0x26, AM_ZERO, I_ROL},
	{0x27,0,0},/* UNUSED */
	{0x28, AM_NONE, I_PLP},
	{0x29, AM_IMM, I_AND},
	{0x2a, AM_NONE, I_ROL},
	{0x2b,0,0},/* UNUSED */
	{0x2c, AM_ABS, I_BIT},
	{0x2d, AM_ABS, I_AND},
	{0x2e, AM_ABS, I_ROL},
	{0x2f,0,0},/* UNUSED */
	{0x30, AM_REL, I_BMI}, /* ??? */
	{0x31, AM_INDY, I_AND},
	{0x32,0,0},/* UNUSED */
	{0x33,0,0},/* UNUSED */
	{0x34,0,0},/* UNUSED */
	{0x35, AM_INDX, I_AND},
	{0x36, AM_ZEROX, I_ROL},
	{0x37,0,0},/* UNUSED */
	{0x38, AM_NONE, I_SEC},
	{0x39, AM_ABSY, I_AND},
	{0x3a,0,0},/* UNUSED */
	{0x3b,0,0},/* UNUSED */
	{0x3c,0,0},/* UNUSED */
	{0x3d, AM_ABSX, I_AND},
	{0x3e, AM_ABSX, I_ROL},
	{0x3f,0,0},/* UNUSED */
	{0x40, AM_NONE, I_RTI},
	{0x41, AM_INDX, I_EOR},
	{0x42,0,0},/* UNUSED */
	{0x43,0,0},/* UNUSED */
	{0x44,0,0},/* UNUSED */
	{0x45, AM_ZERO, I_EOR},
	{0x46, AM_ZERO, I_LSR},
	{0x47,0,0},/* UNUSED */
	{0x48, AM_NONE, I_PHA},
	{0x49, AM_IMM, I_EOR},
	{0x4a, AM_NONE, I_LSR},
	{0x4b,0,0},/* UNUSED */
	{0x4c, AM_ABS, I_JMP},
	{0x4d, AM_ABS, I_EOR},
	{0x4e, AM_ABS, I_LSR},
	{0x4f,0,0},/* UNUSED */
	{0x50, AM_REL, I_BVC},
	{0x51, AM_INDY, I_EOR},
	{0x52,0,0},/* UNUSED */
	{0x53,0,0},/* UNUSED */
	{0x54,0,0},/* UNUSED */
	{0x55, AM_ZEROX, I_EOR},
	{0x56, AM_ZEROX, I_LSR},
	{0x57,0,0},/* UNUSED */
	{0x58, AM_NONE, I_CLI},
	{0x59, AM_ABSY, I_EOR},
	{0x5a,0,0},/* UNUSED */
	{0x5b,0,0},/* UNUSED */
	{0x5c,0,0},/* UNUSED */
	{0x5d, AM_ABSX, I_EOR},
	{0x5e, AM_ABSX, I_LSR},
	{0x5f,0,0},/* UNUSED */ 
	{0x60, AM_NONE, I_RTS},
	{0x61, AM_INDX, I_ADC},
	{0x62,0,0},/* UNUSED */ 
	{0x63,0,0},/* UNUSED */ 
	{0x64,0,0},/* UNUSED */ 
	{0x65, AM_ZERO, I_ADC},
	{0x66, AM_ZERO, I_ROR},
	{0x67,0,0},/* UNUSED */ 
	{0x68, AM_NONE, I_PLA},
	{0x69, AM_IMM, I_ADC},
	{0x6a, AM_NONE, I_ROR},
	{0x6b,0,0},/* UNUSED */ 
	{0x6c, AM_REL, I_JMP}, /* UNUSED ???*/ 
	{0x6d, AM_ABS, I_ADC},
	{0x6e, AM_ABS, I_ROR},
	{0x6f,0,0},/* UNUSED */ 
	{0x70, AM_REL, I_BVS},
	{0x71, AM_INDY, I_ADC},
	{0x72,0,0},/* UNUSED */ 
	{0x73,0,0},/* UNUSED */ 
	{0x74,0,0},/* UNUSED */ 
	{0x75, AM_ZEROX, I_ADC},
	{0x76, AM_ZEROX, I_ROR},
	{0x77,0,0},/* UNUSED */ 
	{0x78, AM_NONE, I_SEI},
	{0x79, AM_ABSY, I_ADC},
	{0x7a,0,0},/* UNUSED */ 
	{0x7b,0,0},/* UNUSED */ 
	{0x7c,0,0},/* UNUSED */ 
	{0x7d, AM_ABSX, I_ADC},
	{0x7e, AM_ABSX, I_ROR},
	{0x7f,0,0},/* UNUSED */ 
	{0x80,0,0},/* UNUSED */ 
	{0x81, AM_INDX, I_STA},
	{0x82,0,0},/* UNUSED */ 
	{0x83,0,0},/* UNUSED */ 
	{0x84, AM_ZERO, I_STY},
	{0x85, AM_ZERO, I_STA},
	{0x86, AM_ZERO, I_STX},
	{0x87,0,0},/* UNUSED */ 
	{0x88, AM_NONE, I_DEY},
	{0x89,0,0},/* UNUSED */ 
	{0x8a, AM_NONE, I_TXA},
	{0x8b,0,0},/* UNUSED */ 
	{0x8c, AM_ABS, I_STY},
	{0x8d, AM_ABS, I_STA},
	{0x8e, AM_ABS, I_STX},
	{0x8f,0,0},/* UNUSED */ 
	{0x90, AM_REL, I_BCC},
	{0x91,0,0},
	{0x92,0,0},
	{0x93,0,0},
	{0x94,0,0},
	{0x95,0,0},
	{0x96,0,0},
	{0x97,0,0},
	{0x98, AM_NONE, I_TYA},
	{0x99,0,0},
	{0x9a, AM_NONE, I_TXS},
	{0x9b,0,0},
	{0x9c,0,0},
	{0x9d,0,0},
	{0x9e,0,0},
	{0x9f,0,0},
	{0xa0, AM_IMM, I_LDY},
	{0xa1, AM_INDX, I_LDA},
	{0xa2, AM_IMM, I_LDX},
	{0xa3,0,0},
	{0xa4, AM_ZERO, I_LDY},
	{0xa5, AM_ZERO, I_LDA},
	{0xa6, AM_ZERO, I_LDX},
	{0xa7,0,0},
	{0xa8, AM_NONE, I_TAY},
	{0xa9, AM_IMM, I_LDA},
	{0xaa, AM_NONE, I_TAX},
	{0xab,0,0},
	{0xac, AM_ABS, I_LDY},
	{0xad, AM_ABS, I_LDA},
	{0xae, AM_ABS, I_LDX},
	{0xaf,0,0},
	{0xb0, AM_REL, I_BCS},
	{0xb1, AM_INDY, I_LDA},
	{0xb2,0,0},
	{0xb3,0,0},
	{0xb4, AM_ZEROX, I_LDY},
	{0xb5, AM_ZEROX, I_LDA},
	{0xb6, AM_ZEROY, I_LDX},
	{0xb7,0,0},
	{0xb8, AM_NONE, I_CLV},
	{0xb9, AM_ABSY, I_LDA},
	{0xba, AM_NONE, I_TSX},
	{0xbb,0,0},
	{0xbc, AM_ABSX, I_LDY},
	{0xbd, AM_ABSX, I_LDA},
	{0xbe, AM_ABSY, I_LDX},
	{0xbf,0,0},
	{0xc0, AM_IMM, I_CPY},
	{0xc1, AM_INDX, I_CMP},
	{0xc2,0,0},
	{0xc3,0,0},
	{0xc4, AM_ZERO, I_CPY},
	{0xc5, AM_ZERO, I_CMP},
	{0xc6, AM_ZERO, I_DEC},
	{0xc7,0,0},
	{0xc8, AM_NONE, I_INY},
	{0xc9, AM_IMM, I_CMP},
	{0xca, AM_NONE, I_DEX},
	{0xcb,0,0},
	{0xcc, AM_ABS, I_CPY},
	{0xcd, AM_ABS, I_CMP},
	{0xce, AM_ABS, I_DEC},
	{0xcf,0,0},
	{0xd0, AM_REL, I_BNE},
	{0xd1, AM_INDX, I_CMP},
	{0xd2,0,0},
	{0xd3,0,0},
	{0xd4,0,0},
	{0xd5, AM_ZEROX, I_CMP},
	{0xd6, AM_ZEROX, I_DEC},
	{0xd7,0,0},
	{0xd8, AM_NONE, I_CLD},
	{0xd9, AM_ABSY, I_CMP},
	{0xda,0,0},
	{0xdb,0,0},
	{0xdc,0,0},
	{0xdd, AM_ABSX, I_CMP},
	{0xde, AM_ABSX, I_DEC},
	{0xdf,0,0},
	{0xe0, AM_IMM, I_CPX},
	{0xe1, AM_INDX, I_SBC},
	{0xe2,0,0},
	{0xe3,0,0},
	{0xe4, AM_ZERO, I_CPX},
	{0xe5, AM_ZERO, I_SBC},
	{0xe6, AM_ZERO, I_INC},
	{0xe7,0,0},
	{0xe8, AM_NONE, I_INX},
	{0xe9, AM_IMM, I_SBC},
	{0xea, AM_NONE, I_NOP},
	{0xeb,0,0},
	{0xec, AM_ABS, I_CPX},
	{0xed, AM_ABS, I_SBC},
	{0xee, AM_ABS, I_INC},
	{0xef,0,0},
	{0xf0, AM_REL, I_BEQ},
	{0xf1, AM_INDX, I_SBC},
	{0xf2,0,0},
	{0xf3,0,0},
	{0xf4,0,0},
	{0xf5, AM_ZEROX, I_SBC},
	{0xf6, AM_ZEROX, I_INC},
	{0xf7,0,0},/* UNUSED  (I_INC??)*/ 
	{0xf8, AM_NONE, I_SED},
	{0xf9, AM_ABSY, I_SBC},
	{0xfa,0,0},/* UNUSED */ 
	{0xfb,0,0},/* UNUSED */ 
	{0xfc,0,0},/* UNUSED */ 
	{0xfd, AM_ABSX, I_SBC},
	{0xfe, AM_ABSX, I_INC},
	{0xff,0,0},/* UNUSED */ 
};

addr_mode get_addr_mode(unsigned char opcode) {
    return opcode_list[opcode].mode;
}

void exec_instruction(unsigned char opcode) {
    if(instruction_list[opcode_list[opcode].inst].handler == NULL) {
	printf("%s not implemented.\t", instruction_list[opcode_list[opcode].inst].name);
    }
    else {
	printf("%s\t",instruction_list[opcode_list[opcode].inst].name);
	instruction_list[opcode_list[opcode].inst].handler(opcode_list[opcode].mode); 
    }
	
}

void disassemble() {
	unsigned char opcode;
	int i = 0;
	printf("----- STARTING DISASSEMBLY -----\n");
	int tmp;
	NES_ROM* rom = get_current_rom();
	while( i < rom->header->prg_pcount*PRG_ROM_SIZE )
	{
		opcode = rom->prg_rom[i];
		printf("0x%x\t%s\t",i+0x8000, instruction_list[opcode_list[opcode].inst].name);
		
		switch(opcode_list[opcode].mode) {
			case AM_NONE:
				if(opcode_list[opcode].inst == I_UNK)
					printf("(0x%x)", opcode);
				else
					printf("(none)");			
				i+=1;
				break;
			case AM_REL:
				printf("$%x\t(relative)", rom->prg_rom[i+1]);
				i+=2;
				break;
			case AM_IMM:
				printf("$%x\t(immediate)", rom->prg_rom[i+1]);
				i+=2;
				break;
			case AM_ZERO:
				printf("$%x\t(zero)", rom->prg_rom[i+1]);
				i+=2;
				break;
			case AM_ABS:
				tmp = (int)rom->prg_rom[i+2];
				tmp = tmp << 8;
				tmp |= (int)rom->prg_rom[i+1];
				printf("$%x\t(%s)",tmp, zone_name(tmp));				
				i+=3;
				break;
			case AM_ZEROX:
				printf("$%x\t(zerox)", rom->prg_rom[i+1]);
				i+=2;
				break;
			case AM_ZEROY:
				printf("$%x\t(zeroy)", rom->prg_rom[i+1]);
				i+=2;
				break;
			case AM_ABSX:
				tmp = (int)rom->prg_rom[i+2];
				tmp = tmp << 8;
				tmp |= (int)rom->prg_rom[i+1];
				printf("X+$%x\t(%s)", tmp, zone_name(tmp));
				i+=3;
				break;
			case AM_ABSY:	
				tmp = (int)rom->prg_rom[i+2];
				tmp = tmp << 8;
				tmp |= (int)rom->prg_rom[i+1];
				printf("Y+$%x\t(%s)", tmp, zone_name(tmp));
				i+=3;		
				break;
			case AM_INDX:
				printf("($%x)\t(indirect x)", rom->prg_rom[i+1]);
				i+=2;
				break;
			case AM_INDY:
				printf("($%x)\t(indirect y)", rom->prg_rom[i+1]);
				i+=2;
				break;
			default:
				printf("Wait... wut? (0x%x)",opcode_list[opcode].mode);
		}
		printf("\n");
	}
}
