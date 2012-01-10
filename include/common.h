#ifndef _COMMON_H
#define _COMMON_H

typedef struct {
    char* rom_path;
    unsigned short int breakpoint;
    int run;
    int dump_ram;
    int dump_stack;
    int dump_zero;
    int dump_vram;
    int dump_all;
}params;

#endif
