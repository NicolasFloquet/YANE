#include <stdio.h>
#include <stdlib.h>

#include <ppu.h>
#include <memory.h>
#include <rom.h>

#include <SDL/SDL.h>

static char* vram = NULL;

static sprite_info* spr_ram = NULL;

static char ppu_ctrl1;
static char ppu_ctrl2;

static unsigned short int spr_addr;

static unsigned short int vram_addr;
static char vram_addr_counter;

static char ppu_status;

/* tmp */
static SDL_Surface * screen;

void _put_pixel(SDL_Surface * surface, Uint16 x, Uint16 y, Uint32 color)
{
    /* Nombre de bits par pixels de la surface d'écran */
    Uint8 bpp = surface->format->BytesPerPixel;
    /* Pointeur vers le pixel à remplacer (pitch correspond à la taille 
       d'une ligne d'écran, c'est à dire (longueur * bitsParPixel) 
       pour la plupart des cas) */
    Uint8 * p = ((Uint8 *)surface->pixels) + y * surface->pitch + x * bpp;
     switch(bpp)
    {
	  case 1:
		*p = (Uint8) color;
		break;
        case 2:
            *(Uint16 *)p = (Uint16) color;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                *(Uint16 *)p = ((color >> 8) & 0xff00) | ((color >> 8) & 0xff);
                *(p + 2) = color & 0xff;
            }
            else
            {
                *(Uint16 *)p = color & 0xffff;
                *(p + 2) = ((color >> 16) & 0xff) ;
            }
            break;
        case 4:
            *(Uint32 *)p = color;
            break;
    }
}

void put_pixel(unsigned short int x,unsigned short int y,unsigned int color, unsigned char scale) {
	int i,j;
	for(i=0; i<scale; i++) {
		for(j=0; j<scale; j++) {
			_put_pixel(screen, x*scale+i, y*scale+j, color);
		}
	}
}

void put_sprite(unsigned short int x, unsigned short int y, unsigned char index, unsigned char bank, unsigned char scale) {
	unsigned char* ptr1 = get_current_rom()->chr_rom +0x1000+(index*16);
	unsigned char* ptr2 = ptr1+8;
	char c1,c2;
	int i,j;
	
	for(i=0; i<8; i++) {
		c1 = *ptr1;
		c2 = *ptr2;
		for(j=0; j<8; j++) {
				put_pixel(	x+j,
							y+i,
							SDL_MapRGB(screen->format, 0xff*((c1&0x80)>>7), 0xff*((c2&0x80)>>7), 0xff*((c1&0x80)>>7)),
							scale);
				c1=c1<<1;
				c2=c2<<1;
		}
		ptr1++;
		ptr2++;
	}
}

void dump_chrrom() {
	int x,y;
	int i = 0;
	for(x=0;x<16;x++) {
		for(y=0; y<16; y++) {
			put_sprite(y*8, x*8, i, 0, 2);
			i++;
		}
	}
	SDL_Flip(screen);
}

void ppu_init(params* p) {
	vram = malloc(0x3fff);
	spr_ram = malloc(sizeof(sprite_info) * 64);
	ppu_ctrl1 = 0;
	ppu_ctrl2 = 0;
	spr_addr = 0;
	vram_addr = 0;
	vram_addr_counter = 0;
	ppu_status = 0x80;
	
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
    }
    screen = SDL_SetVideoMode(512, 480, 16, 
                              SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT);                          
}

void ppu_update() { 

	char* base_nametable = vram + 0x2000 + (ppu_ctrl1&0x3)*0x0400;
	unsigned short int base_attribute = base_nametable + PPU_ATTRIBUTE_OFFSET;
	int sprite;
	int x,y;
	int i;

	/* Affichage du background */
	if(ppu_ctrl2 & 0x08) {
		for(x=0; x<32; x++) {
			for(y=0;y<30; y++) {
				put_sprite(x*8, y*8, base_nametable[x+(y*32)], ppu_ctrl1 & 0x10>>4, 2);
			}
		}
	}
	
	/* Affichage des sprites */
	i = 0;
	if(ppu_ctrl2 & 0x10) {
		for(i=0; i<64; i++) {
			
			put_sprite(spr_ram[i].x, spr_ram[i].y, spr_ram[i].index, ppu_ctrl1 & 0x80>>3, 2);
		}
	}
	
	//if(ppu_ctrl2 != 0) 
		SDL_Flip(screen);
	if(((ppu_ctrl1 & 0x80) != 0))
		cpu_nmi();
		
	ppu_status |= 0x80; /* VBLank flag */
}

unsigned char ppu_reader(unsigned short int addr) {
	char ret = 0;

	switch(addr){
		case PPU_STATUS:
			printf("PPU Read status\n");
			ret = ppu_status;
			ppu_status &= 0x7F; /* Remise à zero du bit de VBlank */
			break;
		case VRAM_IO:
			ret = vram[vram_addr];
			break;
		default:
			printf("\n0x%x is write only!", addr);
	}

	return ret;
}
void ppu_writer(unsigned short int addr, unsigned char data) {
	switch(addr) {
		case PPU_CTRL1:
			printf("\tPPU_CTRL1 = %x\n",data);
			ppu_ctrl1 = data;
			break;
		case PPU_CTRL2:
			printf("\tPPU_CTRL2 = %x\n",data);
			ppu_ctrl2 = data;
			break;
		case SPR_RAM_ADDR:
			printf("\tSPR_RAM_ADDR = %x\n",data);
			spr_addr = data;
			//exit(0);
			break;
		case SPR_RAM_IO:
			printf("\twriting SPR_RAM[0x%x] = 0x%x\n", spr_addr, data);
			((char*)spr_ram)[spr_addr] = data;
			break;
		case VRAM_SCROLL:
			break;
		case VRAM_ADDR:
			if(vram_addr_counter == 0) {
				vram_addr = data<<8;
				vram_addr_counter = 1;
			}			
			else {
				vram_addr |= data;
				vram_addr_counter = 0;
			}
			break;
		case VRAM_IO:
			//printf("\twriting 0x%x at 0x%x\n",data, vram_addr);
			vram[vram_addr] = data;
			if(ppu_ctrl1 & 0x04)
				vram_addr += 32;
			else
				vram_addr += 1;
			break;
		case SPR_DMA:
			printf("\tDMA from 0x%x\n", 0x100*data);
			//exit(-1);
			dma_transfer((unsigned short int*) spr_ram, data*(0x100), 256);
			break;
		default:
			printf("\n0x%x is read only!\n", addr);
	}
}
