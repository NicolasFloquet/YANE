#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cpu.h>
#include <memory.h>
#include <opcodes.h>
#include <rom.h>

typedef struct {
    char* rom_path;
    unsigned short int breakpoint;
    int run;
    int dump_ram;
    int dump_stack;
    int dump_zero;
    int dump_all;
}params;

void usage() {
    printf("USAGE:\tyane rom [-b bp_addr] [-d ram|stack|zero|all] [-h]\n");
    printf("\trom: name of the rom file.\n");
    printf("\t-b: run the rom until bp_addr.\n");
    printf("\t-d: dump memory zone to a file.\n");
    printf("\t-h: print this help.\n");
}

params* parse_args(int argc, char** argv) {
    int i = 1;
    params* new_params = malloc(sizeof(params));

    new_params->rom_path = NULL;
    new_params->breakpoint = 0;
    new_params->run = 0;
    new_params->dump_ram = 0;
    new_params->dump_stack = 0;
    new_params->dump_zero = 0;
    new_params->dump_all = 0;

    while(i<argc) {
	if(strcmp("-h", argv[i]) == 0) {
	    usage();
	    exit(0);
	}
	if(strcmp("-r", argv[i]) == 0) {
	    new_params->run = 1;
	}
	else if(strcmp("-b", argv[i]) == 0) {
	    i++;
	    sscanf(argv[i],"%x",(unsigned int*) &(new_params->breakpoint));
	}
	else if(strcmp("-d", argv[i]) == 0) {
	    i++;
	    if(strcmp("ram", argv[i]) == 0) {
		new_params->dump_ram = 1;
	    }
	    else if(strcmp("stack", argv[i]) == 0) {
		new_params->dump_stack = 1;
	    }
	    else if(strcmp("zero", argv[i]) == 0) {
		new_params->dump_zero = 1;
	    }
	    else if(strcmp("all", argv[i]) == 0) {
		new_params->dump_all = 1;
	    }
	    else {
		printf("%s is not a valid memory zone, cannot dump\n", argv[i]);
	    }
	}
	else {
	    if(argv[i][0] == '-') {
		printf("%s is not a valid parameter.\n", argv[i]);
	    }
	    else {
		new_params->rom_path = strdup(argv[i]);
	    }
	}
	i++;
    }

    if(new_params->rom_path == NULL)
	new_params->rom_path = strdup("./rom/test.nes");

    return new_params;
}

int main(int argc, char** argv) {
	FILE* fd = NULL; 
	params* p;

	p = parse_args(argc, argv);

	fd = fopen(p->rom_path, "r");
	unsigned short int last_pc = 0;
	if(fd != NULL) {
		load_rom(fd);
		create_cpu_state();
		init_ram();

		if(p->breakpoint != 0) {
			printf("break at 0x%x\n", p->breakpoint);
			runto(p->breakpoint);
		}
		if(p->run == 1) {
			while(1) {
			    if(p->dump_zero)
				dump_zero();
			    if(p->dump_stack)
				dump_stack();
			    if(p->dump_ram)
				dump_ram();
			    if(p->dump_all)
				dump_all();
			    step();
			    if(last_pc == get_current_cpu_state()->pc)
				exit(0);
			    last_pc = get_current_cpu_state()->pc;
			    printf("\n");
			}
		}

		while(1){
		    
		    if(p->dump_zero)
			dump_zero();
		    if(p->dump_stack)
			dump_stack();
		    if(p->dump_ram)
			dump_ram();
		    if(p->dump_all)
			dump_all();
		    step();

		    while (getchar() != '\n');
		}
	}
	else {
		printf("impossible d'ouvrir la rom.\n");
	}
	
    return 0;
}


