#pragma once
#include "i8080Cpu.hpp"
#include "multimedia.hpp"
#include <stdlib.h>
#include <cstdint>
#include <chrono>

struct Ports
{
  uint8_t port1;
  uint8_t port2;
  uint8_t port3;
  uint8_t prevPort3;
  uint8_t port5;
  uint8_t prevPort5;
};

class Cabinet
{
  public:
    Cabinet();
    void Boot();

  private:
    I8080Cpu cpu;
    Multimedia multimedia;
    struct Ports ports;
    uint8_t shift0;
    uint8_t shift1;
    uint8_t shiftOffset;
    int interruptNum;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastInterrupt;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastCycleTimer;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastDrawTimer;

    std::chrono::time_point<std::chrono::high_resolution_clock> GetCurrentTime();
    uint8_t GetInput(uint8_t port);
    void SetOutput(uint8_t port, uint8_t value);
    void HandleInterrupt();
    void HandleCpuCycles();
    void HandleSounds();
    void HandleDisplay();
};