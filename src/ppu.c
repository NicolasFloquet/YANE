#include <stdio.h>
#include <stdlib.h>

#include <ppu.h>
#include <cpu.h>
#include <memory.h>
#include <rom.h>
#include <framebuffer.h>

#define GET_TILE_INDEX(X,Y)  (*(state->vram + 0x2000 + (state->ppu_ctrl1&0x3)*0x0400 + X + Y*32))
#define GET_TILE_SCANLINE(I,S,P) (get_current_rom()->chr_rom + (P) +(I*16) + S)

static ppu_state* state = NULL;

unsigned char internal_palette[][3] = {
	/* Luminance 0 */
	{0x80,0x80,0x80}, {0x00,0x3D,0xA6}, {0x00,0x12,0xB0}, {0x44,0x00,0x96}, 
	{0xA1,0x00,0x5E}, {0xC7,0x00,0x28}, {0xBA,0x06,0x00}, {0x8C,0x17,0x00},
	{0x5C,0x2F,0x00}, {0x10,0x45,0x00}, {0x05,0x4A,0x00}, {0x00,0x47,0x2E},
	{0x00,0x41,0x66}, {0x00,0x00,0x00}, {0x05,0x05,0x05}, {0x05,0x05,0x05},
	/* Luminance 1 */
	{0xC7,0xC7,0xC7}, {0x00,0x77,0xFF}, {0x21,0x55,0xFF}, {0x82,0x37,0xFA},
	{0xEB,0x2F,0xB5}, {0xFF,0x29,0x50}, {0xFF,0x22,0x00}, {0xD6,0x32,0x00},
	{0xC4,0x62,0x00}, {0x35,0x80,0x00}, {0x05,0x8F,0x00}, {0x00,0x8A,0x55},
	{0x00,0x99,0xCC}, {0x21,0x21,0x21}, {0x09,0x09,0x09}, {0x09,0x09,0x09},
	/* Luminance 2 */
	{0xFF,0xFF,0xFF}, {0x0F,0xD7,0xFF}, {0x69,0xA2,0xFF}, {0xD4,0x80,0xFF},
	{0xFF,0x45,0xF3}, {0xFF,0x61,0x8B}, {0xFF,0x88,0x33}, {0xFF,0x9C,0x12},
	{0xFA,0xBC,0x20}, {0x9F,0xE3,0x0E}, {0x2B,0xF0,0x35}, {0x0C,0xF0,0xA4},
	{0x05,0xFB,0xFF}, {0x5E,0x5E,0x5E}, {0x0D,0x0D,0x0D}, {0x0D,0x0D,0x0D},
	/* Luminance 3 */
	{0xFF,0xFF,0xFF}, {0xA6,0xFC,0xFF}, {0xB3,0xEC,0xFF}, {0xDA,0xAB,0xEB},
	{0xFF,0xA8,0xF9}, {0xFF,0xAB,0xB3}, {0xFF,0xD2,0xB0}, {0xFF,0xEF,0xA6},
	{0xFF,0xF7,0x9C}, {0xD7,0xE8,0x95}, {0xA6,0xED,0xAF}, {0xA2,0xF2,0xDA},
	{0x99,0xFF,0xFC}, {0xDD,0xDD,0xDD}, {0x11,0x11,0x11}, {0x11,0x11,0x11},
};

void put_sprite(unsigned short int x, 
			    unsigned short int y, 
			    unsigned char index, 
			    unsigned char bank, 
			    unsigned char scale) {
	unsigned char* ptr1;
	unsigned char* ptr2;
	char c1,c2;
	int i,j;
	
	for(i=0; i<8; i++) {
		
		ptr1 = GET_TILE_SCANLINE(index,i,bank);
		ptr2 = ptr1+8;
		c1 = *ptr1;
		c2 = *ptr2;
		for(j=0; j<8; j++) {
				fb_put_pixel(	x+j,
							y+i,
							(0xff*((c1&0x80)>>7))<<16 | (0xff*((c2&0x80)>>7))<<8 | (0xff*((c1&0x80)>>7)),
							scale);
				c1=c1<<1;
				c2=c2<<1;
		}
	}
}

