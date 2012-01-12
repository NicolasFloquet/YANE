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

typedef struct __attribute__ ((__packed__)){
	unsigned char y;
	unsigned char index;
	unsigned char attribute;
	unsigned char x;
}sprite_info;

typedef struct {
	/* Registres */
	char ppu_ctrl1;		/* 0x2000 */
	char ppu_ctrl2;		/* 0x2001 */
	char ppu_status;	/* Ox2002 */
	char spr_addr;		/* 0x2003 */
	/* spr_io */		/* 0x2004 */
	char vram_scroll;	/* 0x2005 */
	unsigned short int vram_addr;	/* 0x2006 */
	/* vram_io */		/* 0x2007 */
	
	/* VRAM, 0x0000-0x3fff */
	char* vram;
	
	/* Sprite RAM */
	sprite_info* spr_ram;
	
	/* Scanline counter */
	int scanline;
	
	/* Misc */
	char vram_addr_counter;	/* Sert à déterminer si on écrit pour la première fois dans vram_addr, étant donné qu'il faut faire deux écritures de 8bit pour écrire une adresse de 16bit */
}ppu_state;

void ppu_init(params* p);
void ppu_update();
void ppu_scanline();

unsigned char ppu_reader(unsigned short int addr);
void ppu_writer(unsigned short int addr, unsigned char data);

void dump_chrrom();

#endif /* _PPU_H */
