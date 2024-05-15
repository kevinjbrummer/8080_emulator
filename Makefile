OBJS = main.cpp emulator8080.cpp display.cpp

CC = g++

LINKED_FLAGS = -lmingw32 -lSDL2main -lSDL2

OBJ_NAME = space_invaders_emulator.exe

all : $(OBJS)
	$(CC) $(OBJS) $(LINKED_FLAGS) -o $(OBJ_NAME)