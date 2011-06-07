#ifndef _MEMORY_H
#define _MEMORY_H

#define read_memory16(addr) (((unsigned short int)read_memory(addr+1)<<8) | ((unsigned short int)read_memory(addr)&0xFF))
#define read_memory16z(addr) (((unsigned short int)read_memory((addr+1)%0x100)<<8) | ((unsigned short int)read_memory(addr%0x100)&0xFF))

typedef unsigned char(*memory_reader)(unsigned short int);
typedef void(*memory_writer)(unsigned short int, unsigned char);

typedef struct {
    unsigned short int start;
    unsigned short int end;
    char* name;
    memory_reader reader;
    memory_writer writer;
}memory_zone;

void init_ram();

void dump_zero();
void dump_stack();
void dump_ram();
void dump_all();

char* zone_name(unsigned short int addr);
char read_memory(unsigned short int addr);
void write_memory(unsigned short int addr, unsigned char data);
void dma_transfer(unsigned short int* dst, unsigned short int src, unsigned short int size);

void stack_push(unsigned char data);
unsigned char stack_pop();

#endif /* _MEMORY_H */
