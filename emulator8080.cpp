#include "emulator8080.hpp"
#include <stdlib.h>
#include <stdio.h>

Emulator8080::Emulator8080()
{
  memory = (uint8_t*)calloc(16*1024, 1);
  display = &memory[0x2400];
  pc = 0;
  halt = false;
  interuptEnabled = false;
}

bool Emulator8080::LoadRom(char* file)
{
  FILE* rom;
  rom = fopen(file, "rb");
  if (rom == NULL)
  {
    printf("Error opening rom file\n");
    return false;
  }

  fseek(rom, 0, SEEK_END);
  int size = ftell(rom);
  rewind(rom);

  fread(memory, 1, size, rom);
  fclose(rom);

  return true;
}

int Emulator8080::Parity(int x, int size)
{
  int i;
  int p = 0;
  x = (x & ((1 << size) - 1));

  for (i; i < size; i++)
  {
    if (x & 0x1)
    {
      p++;
    }
    x >>= 1;
  }
  return (0 == (p & 0x1));
}

void Emulator8080::Cycle()
{
  uint8_t* code = &memory[pc];
  // printf("%04x %02x\n", pc, code[0]);
  switch (code[0])
  {
    case 0x00: Op0x00(); break;
    case 0x01: Op0x01(code); break;
    case 0x05: Op0x05(); break;
    case 0x06: Op0x06(code); break;
    case 0x09: Op0x09(); break;
    case 0x0C: Op0x0C(); break;
    case 0x0D: Op0x0D(); break;
    case 0x0E: Op0x0E(code); break;
    case 0x0F: Op0x0F(); break;
    case 0x11: Op0x11(code); break;
    case 0x13: Op0x13(); break;
    case 0x19: Op0x19(); break;
    case 0x1A: Op0x1A(); break;
    case 0x21: Op0x21(code); break;
    case 0x23: Op0x23(); break;
    case 0x26: Op0x26(code); break;
    case 0x29: Op0x29(); break;
    case 0x31: Op0x31(code); break;
    case 0x32: Op0x32(code); break;
    case 0x36: Op0x36(code); break;
    case 0x3A: Op0x3A(code); break;
    case 0x3E: Op0x3E(code); break;
    case 0x56: Op0x56(); break;
    case 0x5E: Op0x5E(); break;
    case 0x66: Op0x66(); break;
    case 0x6F: Op0x6F(); break;
    case 0x77: Op0x77(); break;
    case 0x7A: Op0x7A(); break;
    case 0x7B: Op0x7B(); break;
    case 0x7C: Op0x7C(); break;
    case 0x7E: Op0x7E(); break;
    case 0xA7: Op0xA7(); break;
    case 0xAF: Op0xAF(); break;
    case 0xC1: Op0xC1(); break;
    case 0xC2: Op0xC2(code); break;
    case 0xC3: Op0xC3(code); break;
    case 0xC5: Op0xC5(); break;
    case 0xC6: Op0xC6(code); break;
    case 0xC9: Op0xC9(); break;
    case 0xCD: Op0xCD(code); break;
    case 0xD1: Op0xD1(); break;
    case 0xD3: Op0xD3(code); break;
    case 0xD5: Op0xD5(); break;
    case 0xE1: Op0xE1(); break;
    case 0xE5: Op0xE5(); break;
    case 0xE6: Op0xE6(code); break;
    case 0xEB: Op0xEB(); break;
    case 0xF1: Op0xF1(); break;
    case 0xF5: Op0xF5(); break;
    case 0xFB: Op0xFB(); break;
    case 0xFE: Op0xFE(code); break;
    default: UnimplementedOp(code); break;
  }
}

void Emulator8080::Op0x00()
{
  pc++;
}

void Emulator8080::Op0x01(uint8_t* code)
{
  registers.b = code[2];
  registers.c = code[1];
  pc += 3;
}

void Emulator8080::Op0x05()
{
  uint8_t res = registers.b - 1;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  registers.b = res;
  pc++;
}


