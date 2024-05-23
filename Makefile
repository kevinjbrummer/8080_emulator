OBJS = main.cpp i8080Cpu.cpp multimedia.cpp

CC = g++

LINKED_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer

OBJ_NAME = space_invaders_emulator.exe

all : $(OBJS)
	$(CC) $(OBJS) $(LINKED_FLAGS) -o $(OBJ_NAME)