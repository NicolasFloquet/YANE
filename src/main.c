#include <stdio.h>
#include <stdlib.h>

#include <cpu.h>
#include <memory.h>
#include <opcodes.h>
#include <rom.h>

int main(int argc, char** argv) {
	FILE* fd = NULL; 
	char* filename = argv[1];
	char* bp = argv[2];
	int i=0;
	int breakpoint;
	if(argc<=1) {
	    printf("No rom specified, opening rom/test.nes ...\n");
	    fd = fopen("./rom/test.nes", "r");
	}
	else {
	    fd = fopen(filename, "r");
	}

	if(fd != NULL) {
		load_rom(fd);
		create_cpu_state();
		init_ram();
		//print_header(get_current_rom()->header);
		//disassemble();
		if(argc == 3) {
			sscanf(bp,"%x",&breakpoint);
			printf("break at 0x%x\n", breakpoint);
			runto((unsigned short int)breakpoint);
		}
		while(i<10){
		    step();
		    while (getchar() != '\n');
		}
	}
	else {
		printf("impossible d'ouvrir la rom.\n");
	}
	
    return 0;
}