void Emulator8080::Op0x06(uint8_t* code)
{
  registers.b = code[1];
  pc += 2;
}

void Emulator8080::Op0x09()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t bc = (registers.b <<8) | registers.c;
  uint32_t res = hl + bc;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
  pc++; 
}

void Emulator8080::Op0x0C()
{
  uint8_t res = registers.c + 1;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  registers.c = res;
  pc++;
}

void Emulator8080::Op0x0D()
{
  uint8_t res = registers.c - 1;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  registers.c = res;
  pc++;
}

void Emulator8080::Op0x0E(uint8_t* code)
{
  registers.c = code[1];
  pc += 2;
}

void Emulator8080::Op0x0F()
{
  uint8_t res = registers.a >> 1;
  registers.a = ((res & 0x1) << 7) | (res >> 0x1);
  conditionCodes.cy = (1 == (res & 0x1));
  pc++;
}

void Emulator8080::Op0x11(uint8_t* code)
{
  registers.d = code[2];
  registers.e = code[1];
  pc += 3;
}

void Emulator8080::Op0x13()
{
  registers.e++;
  if (registers.e == 0)
  {
    registers.d++;
  }
  pc++;
}

void Emulator8080::Op0x19()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t de = (registers.d <<8) | registers.e;
  uint32_t res = hl + de;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
  pc++; 
}

void Emulator8080::Op0x1A()
{
  uint16_t address = (registers.d << 8) | registers.e;
  registers.a = memory[address];
  pc++;
}

void Emulator8080::Op0x21(uint8_t* code)
{
  registers.h = code[2];
  registers.l = code[1];
  pc += 3;
}

void Emulator8080::Op0x23()
{
  registers.l++;
  if (registers.l == 0){
    registers.h++;
  }
  pc++;
}

void Emulator8080::Op0x26(uint8_t* code)
{
  registers.h = code[1];
  pc += 2;
}

void Emulator8080::Op0x29()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t res = hl + hl;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
  pc++; 
}

void Emulator8080::Op0x31(uint8_t* code)
{
  uint16_t value = (code[2] << 8) | code[1];
  sp = value;
  pc += 3;
}

void Emulator8080::Op0x32(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  memory[address] = registers.a;
  pc += 3;
}

void Emulator8080::Op0x36(uint8_t* code)
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = code[1];
  pc += 2;
}

void Emulator8080::Op0x3A(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  registers.a = memory[address];
  pc += 3;
}

void Emulator8080::Op0x3E(uint8_t* code)
{
  registers.a = code[1];
  pc += 2;
}

void Emulator8080::Op0x56()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.d = memory[address];
  pc++;
}

void Emulator8080::Op0x5E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.e = memory[address];
  pc++;
}

void Emulator8080::Op0x66()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.h = memory[address];
  pc++;
}

void Emulator8080::Op0x6F()
{
  registers.l = registers.a;
  pc++;
}

void Emulator8080::Op0x77()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.a;
  pc++;
}

void Emulator8080::Op0x7A()
{
  registers.a = registers.d;
  pc++;
}

void Emulator8080::Op0x7B()
{
  registers.a = registers.e;
  pc++;
}

void Emulator8080::Op0x7C()
{
  registers.a = registers.h;
  pc++;
}

void Emulator8080::Op0x7E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.a = memory[address];
  pc++;
}

void Emulator8080::Op0xA7()
{
  uint8_t res = registers.a & registers.a;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  conditionCodes.cy = conditionCodes.ac = 0;
  registers.a = res;
  pc++;
}

void Emulator8080::Op0xAF()
{
  uint8_t res = registers.a ^ registers.a;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  conditionCodes.cy = conditionCodes.ac = 0;
  registers.a = res;
  pc++;
}

void Emulator8080::Op0xC1()
{
  registers.b = memory[sp + 1];
  registers.c = memory[sp];
  sp += 2;
  pc++;
}

void Emulator8080::Op0xC2(uint8_t* code)
{
  if (conditionCodes.z == 0)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 3;
  }
}

void Emulator8080::Op0xC3(uint8_t* code)
{
  pc = (code[2]) << 8 | code[1];
}