void render_tile_scanline(int n, 
						  int scanline, 
						  unsigned short int patterntable,
				 		  unsigned short int nametable) {
	int i = n*8;
	int Y = scanline >> 3;
	int X = n;
	
	int att_x = X>>2;
	int att_y = Y>>2; 
	
	unsigned char index = *(state->vram + nametable + n + (scanline/8)*32);
	unsigned char* ptr1 = GET_TILE_SCANLINE(index,scanline%8,patterntable);
	unsigned char* ptr2 = ptr1+8;
	
	unsigned char data[8];
	
	unsigned char* palette = state->vram + 0x3F00;
	unsigned char attribute;
	
	char color_lower1 = *ptr1;
	char color_lower2 = *ptr2;
	char color_upper;
	
	/* Get the upper part of the color index in the attribute table */
	attribute = *(state->vram + nametable + PPU_ATTRIBUTE_OFFSET + att_x + att_y*8);
	int x = X & 0x2;
	int y = Y & 0x2;
	int shift = x | (y<<1);
	color_upper = ((0x3 << shift) & attribute)>>shift;
	
	/* Unpack tile scanline data */
	for(i=0; i<8; i++) {
			data[i] = ((color_lower1 & 0x80)>>7) | ((color_lower2 & 0x80)>>6) | color_upper << 2;
			color_lower1 = color_lower1<<1;
			color_lower2 = color_lower2<<1;
	}
	
	/* Render the scanline */
	for(i=0; i<8; i++) {
			fb_put_pixel(i+n*8,
						 scanline,
						 (internal_palette[palette[data[i]]][0]<<16) | (internal_palette[palette[data[i]]][1]<<8) | (internal_palette[palette[data[i]]][2]),
						 2);		
	}
	
}

static int sprite_evaluation(int sprites[8], int scanline) {
	int i;
	
	int n = 0; /* Compteur de sprites */
	
	int y;
	int y_offset;
	
	for(i=0; i<64; i++) {
		y = state->spr_ram[i].y;
		y_offset = scanline - y;
		
		/* On vérifie si le sprite est traversé par la scanline */
		/* TODO: prendre en compte les sprites 8x16 */
		if((y_offset >= 0) && (y_offset < 8)) {
			
			/* Si le sprite est touché, on l'ajoute à la liste */
			sprites[n] = i;
			n++;
		}
		if(n==8)
			break;
	}
	
	return n;	
}

void render_sprite_scanline(int scanline,						  
							unsigned short int patterntable) {
	int sprites[8];
	int sprite_count;
	int i;
	int x;
	char c1;
	char c2;
	unsigned char index;
	unsigned char* ptr1;
	unsigned char* ptr2;
	
	sprite_count = sprite_evaluation(sprites, scanline);
	
	/* Si on a trouvé plus de 8 sprites, on met le sprite count flag*/
	if(sprite_count == 8) {
		state->ppu_status |= 0x20;
	}
	else {
		state->ppu_status &= 0xdf;
	}
	
	/* Si on affiche le sprite0, on met le hit flag */
	if(sprites[0] == 0) {
		state->ppu_status |= 0x40;
	}
	/* Et on les affiche */
	for(i=0; i<sprite_count; i++) {
		index = state->spr_ram[sprites[i]].index;
		ptr1 = GET_TILE_SCANLINE(index,scanline%8,0);
		ptr2 = ptr1+8;
		c1 = *ptr1;
		c2 = *ptr2;
		for(x=state->spr_ram[sprites[i]].x; x<state->spr_ram[sprites[i]].x+8; x++) {
			fb_put_pixel(x,
						 scanline,
						 /*(0xff*((c1&0x80)>>7))<<16 | (0xff*((c2&0x80)>>7))<<8 | (0xff*((c1&0x80)>>7))*/0xFF00FF,
						 2);	
			c1=c1<<1;
			c2=c2<<1;		
		}
	}

}

void ppu_init(params* p) {
	state = malloc(sizeof(ppu_state));
	
	state->vram = malloc(0x3fff);
	state->spr_ram = malloc(sizeof(sprite_info) * 64);
	state->ppu_ctrl1 = 0;
	state->ppu_ctrl2 = 0;
	state->spr_addr = 0;
	state->vram_addr = 0;
	state->vram_addr_counter = 0;
	state->ppu_status = 0x00;
	state->scanline = 261;
	
	fb_init();                    
}

