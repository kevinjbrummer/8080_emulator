#include "emulator8080.hpp"
#include "display.hpp"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  Emulator8080 emulator8080;
  Display display("Space Invaders");

  if (!emulator8080.LoadRom(argv[1]))
  {
    exit(EXIT_FAILURE);
  }

  bool quit = false;
  while (!quit)
  {
    quit = display.ProcessInput();
    emulator8080.Cycle();

    display.Update(emulator8080.display);
  
  }
  return 0;
}