#include <stdio.h>
#include <stdlib.h>

#include <memory.h>
#include <rom.h>
#include <cpu.h>

static unsigned char* RAM = NULL;

static unsigned char ram_read(unsigned short int addr) {
    return RAM[addr%0x800];
}

static void ram_write(unsigned short int addr, unsigned char data) {
    RAM[addr%0x800] = data;
}

static const memory_zone memory_map[] = {
    {0x0000, 0x00ff, "zero page", ram_read, ram_write},
    {0x0100, 0x01ff, "stack", ram_read, ram_write},
    {0x0000, 0x00ff, "zero page", ram_read, ram_write},
    {0x0200, 0x07ff, "RAM", ram_read, ram_write},
    {0x0800, 0x1fff, "RAM mirror", ram_read, ram_write},
    {0x2000, 0x2007, "IO regs1", NULL, NULL},
    {0x2008, 0x3fff, "IO mirror", NULL, NULL},
    {0x4000, 0x401f, "IO regs2", NULL, NULL},
    {0x4020, 0x5fff, "exp RAM", NULL, NULL},
    {0x6000, 0x7fff, "SRAM", NULL, NULL},
    {0x8000, 0xbfff, "PRG-ROM low bank", prg_read, NULL},	/* ROM => read-only */
    {0xc000, 0xffff, "PGR-ROM high bank", prg_read, NULL}	/* ROM => read-only */
};


void init_ram() {
    RAM = malloc(0x800);
}

static int find_zone(unsigned short int addr) {
    int i = 0;

    while( !( memory_map[i].start <= addr && addr <= memory_map[i].end))
	i++;

    return i;
}

char* zone_name(unsigned short int addr) {
    return memory_map[find_zone(addr)].name;
}

char read_memory(unsigned short int addr) {
    int i = find_zone(addr);
    char ret = 0;

    if(memory_map[i].reader == NULL)
	printf("Reader for %s is not implemented yet.\n", memory_map[i].name);
    else
	ret = memory_map[i].reader(addr);

    return ret;
}

void write_memory(unsigned short int addr, unsigned char data) {
    int i = find_zone(addr);

    if(memory_map[i].reader == NULL)
	printf("Writer for %s is not implemented yet.\n", memory_map[i].name);
    else
	memory_map[i].writer(addr,data);
}

/* Un peu trop hardcodé à mon gout */
void stack_push(unsigned char data) {
    cpu_state* state = get_current_cpu_state();
    state->sp--;
    write_memory(state->sp + 0x0100, data);

}

unsigned char stack_pop() {
    cpu_state* state = get_current_cpu_state();
    state->sp++;
    return read_memory(state->sp-1 + 0x0100);

}