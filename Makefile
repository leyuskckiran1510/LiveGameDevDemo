RAYLIB_DIR=raylib
OUTPUT=game.out

main:
	gcc src/main.c -L$(RAYLIB_DIR)/lib -I$(RAYLIB_DIR)/include -Wl,-rpath,$(RAYLIB_DIR)/lib -lraylib -lm -o $(OUTPUT)
	./$(OUTPUT)