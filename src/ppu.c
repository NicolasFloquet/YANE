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
	char c1,c2;
	unsigned char index = *(state->vram + nametable + n + (scanline/8)*32);
	unsigned char* ptr1 = GET_TILE_SCANLINE(index,scanline%8,patterntable);
	unsigned char* ptr2 = ptr1+8;

	c1 = *ptr1;
	c2 = *ptr2;
	for(i=n*8; i<n*8+8; i++) {

			fb_put_pixel(i,
						 scanline,
						 (0xff*((c1&0x80)>>7))<<16 | (0xff*((c2&0x80)>>7))<<8 | (0xff*((c1&0x80)>>7)),
						 2);	
			c1=c1<<1;
			c2=c2<<1;			 
				
	}
	
}

void render_sprite_scanline(int scanline) {
	
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
	state->ppu_status = 0x80;
	state->scanline = 261;
	
	fb_init();                    
}

void ppu_scanline() {
	
	unsigned short int nametable 	= ((state->ppu_ctrl1 & 0x3)*0x0400) + 0x2000;
	unsigned short int patterntable = ((state->ppu_ctrl1 & 0x10)>>4) * 0x1000;
	int i;
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
		if(state->ppu_ctrl2 & 0x10) {
			
		}
	}
	else if(state->scanline == 240) {
		/* Idle scanline */
	}
	else if(state->scanline == 241) {
		/* VBlank start */
		fb_update();
		if(((state->ppu_ctrl1 & 0x80) != 0))
			cpu_nmi();
		state->ppu_status |= 0x80;
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
	
	//if(ppu_ctrl2 != 0) 
		fb_update();
	if(((state->ppu_ctrl1 & 0x80) != 0))
		cpu_nmi();
		
	state->ppu_status |= 0x80; /* VBLank flag */
}

unsigned char ppu_reader(unsigned short int addr) {
	char ret = 0;

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
