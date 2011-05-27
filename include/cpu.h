#ifndef _CPU_H
#define _CPU_H

/* Macro pour le registre flags */
#define SET_CARRY(p) (p=p|0x01)
#define CLEAR_CARRY(p) (p=p&0xFE)
#define GET_CARRY(p) (p&0x1)

#define SET_ZERO(p) (p=p|0x02)
#define CLEAR_ZERO(p) (p=p&0xFD)
#define GET_ZERO(p) ((p&0x2) >> 1)

#define SET_INTERRUPT(p) (p=p|0x04)
#define CLEAR_INTERRUPT(p) (p=p&0xFB)
#define GET_INTERRUPT(p) ((p&0x4) >> 2)

#define SET_BCD(p) (p=p|0x8)
#define CLEAR_BCD(p) (p=p&0xF7)
#define GET_BCD(p) ((p&0x8) >> 3)

#define SET_BRK(p) (p=p|0x10)
#define CLEAR_BRK(p) (p=p&0xEF)
#define GET_BRK(p) ((p&0x10) >> 4)

#define SET_OVERF(p) (p=p|0x40)
#define CLEAR_OVERF(p) (p=p&0xBF)
#define GET_OVERF(p) ((p&0x40) >> 6)

#define SET_SIGN(p) (p=p|0x80)
#define CLEAR_SIGN(p) (p=p&0x7F)
#define GET_SIGN(p) ((p&0x80) >> 7)

typedef struct {
    /* CPU info */
    unsigned short int pc;
    unsigned char sp;
    unsigned char A;
    unsigned char X;
    unsigned char Y;
    unsigned char P;

    /* Debug info */
    int cycle;

}cpu_state;

cpu_state* get_current_cpu_state();
void create_cpu_state();
void step();
void runto(unsigned short int addr);
#endif /* _CPU_H */
