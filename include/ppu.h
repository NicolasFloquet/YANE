#ifndef _PPU_H
#define _PPU_H

#include <common.h>

#define PPU_CTRL1	0x2000
#define PPU_CTRL2	0x2001
#define PPU_STATUS	0x2002
#define SPR_RAM_ADDR	0x2003
#define SPR_RAM_IO	0x2004
#define VRAM_SCROLL	0x2005
#define VRAM_ADDR	0x2006
#define VRAM_IO		0x2007
#define SPR_DMA		0x4014

#define PPU_NAMETABLE_BASE 0x2000
#define PPU_ATTRIBUTE_OFFSET 0x03C0

typedef struct {
	unsigned char y;
	unsigned char index;
	unsigned char attribute;
	unsigned char x;
}sprite_info;

void ppu_init(params* p);
void ppu_update();

unsigned char ppu_reader(unsigned short int addr);
void ppu_writer(unsigned short int addr, unsigned char data);

void dump_chrrom();

#endif /* _PPU_H */
