#ifndef _ROM_H
#define _ROM_H

#include <stdio.h>

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
	
	uchar unused3[6];
	
}ROM_HEADER;

ROM_HEADER* load_header(FILE* fd);
void print_header(ROM_HEADER* header);

#endif /* _ROM_H */
