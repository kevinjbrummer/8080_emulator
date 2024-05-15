#include "emulator8080.hpp"
#include "display.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <chrono>

int main(int argc, char* argv[])
{
  Emulator8080 emulator8080;
  Display display("Space Invaders");

  if (!emulator8080.LoadRom(argv[1]))
  {
    exit(EXIT_FAILURE);
  }

  bool quit = false;
  auto lastInterrupt = std::chrono::high_resolution_clock::now();
  int interruptNum = 1;
  while (!quit)
  {
    quit = display.ProcessInput();
    emulator8080.Cycle();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastInterrupt).count();

    if (emulator8080.interuptEnabled)
    {
      if(dt > 1.0/30.0){

        emulator8080.GenerateInterupt(interruptNum);
        lastInterrupt = currentTime;
        if (interruptNum == 1)
        {
          interruptNum = 2;
        }
        else
        {
          interruptNum = 1;
        }
      }
    }

    display.Update(emulator8080.display);
  
  }
  return 0;
}