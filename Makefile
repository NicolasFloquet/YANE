INCLUDES := -Iinclude
CFLAGS := -Wall -g $(INCLUDES)
OBJ := obj/
SRC := src/
CC := colorgcc

OBJECTS=src/main.o src/rom.o src/opcodes.o src/cpu.o src/memory.o
OUTPUT=yane

all: YANE

run: all
	./yane | less

YANE:$(OBJECTS)
	@$(CC) -o $(OUTPUT) $^
	@echo "CC $<"

%.o: $(SRC)%.c
	@$(CC) -c -o $@ $<
	@echo "CC $<"

clean:
	rm -rf $(OBJECTS) $(OUTPUT)
