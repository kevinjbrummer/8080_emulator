#include "cabinet.hpp"
#include <stdlib.h>
#include <cstdint>
#include <chrono>

const float INTERRUPT_TIMING = 1.0/60.0; //60 times per second
const float DRAW_TIMING = 16.0; //16 milliseconds

Cabinet::Cabinet()
  : cpu(), multimedia("Space Invaders")
{
  shift0 = 0;
  shift1 = 0;
  shiftOffset = 0;
  ports.port1 = 0;
  ports.port2 = 0;
  ports.port3 = 0;
  ports.prevPort3 = 0;
  ports.port5 = 0;
  ports.prevPort5 = 0;
  interruptNum = 1;

  quit = false;

  auto now = GetCurrentTime();
  lastInterrupt = now;
  lastCycleTimer = now;
  lastDrawTimer = now;
}

std::chrono::time_point<std::chrono::high_resolution_clock> Cabinet::GetCurrentTime()
{
  return std::chrono::high_resolution_clock::now();
}

uint8_t Cabinet::GetInput(uint8_t port)
{
  switch (port)
  {
  case 0:
    return 1;
  case 1:
    return ports.port1;
  case 2:
    return ports.port2;
  case 3:
    {
      uint16_t value = (shift1 << 8) | shift0;
      return ((value >> (8 - shiftOffset)) & 0xFF);
    }
  default:
    return 0;
  }
}

void Cabinet::SetOutput(uint8_t port, uint8_t value)
{
  switch (port)
  {
  case 2:
    shiftOffset = value & 0x7;
    break;
  case 3:
    ports.port3 = value;
    break;
  case 4:
    shift0 = shift1;
    shift1 = value;
    break;
  case 5:
    ports.port5 = value;
    break;
  default:
    break;
  }
}

void Cabinet::HandleInterrupt()
{
  float passedInterruptTime = std::chrono::duration<float, std::chrono::seconds::period>(GetCurrentTime() - lastInterrupt).count();

    if(passedInterruptTime >= INTERRUPT_TIMING && cpu.interuptEnabled)
    {
      cpu.GenerateInterupt(interruptNum);
      interruptNum = (interruptNum == 1) ? 2 : 1;
      lastInterrupt = GetCurrentTime();
    }
}

void Cabinet::HandleCpuCycles()
{
  float passedCycleTime = std::chrono::duration<float, std::chrono::microseconds::period>(GetCurrentTime() - lastCycleTimer).count();
  int cyclesToCatchUp = 2 * passedCycleTime;
  int cycles = 0;
  while (cyclesToCatchUp > cycles)
  {
    uint8_t* op = &cpu.memory[cpu.pc];
    switch (op[0])
    {
    case 0xDB: //input
      cpu.WriteRegisterA(GetInput(op[1]));
      cpu.pc += 2;
      cycles += 3;
      break;
    case 0xD3: //output
      SetOutput(op[1], cpu.ReadRegisterA());
      cpu.pc += 2;
      cycles += 3;
      break;
    default:
      cycles += cpu.Cycle();
      break;
    }
  }
  lastCycleTimer = GetCurrentTime();
}

void Cabinet::HandleSounds()
{
  if (ports.port3 != ports.prevPort3)
  {
    multimedia.PlayPortSounds(ports.port3, ports.prevPort3, 3);
    ports.prevPort3 = ports.port3;
  }

  if (ports.port5 != ports.prevPort5)
  {
    multimedia.PlayPortSounds(ports.port5, ports.prevPort5, 5);
    ports.prevPort5 = ports.port5;
  }
}

void Cabinet::HandleDisplay()
{
  float passedDrawTime = std::chrono::duration<float, std::chrono::milliseconds::period>(GetCurrentTime() - lastDrawTimer).count();

  if (passedDrawTime > DRAW_TIMING)
  {
    multimedia.UpdateDisplay(cpu.display);
    lastDrawTimer = GetCurrentTime();
  }
}

void Cabinet::Boot()
{
  if (!cpu.LoadRom())
  {
    exit(EXIT_FAILURE);
  }

  while (!quit)
  {

    quit = multimedia.HandleEvents(&ports.port1, &ports.port2);

    HandleInterrupt();
    if (!multimedia.isPaused)
    {
      HandleCpuCycles();
      HandleSounds();
      HandleDisplay();
    }
  }
}