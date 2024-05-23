#include "i8080Cpu.hpp"
#include "display.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <cstdio>

int main(int argc, char* argv[])
{
  I8080Cpu cpu;
  Display display("Space Invaders");

  if (!cpu.LoadRom())
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
  uint8_t Port3Output = 0;
  uint8_t PrevPort3Output = 0;
  uint8_t Port5Output = 0;
  uint8_t PrevPort5Output = 0;
  uint8_t shift0 = 0;
  uint8_t shift1 = 0;
  uint8_t shiftOffset = 0;

  display.ToggleMusic();
  while (!quit)
  {

    quit = display.ProcessInput(&Port1Input, &Port2Input);

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastInterrupt).count();

    float dtDraw = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastDraw).count();

      if(dt >= 1.0/60.0){
    if (cpu.interuptEnabled)
    {

        cpu.GenerateInterupt(interruptNum);
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
      uint8_t* op = &cpu.memory[cpu.pc];
      switch (op[0])
      {
      case 0xDB:
        {
          switch (op[1])
          {
          case 0:
            cpu.registers.a = 1;
            break;
          case 1:
            cpu.registers.a = Port1Input;
            break;
          case 2:
            cpu.registers.a = Port2Input;
            break;
          case 3:
            {
              uint16_t value = (shift1 << 8) | shift0;
              cpu.registers.a = ((value >> (8 - shiftOffset)) & 0xFF);
            }
            break;
          default:
            break;
          }
        }
        cpu.pc += 2;
        cycles += 3;
        break;
      case 0xD3:
        {
          switch (op[1])
          {
          case 2:
            shiftOffset = cpu.registers.a & 0x7;
            break;
          case 3:
            Port3Output = cpu.registers.a;
            break;
          case 4:
            shift0 = shift1;
            shift1 = cpu.registers.a;
            break;
        case 5:
            Port5Output = cpu.registers.a;
            break;
          default:
            break;
          }
        }
        cpu.pc += 2;
        cycles += 3;
        break;
      default:
        cycles += cpu.Cycle();
        break;
      }
    }

    if (Port3Output != PrevPort3Output)
    {
      display.PlayPort3Sounds(Port3Output, PrevPort3Output);
    }
    PrevPort3Output = Port3Output;

    if (Port5Output != PrevPort5Output)
    {
      display.PlayPort5Sounds(Port5Output, PrevPort5Output);
    }
    PrevPort5Output = Port5Output;


    if (dtDraw > 16.0)
    {
      display.Update(cpu.display);
      lastDraw = currentTime;

    }
    lastTimer = currentTime;
  
  }
  return 0;
}