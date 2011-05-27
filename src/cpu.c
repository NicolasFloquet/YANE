#include <stdlib.h>

#include <cpu.h>
#include <memory.h>
#include <opcodes.h>

static cpu_state* current_state = NULL;

cpu_state* get_current_cpu_state() {
	return current_state;
}

void create_cpu_state() {
	cpu_state* state = malloc(sizeof(cpu_state));
	
	state->pc = 0xC000; /* à voir */
	state->sp = 0xFF;
	state->A = 0;
	state->X = 0;
	state->Y = 0;

	state->P.flags.n = 0;
	state->P.flags.v = 0;
	state->P.flags.unused = 0;
	state->P.flags.b = 0;
	state->P.flags.d = 0;
	state->P.flags.i = 0;
	state->P.flags.z = 0;
	state->P.flags.c = 0;

	state->cycle = 0;
	
	current_state = state;
}

void print_cpu_state() {
	printf("PC=0x%x SP=0x%x A=0x%x X=0x%x Y=0x%x P=0x%x (cycles=%d)\n", current_state->pc,
							    current_state->sp,
							    current_state->A,
							    current_state->X,
							    current_state->Y,
							    (char)(current_state->P.byte),
							    current_state->cycle);
}

void step() {
	unsigned char opcode = read_memory(current_state->pc);
	printf("0x%x\t",current_state->pc);
	exec_instruction(opcode);
	print_cpu_state();
}