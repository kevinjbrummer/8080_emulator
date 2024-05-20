#include "emulator8080.hpp"
#include "display.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <cstdio>

int main(int argc, char* argv[])
{
  Emulator8080 emulator8080;
  Display display("Space Invaders");

  if (!emulator8080.LoadRom())
  {
    exit(EXIT_FAILURE);
  }

  bool quit = false;
  auto lastInterrupt = std::chrono::high_resolution_clock::now();
  int interruptNum = 1;
  auto lastDraw = std::chrono::high_resolution_clock::now();
  auto lastTimer = std::chrono::high_resolution_clock::now();
  while (!quit)
  {

    if (display.ProcessInput() || emulator8080.halt)
    {
      quit = true;
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastInterrupt).count();

    float dtDraw = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastDraw).count();

      if(dt >= 1.0/60.0){
    if (emulator8080.interuptEnabled)
    {

        emulator8080.GenerateInterupt(interruptNum);
        if (interruptNum == 1)
        {
          interruptNum = 2;
        }
        else
        {
          interruptNum = 1;
        }
        lastInterrupt = currentTime;
      }
    }

    float dc = std::chrono::duration<float, std::chrono::microseconds::period>(currentTime - lastTimer).count();
    int cyclesToCatchUp = 2 * dc;
    int cycles = 0;
    while (cyclesToCatchUp > cycles)
    {
      cycles += emulator8080.Cycle();
    }

    if (dtDraw > 16.0)
    {
      display.Update(emulator8080.display);
      lastDraw = currentTime;

    }
    lastTimer = currentTime;
  
  }
  return 0;
}