#ifndef _ROM_H
#define _ROM_H

#include <stdio.h>

#define PRG_ROM_SIZE	16384
#define CHR_ROM_SIZE	8192

typedef unsigned char uchar;

typedef struct {
	uchar nes_id[3];
	uchar magic;
	
	uchar prg_pcount;
	uchar chr_pcount;
	
	uchar rom_ctrl1;
	uchar rom_ctrl2;
	
	uchar unused1;
	uchar unused2;
	
	uchar info;
	
	uchar unused3[5];
	
}ROM_HEADER;

typedef struct {
	ROM_HEADER* header;
	unsigned char* prg_rom;
	unsigned char* chr_rom;
}NES_ROM;

void load_rom(FILE* fd);
NES_ROM* get_current_rom();
ROM_HEADER* load_header(FILE* fd);

unsigned char prg1_read(unsigned short int addr);
unsigned char prg2_read(unsigned short int addr);


/* Debug functions */
void print_header(ROM_HEADER* header);

#endif /* _ROM_H */
