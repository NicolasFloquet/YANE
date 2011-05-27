#ifndef _CPU_H
#define _CPU_H

typedef struct {
    char n:1;
    char v:1;
    char unused:1;
    char b:1;
    char d:1;
    char i:1;
    char z:1;
    char c:1;
}s_flags;

typedef union {
    char byte;
    s_flags flags;
}u_flags;

typedef struct {
    /* CPU info */
    unsigned short int pc;
    unsigned char sp;
    unsigned char A;
    unsigned char X;
    unsigned char Y;
    u_flags P;

    /* Debug info */
    int cycle;

}cpu_state;

cpu_state* get_current_cpu_state();
void create_cpu_state();
void step();
#endif /* _CPU_H */