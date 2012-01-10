#include "../include/rom.h"

#include <stdlib.h>

static NES_ROM* current_rom = NULL;

void load_rom(FILE* fd) {
	NES_ROM* rom = malloc(sizeof(NES_ROM));
	
	/* Load ROM header */
	rom->header = load_header(fd);
	
	if(((rom->header->rom_ctrl1&0xF0)>>4 | (rom->header->rom_ctrl2&0xF0)) != 0) {
		printf("Mapper %x not supported yet.\n", (rom->header->rom_ctrl1&0xF0)>>4 | (rom->header->rom_ctrl2&0xF0));
		exit(-1);
	}
	
	/* Load PRG ROM */
	rom->prg_rom = malloc(rom->header->prg_pcount * PRG_ROM_SIZE);
	fread(rom->prg_rom, 1, rom->header->prg_pcount * PRG_ROM_SIZE, fd);
	
	/* Load CHR ROM */
	rom->chr_rom = malloc(rom->header->chr_pcount * CHR_ROM_SIZE);
	fread(rom->chr_rom, 1, rom->header->chr_pcount * CHR_ROM_SIZE, fd);

	current_rom = rom;
}

NES_ROM* get_current_rom() {
	return current_rom;
}

ROM_HEADER* load_header(FILE* fd) {
	ROM_HEADER* header = NULL;
	
	header = malloc(sizeof(ROM_HEADER));
	
	fseek(fd, 0, SEEK_SET); /* Retour au début du fichier au cas où */
	
	fread(header, sizeof(ROM_HEADER), 1, fd);
	
	return header;
}

unsigned char prg1_read(unsigned short int addr) {
		return current_rom->prg_rom[addr-0x8000];
}

unsigned char prg2_read(unsigned short int addr) {
	if(current_rom->header->prg_pcount == 1)
		return current_rom->prg_rom[addr-0xC000];
	else
		return current_rom->prg_rom[addr-0x8000];
}

void print_header(ROM_HEADER* header) {
	printf("-----ROM HEADER INFO-----\n");
	printf("ID:\t\t%c%c%c\n", header->nes_id[0], header->nes_id[1], header->nes_id[2]);
	printf("Magic Number:\t0x%x\n\n", header->magic);
	printf("16k prg-rom page count:\t0x%x\n", header->prg_pcount);
	printf("8k chr-rom page count:\t0x%x\n\n", header->prg_pcount);
	printf("ROM Control byte #1:\tv-T-s-M\n");
	printf("\t\t\t%d-%d-%d-%d\n",(header->rom_ctrl1&0x8)>>3,
							(header->rom_ctrl1&0x4)>>2,
							(header->rom_ctrl1&0x2)>>1,
							(header->rom_ctrl1&0x1));
	printf("ROM Control byte #2:\t0x%x\n", header->rom_ctrl2 & 0xF);
	printf("Mapper:\t0x%x%x\n\n",(header->rom_ctrl1&0xF0)>>4, (header->rom_ctrl1&0xF0)>>4);
	
	printf("unused1: 0x%x\n", header->unused1);
	printf("unused2: 0x%x\n", header->unused2);
	
	switch(header->info) {
		case 0:
			printf("This ROM is 100%% compatible with NTSC console\n");
			break;
		case 1:
			printf("This ROM is not necessarily 100%% compatible with PAL console\n");
			break;
		case 4:
			printf("This ROM has extra RAM at 0x6000-0x7fff\n");
			break;
		case 5:
			printf("This ROM don't have bus conflicts");
			break;
		default:
			printf("Wait... wut? (=>0x%x\n", header->info);
	}
}
