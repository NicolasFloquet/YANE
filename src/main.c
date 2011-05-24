#include <stdio.h>
#include <stdlib.h>

#include "../include/rom.h"

int main(int argc, char** argv) {
	FILE* fd = NULL; 
	ROM_HEADER* header = NULL;
	
	fd = fopen("./rom/test.nes", "r");
	
	if(fd != NULL) {
		header = load_header(fd);
		print_header(header);
		disassemble(fd);
	}
	else {
		printf("impossible d'ouvrir la rom.\n");
	}
	
    return 0;
}