void ppu_scanline() {
	
	unsigned short int nametable 	= ((state->ppu_ctrl1 & 0x3)*0x0400) + 0x2000;
	unsigned short int patterntable = ((state->ppu_ctrl1 & 0x10)>>4) * 0x1000;
	int i;
	//printf("%d \n",state->scanline);
	//printf("SCANLINE %d\n", state->scanline);
	if(state->scanline == 261) { 
		/* Pre-render scanline */
		state->scanline = 0;
		state->ppu_status &= 0x7F;
	}
	else if(state->scanline >= 0 && state->scanline <= 239) {
		/* Render scanlines */
		
		/* Render background */
		if(state->ppu_ctrl2 & 0x08) {
			for(i=0; i<31; i++) {
				render_tile_scanline(i, state->scanline, patterntable, nametable);
				//fb_update();
			}
		}
		
		/* Render sprites */
	/*	if(state->ppu_ctrl2 & 0x10) {
			render_sprite_scanline(state->scanline, patterntable);
		}*/
	}
	else if(state->scanline == 240) {
		/* Idle scanline */
	}
	else if(state->scanline == 241) {
		/* VBlank start */
		fb_update();
		
		if(((state->ppu_ctrl1 & 0x80) != 0))
			cpu_nmi();
		
		/* set VBlank flag */
		state->ppu_status |= 0x80;
		
		/* clear Hit Flag */
		state->ppu_status &= 0xBF;
	}
	else if(state->scanline > 241 && state->scanline <= 260) {
		/* VBlank */
	}
	state->scanline++;
}

void ppu_update() { 
	int x,y;
	int i;
	unsigned short int patterntable = ((state->ppu_ctrl1 & 0x10)>>4) * 0x1000;
	printf("----0x%x\n",patterntable);
	/* Affichage du background */
	if(state->ppu_ctrl2 & 0x08) {
		for(x=0; x<32; x++) {
			for(y=0;y<30; y++) {
				put_sprite(x*8,	/* Coord X */
						   y*8,	/* Coord Y */
						   GET_TILE_INDEX(x,y),	/* Sprite Index */
						   patterntable,	/* Pattern table selector */
						   2); /* Scale */
			}
		}
	}
	
	/* Affichage des sprites */
	i = 0;
	if(state->ppu_ctrl2 & 0x10) {
		for(i=0; i<64; i++) {
			
			put_sprite(state->spr_ram[i].x, /* Coord X */
					   state->spr_ram[i].y, /* Coord Y */
					   state->spr_ram[i].index, /* Sprite Index */
					   state->ppu_ctrl1 & 0x80>>3, /* Pattern table selector */
					   2); /* Scale */
		}
	}
	
	if(state->ppu_ctrl2 != 0) 
		fb_update();
	if(((state->ppu_ctrl1 & 0x80) != 0))
		cpu_nmi();
		
	state->ppu_status |= 0x80; /* VBLank flag */
}

unsigned char ppu_reader(unsigned short int addr) {
	char ret = 0;
	addr = addr%8 + 0x2000;
	switch(addr){
		case PPU_STATUS:
			//printf("PPU Read status\n");
			ret = state->ppu_status;
			state->ppu_status &= 0x7F; /* Remise à zero du bit de VBlank */
			break;
		case VRAM_IO:
			ret = state->vram[state->vram_addr];
			break;
		default:
			printf("\n0x%x is write only!", addr);
	}

	return ret;
}
void ppu_writer(unsigned short int addr, unsigned char data) {
	addr = addr%8 + 0x2000;
	switch(addr) {
		case PPU_CTRL1:
			//printf("\tPPU_CTRL1 = %x\n",data);
			state->ppu_ctrl1 = data;
			break;
		case PPU_CTRL2:
			//printf("\tPPU_CTRL2 = %x\n",data);
			state->ppu_ctrl2 = data;
			break;
		case SPR_RAM_ADDR:
			//printf("\tSPR_RAM_ADDR = %x\n",data);
			state->spr_addr = data;
			//exit(0);
			break;
		case SPR_RAM_IO:
			//printf("\twriting SPR_RAM[0x%x] = 0x%x\n", state->spr_addr, data);
			((char*)state->spr_ram)[(unsigned int)state->spr_addr] = data;
			break;
		case VRAM_SCROLL:
			break;
		case VRAM_ADDR:
			if(state->vram_addr_counter == 0) {
				state->vram_addr = data<<8;
				state->vram_addr_counter = 1;
			}			
			else {
				state->vram_addr |= data;
				state->vram_addr_counter = 0;
			}
			break;
		case VRAM_IO:
			//printf("\twriting 0x%x at 0x%x\n",data, vram_addr);
			if(state->vram_addr > 0x3F1F)
				exit(0);
			state->vram[state->vram_addr] = data;
			if(state->ppu_ctrl1 & 0x04)
				state->vram_addr += 32;
			else
				state->vram_addr += 1;
			break;
		case SPR_DMA:
			printf("\tDMA from 0x%x\n", 0x100*data);
			//exit(-1);
			dma_transfer((unsigned short int*) state->spr_ram, data*(0x100), 256);
			break;
		default:
			printf("\n0x%x is read only!\n", addr);
	}
}
