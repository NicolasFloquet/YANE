#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory.h>
#include <rom.h>
#include <cpu.h>
#include <ppu.h>

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
    {0x0200, 0x07ff, "RAM", ram_read, ram_write},
    {0x0800, 0x1fff, "RAM mirror", ram_read, ram_write},
    {0x2000, 0x2007, "PPU regs", ppu_reader, ppu_writer},
    {0x2008, 0x3fff, "PPU regs mirror", ppu_reader, ppu_writer},
    {0x4000, 0x4013, "IO regs2", NULL, NULL},
    {0x4014, 0x4014, "DMA reg", NULL, ppu_writer},
    {0x4015, 0x4017, "IO regs2", NULL, NULL},
    {0x4020, 0x5fff, "exp RAM", NULL, NULL},
    {0x6000, 0x7fff, "SRAM", NULL, NULL},
    {0x8000, 0xbfff, "PRG-ROM low bank", prg1_read, NULL},	/* ROM => read-only */
    {0xc000, 0xffff, "PGR-ROM high bank", prg2_read, NULL}	/* ROM => read-only */
};


void init_ram() {
    RAM = malloc(0x800);
}

void dump_zero() {
    FILE* fd = fopen("zero.dump", "w");
    fwrite(RAM, 0xff, 1, fd);
    fclose(fd);
}

void dump_stack() {
    FILE* fd = fopen("stack.dump", "w");
    fwrite(&(RAM[0x100]), 0xff, 1, fd);
    fclose(fd);
}

void dump_ram() {
    FILE* fd = fopen("ram.dump", "w");
    fwrite(&(RAM[0x200]), 0x5ff, 1, fd);
    fclose(fd);
}

void dump_all() {
    FILE* fd = fopen("all.dump", "w");
    fwrite((RAM), 0x7ff, 1, fd);
    fclose(fd);
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
		printf("\nReader for %s is not implemented yet. (0x%x)", memory_map[i].name, addr);
    else
		ret = memory_map[i].reader(addr);

    return ret;
}

void write_memory(unsigned short int addr, unsigned char data) {
    int i = find_zone(addr);

    //if(memory_map[i].writer == NULL)
	//printf("\nWriter for %s is not implemented yet. (0x%x)", memory_map[i].name, addr);
    //else
    if(memory_map[i].writer != NULL)
	memory_map[i].writer(addr,data);
}

void dma_transfer(unsigned short int* dst, unsigned short int src, unsigned short int size) {
	memcpy((void*)dst, (void*) &(RAM[src]), size);
}

/* Un peu trop hardcodé à mon gout */
void stack_push(unsigned char data) {
    cpu_state* state = get_current_cpu_state();
    write_memory(state->sp + 0x0100, data);
    state->sp--;

}

unsigned char stack_pop() {
    cpu_state* state = get_current_cpu_state();
    state->sp++;
    return read_memory(state->sp + 0x0100);

}
