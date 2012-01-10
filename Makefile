INCLUDES := -Iinclude
CFLAGS := -Wall -g $(INCLUDES)
OBJ := obj/
SRC := src/
CC := gcc

OBJECTS=src/main.o src/rom.o src/opcodes.o src/cpu.o src/ppu.o src/memory.o src/framebuffer.o
OUTPUT=yane

all: YANE

run: all
	./yane | less

YANE:$(OBJECTS)
	@$(CC) `sdl-config --cflags --libs` -o $(OUTPUT) $^
	@echo "CC $<"

%.o: $(SRC)%.c
	@$(CC) -c -o $@ $<
	@echo "CC $<"

clean:
	rm -rf $(OBJECTS) $(OUTPUT)
