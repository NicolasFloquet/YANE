#ifndef _MEMORY_H
#define _MEMORY_H

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

void stack_push(unsigned char data);
unsigned char stack_pop();

#endif /* _MEMORY_H */