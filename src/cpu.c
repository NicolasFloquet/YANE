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
	
	state->pc = read_memory16(0xfffc); /* à voir */
	state->sp = 0xFD;
	state->A = 0;
	state->X = 0;
	state->Y = 0;

	state->P = 0x24; /* Le 5ième bit n'es pas utilisé et toujours set à 1, et interruptions désactivées au début */
	state->br = 0;

	state->cycle = 0;
	
	current_state = state;
}

void print_cpu_state() {
	printf("A=0x%x X=0x%x Y=0x%x P=0x%x SP=0x%x CY=%d", 
							    current_state->A,
							    current_state->X,
							    current_state->Y,
							    current_state->P,
							    current_state->sp,
							    current_state->cycle);
}

void step() {
	unsigned char opcode = read_memory(current_state->pc);
	//print_instruction();
	exec_instruction(opcode);
}

void cpu_nmi() {
	
	//printf("---NMI---\n");
	current_state->cycle = 0;

	stack_push(current_state->P);
	stack_push(current_state->pc & 0xFF);
	stack_push((current_state->pc>>8) & 0xFF);
	current_state->pc = read_memory16(0xfffa);
}

int cpu_get_cycles() {
	return current_state->cycle;
}

void runto(unsigned short int addr)
{
	while(current_state->pc != addr) {
		step();
		printf("\n");
	}
}
