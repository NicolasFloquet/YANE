#include <stdio.h>
#include <stdlib.h>

#include "../include/rom.h"

int main(int argc, char** argv) {
	FILE* fd = NULL; 
	NES_ROM* rom = NULL;
	
	fd = fopen("./rom/test.nes", "r");
	
	if(fd != NULL) {
		rom = load_rom(fd);
		print_header(rom->header);
		disassemble(rom);
	}
	else {
		printf("impossible d'ouvrir la rom.\n");
	}
	
    return 0;
}


