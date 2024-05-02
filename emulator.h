#include <stdio.h>
#include <stdint.h>
#include <vector>
typedef unsigned char BYTE;

struct ConditionCodes {
  uint8_t z   = 1;
  uint8_t s   = 1;
  uint8_t p   = 1;
  uint8_t cy  = 1;
  uint8_t ac  = 1;
  uint8_t pad = 3;
};

class Emulator
{
  public:
    uint8_t             a;
    uint8_t             b;
    uint8_t             c;
    uint8_t             d;
    uint8_t             e;
    uint8_t             h;
    uint8_t             l;
    uint16_t            sp;
    uint16_t            pc;
    std::vector<BYTE>   memory;
    ConditionCodes      cc;
    uint8_t             int_enable;

    Emulator();
    void UnimplementedInstruction();
    void Emulate8080Op();
};