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
  uint8_t Port1Input = 0;
  uint8_t Port2Input = 0;
  uint8_t shift0 = 0;
  uint8_t shift1 = 0;
  uint8_t shiftOffset = 0;
  while (!quit)
  {

    quit = display.ProcessInput(&Port1Input, &Port2Input);

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
      uint8_t* op = &emulator8080.memory[emulator8080.pc];
      switch (op[0])
      {
      case 0xDB:
        {
          switch (op[1])
          {
          case 0:
            emulator8080.registers.a = 1;
            break;
          case 1:
            emulator8080.registers.a = Port1Input;
            break;
          case 2:
            emulator8080.registers.a = Port2Input;
            break;
          case 3:
            {
              uint16_t value = (shift1 << 8) | shift0;
              emulator8080.registers.a = ((value >> (8 - shiftOffset)) & 0xFF);
            }
            break;
          default:
            break;
          }
        }
        emulator8080.pc += 2;
        cycles += 3;
        break;
      case 0xD3:
        {
          switch (op[1])
          {
          case 2:
            shiftOffset = emulator8080.registers.a & 0x7;
            break;
          case 4:
            shift0 = shift1;
            shift1 = emulator8080.registers.a;
            break;
          default:
            break;
          }
        }
        emulator8080.pc += 2;
        cycles += 3;
        break;
      default:
        cycles += emulator8080.Cycle();
        break;
      }
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