void Emulator8080::Op0xC5()
{
  sp -= 2;
  memory[sp + 1] = registers.b;
  memory[sp] = registers.c;
  pc++;
}

void Emulator8080::Op0xC6(uint8_t* code)
{
  uint16_t res = (uint16_t)registers.a + (uint16_t)code[1];
  conditionCodes.z = ((res & 0xFF) == 0);
  conditionCodes.s = ((res & 0x80) == 0x80);
  conditionCodes.p = Parity((res & 0xFF), 8);
  conditionCodes.cy = (res > 0xFF);
  registers.a = (res & 0xFF);
  pc += 2;
}

void Emulator8080::Op0xC9()
{
  pc = (memory[sp + 1] << 8) | memory[sp];
  sp += 2;
}

void Emulator8080::Op0xCD(uint8_t* code)
{
  uint16_t address = (code[2]) << 8 | code[1];
  uint16_t retAddress = pc + 3;
  memory[sp - 1] = (retAddress >> 8) & 0xFF;
  memory[sp - 2] = retAddress & 0xFF;
  sp -= 2;
  pc = address;
}

void Emulator8080::Op0xD1()
{
  registers.d = memory[sp + 1];
  registers.e = memory[sp];
  sp += 2;
  pc++;
}

void Emulator8080::Op0xD3(uint8_t* code)
{
  //TODO: Implement OUT
  pc += 2;
}

void Emulator8080::Op0xD5()
{
  sp -= 2;
  memory[sp + 1] = registers.d;
  memory[sp] = registers.e;
  pc++;
}

void Emulator8080::Op0xE1()
{
  registers.h = memory[sp + 1];
  registers.l = memory[sp];
  sp += 2;
  pc++;
}

void Emulator8080::Op0xE5()
{
  sp -= 2;
  memory[sp + 1] = registers.h;
  memory[sp] = registers.l;
  pc++;
}

void Emulator8080::Op0xE6(uint8_t* code)
{
  uint8_t res = registers.a & code[1];
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  conditionCodes.cy = conditionCodes.ac = 0;
  registers.a = res;
  pc += 2;
}

void Emulator8080::Op0xEB()
{
  uint8_t temp = registers.h;
  registers.h = registers.d;
  registers.d = temp;

  temp = registers.l;
  registers.l = registers.e;
  registers.e = temp;
  pc++;
}

void Emulator8080::Op0xF1()
{
  registers.a = memory[sp + 1];
  uint8_t psw = memory[sp];
  conditionCodes.ac = (0x10 == (psw & 0x10));
  conditionCodes.cy = (0x8 == (psw & 0x8));
  conditionCodes.p = (0x4 == (psw & 0x4));
  conditionCodes.s = (0x2 == (psw & 0x2));
  conditionCodes.z = (0x1 == (psw & 0x1));
  sp += 2;
  pc++;
}

void Emulator8080::Op0xF5()
{
  sp -= 2;
  memory[sp + 1] = registers.a;
  uint8_t flags = (
    conditionCodes.ac << 4 |
    conditionCodes.cy << 3 |
    conditionCodes.p << 2 |
    conditionCodes.s << 1 |
    conditionCodes.z
  );
  memory[sp] = flags;
  pc++;
}

void Emulator8080::Op0xFE(uint8_t* code)
{
  uint8_t res = registers.a - code[1];
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  conditionCodes.cy = registers.a < code[1];
  pc += 2;
}

void Emulator8080::Op0xFB()
{
  interuptEnabled = true;
  pc++;
}

void Emulator8080::UnimplementedOp(uint8_t* code)
{
  printf("%04x %02x ", pc, code[0]);
  printf("Unimplemented Op. Stopping Program");
  halt = true;
}

void Emulator8080::GenerateInterupt(int interuptNum)
{
  memory[sp - 1] = (pc & 0xFF00) >> 8;
  memory[sp - 2] = pc & 0xFF;
  sp -= 2;
  pc = 8 * interuptNum;

  interuptEnabled = false;
}