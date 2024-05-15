#include "emulator8080.hpp"
#include <stdlib.h>
#include <stdio.h>

Emulator8080::Emulator8080()
{
  memory = new uint8_t[1024 * 16]();
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

void Emulator8080::LogicFlagsA()
{
  conditionCodes.z = (registers.a == 0);
  conditionCodes.s = (0x80 == (registers.a & 0x80));
  conditionCodes.p = Parity(registers.a, 8);
  conditionCodes.cy = conditionCodes.ac = 0;
}

void Emulator8080::ArithFlagsA(uint16_t res)
{
  conditionCodes.z = ((res & 0xFF) == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity((res & 0xFF), 8);
  conditionCodes.cy = (res > 0xFF);
}

void Emulator8080::Cycle()
{
  uint8_t* code = &memory[pc];
  // printf("%04x %02x\n", pc, code[0]);
  switch (code[0])
  {
    case 0x00: Op0x00(); break;
    case 0x01: Op0x01(code); break;
    case 0x03: Op0x03(); break;
    case 0x05: Op0x05(); break;
    case 0x06: Op0x06(code); break;
    case 0x09: Op0x09(); break;
    case 0x0A: Op0x0A(); break;
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
    case 0x35: Op0x35(); break;
    case 0x36: Op0x36(code); break;
    case 0x37: Op0x37(); break;
    case 0x3A: Op0x3A(code); break;
    case 0x3D: Op0x3D(); break;
    case 0x3E: Op0x3E(code); break;
    case 0x40: Op0x40(); break;
    case 0x41: Op0x41(); break;
    case 0x42: Op0x42(); break;
    case 0x43: Op0x43(); break;
    case 0x44: Op0x44(); break;
    case 0x45: Op0x45(); break;
    case 0x46: Op0x46(); break;
    case 0x47: Op0x47(); break;
    case 0x48: Op0x48(); break;
    case 0x49: Op0x49(); break;
    case 0x4A: Op0x4A(); break;
    case 0x4B: Op0x4B(); break;
    case 0x4C: Op0x4C(); break;
    case 0x4D: Op0x4D(); break;
    case 0x4E: Op0x4E(); break;
    case 0x4F: Op0x4F(); break;
    case 0x50: Op0x50(); break;
    case 0x51: Op0x51(); break;
    case 0x52: Op0x52(); break;
    case 0x53: Op0x53(); break;
    case 0x54: Op0x54(); break;
    case 0x55: Op0x55(); break;
    case 0x56: Op0x56(); break;
    case 0x57: Op0x57(); break;
    case 0x58: Op0x58(); break;
    case 0x59: Op0x59(); break;
    case 0x5A: Op0x5A(); break;
    case 0x5B: Op0x5B(); break;
    case 0x5C: Op0x5C(); break;
    case 0x5D: Op0x5D(); break;
    case 0x5E: Op0x5E(); break;
    case 0x5F: Op0x5F(); break;
    case 0x60: Op0x60(); break;
    case 0x61: Op0x61(); break;
    case 0x62: Op0x62(); break;
    case 0x63: Op0x63(); break;
    case 0x64: Op0x64(); break;
    case 0x65: Op0x65(); break;
    case 0x66: Op0x66(); break;
    case 0x67: Op0x67(); break;
    case 0x68: Op0x68(); break;
    case 0x69: Op0x69(); break;
    case 0x6A: Op0x6A(); break;
    case 0x6B: Op0x6B(); break;
    case 0x6C: Op0x6C(); break;
    case 0x6D: Op0x6D(); break;
    case 0x6E: Op0x6E(); break;
    case 0x6F: Op0x6F(); break;
    case 0x70: Op0x70(); break;
    case 0x71: Op0x71(); break;
    case 0x72: Op0x72(); break;
    case 0x73: Op0x73(); break;
    case 0x74: Op0x74(); break;
    case 0x75: Op0x75(); break;
    case 0x76: Op0x76(); break;
    case 0x77: Op0x77(); break;
    case 0x78: Op0x78(); break;
    case 0x79: Op0x79(); break;
    case 0x7A: Op0x7A(); break;
    case 0x7B: Op0x7B(); break;
    case 0x7C: Op0x7C(); break;
    case 0x7D: Op0x7D(); break;
    case 0x7E: Op0x7E(); break;
    case 0x7F: Op0x7F(); break;
    case 0x80: Op0x80(); break;
    case 0x81: Op0x81(); break;
    case 0x82: Op0x82(); break;
    case 0x83: Op0x83(); break;
    case 0x84: Op0x84(); break;
    case 0x85: Op0x85(); break;
    case 0x86: Op0x86(); break;
    case 0x87: Op0x87(); break;
    case 0x88: Op0x88(); break;
    case 0x89: Op0x89(); break;
    case 0x8A: Op0x8A(); break;
    case 0x8B: Op0x8B(); break;
    case 0x8C: Op0x8C(); break;
    case 0x8D: Op0x8D(); break;
    case 0x8E: Op0x8E(); break;
    case 0x8F: Op0x8F(); break;
    case 0x90: Op0x90(); break;
    case 0x91: Op0x91(); break;
    case 0x92: Op0x92(); break;
    case 0x93: Op0x93(); break;
    case 0x94: Op0x94(); break;
    case 0x95: Op0x95(); break;
    case 0x96: Op0x96(); break;
    case 0x97: Op0x97(); break;
    case 0x98: Op0x98(); break;
    case 0x99: Op0x99(); break;
    case 0x9A: Op0x9A(); break;
    case 0x9B: Op0x9B(); break;
    case 0x9C: Op0x9C(); break;
    case 0x9D: Op0x9D(); break;
    case 0x9E: Op0x9E(); break;
    case 0xA0: Op0xA0(); break;
    case 0xA1: Op0xA1(); break;
    case 0xA2: Op0xA2(); break;
    case 0xA3: Op0xA3(); break;
    case 0xA4: Op0xA4(); break;
    case 0xA5: Op0xA5(); break;
    case 0xA6: Op0xA6(); break;
    case 0xA7: Op0xA7(); break;
    case 0xA8: Op0xA8(); break;
    case 0xA9: Op0xA9(); break;
    case 0xAA: Op0xAA(); break;
    case 0xAB: Op0xAB(); break;
    case 0xAC: Op0xAC(); break;
    case 0xAD: Op0xAD(); break;
    case 0xAE: Op0xAE(); break;
    case 0xAF: Op0xAF(); break;
    case 0xB0: Op0xB0(); break;
    case 0xC1: Op0xC1(); break;
    case 0xC2: Op0xC2(code); break;
    case 0xC3: Op0xC3(code); break;
    case 0xC5: Op0xC5(); break;
    case 0xC6: Op0xC6(code); break;
    case 0xC8: Op0xC8(); break;
    case 0xC9: Op0xC9(); break;
    case 0xCA: Op0xCA(code); break;
    case 0xCD: Op0xCD(code); break;
    case 0xD1: Op0xD1(); break;
    case 0xD2: Op0xD2(code); break;
    case 0xD3: Op0xD3(code); break;
    case 0xD5: Op0xD5(); break;
    case 0xDA: Op0xDA(code); break;
    case 0xDB: Op0xDB(code); break;
    case 0xE1: Op0xE1(); break;
    case 0xE3: Op0xE3(); break;
    case 0xE5: Op0xE5(); break;
    case 0xE6: Op0xE6(code); break;
    case 0xE9: Op0xE9(); break;
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

void Emulator8080::Op0x03()
{
  registers.c++;
  if (registers.c == 0)
  {
    registers.b++;
  }
  pc++;
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


void Emulator8080::Op0x0A()
{
  uint8_t address = (registers.b << 8) | registers.c;
  registers.a = memory[address];
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

void Emulator8080::Op0x35()
{
  uint16_t address = (registers.h << 8) | registers.l;
  uint8_t res = memory[address] - 1;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  memory[address] = res;
  pc++;
}

void Emulator8080::Op0x36(uint8_t* code)
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = code[1];
  pc += 2;
}

void Emulator8080::Op0x37()
{
  conditionCodes.cy = 1;
  pc++;
}

void Emulator8080::Op0x3A(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  registers.a = memory[address];
  pc += 3;
}

void Emulator8080::Op0x3D()
{
  uint8_t res = registers.a - 1;
  conditionCodes.z = (res == 0);
  conditionCodes.s = (0x80 == (res & 0x80));
  conditionCodes.p = Parity(res, 8);
  registers.a = res;
  pc++;
}

void Emulator8080::Op0x3E(uint8_t* code)
{
  registers.a = code[1];
  pc += 2;
}

void Emulator8080::Op0x40()
{
  registers.b = registers.b;
  pc++;
}

void Emulator8080::Op0x41()
{
  registers.b = registers.c;
  pc++;
}

void Emulator8080::Op0x42()
{
  registers.b = registers.d;
  pc++;
}

void Emulator8080::Op0x43()
{
  registers.b = registers.e;
  pc++;
}

void Emulator8080::Op0x44()
{
  registers.b = registers.h;
  pc++;
}

void Emulator8080::Op0x45()
{
  registers.b = registers.l;
  pc++;
}

void Emulator8080::Op0x46()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.b = memory[address];
  pc++;
}

void Emulator8080::Op0x47()
{
  registers.b = registers.a;
  pc++;
}

void Emulator8080::Op0x48()
{
  registers.c = registers.b;
  pc++;
}

void Emulator8080::Op0x49()
{
  registers.c = registers.c;
  pc++;
}

void Emulator8080::Op0x4A()
{
  registers.c = registers.d;
  pc++;
}

void Emulator8080::Op0x4B()
{
  registers.c = registers.e;
  pc++;
}

void Emulator8080::Op0x4C()
{
  registers.c = registers.h;
  pc++;
}

void Emulator8080::Op0x4D()
{
  registers.c = registers.l;
  pc++;
}

void Emulator8080::Op0x4E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.c = memory[address];
  pc++;
}

void Emulator8080::Op0x4F()
{
  registers.c = registers.a;
  pc++;
}

void Emulator8080::Op0x50()
{
  registers.d = registers.b;
  pc++;
}

void Emulator8080::Op0x51()
{
  registers.d = registers.c;
  pc++;
}

void Emulator8080::Op0x52()
{
  registers.d = registers.d;
  pc++;
}

void Emulator8080::Op0x53()
{
  registers.d = registers.e;
  pc++;
}

void Emulator8080::Op0x54()
{
  registers.d = registers.h;
  pc++;
}

void Emulator8080::Op0x55()
{
  registers.d = registers.l;
  pc++;
}

void Emulator8080::Op0x56()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.d = memory[address];
  pc++;
}

void Emulator8080::Op0x57()
{
  registers.d = registers.a;
  pc++;
}

void Emulator8080::Op0x58()
{
  registers.e = registers.b;
  pc++;
}

void Emulator8080::Op0x59()
{
  registers.e = registers.c;
  pc++;
}

void Emulator8080::Op0x5A()
{
  registers.e = registers.d;
  pc++;
}

void Emulator8080::Op0x5B()
{
  registers.e = registers.e;
  pc++;
}

void Emulator8080::Op0x5C()
{
  registers.e = registers.h;
  pc++;
}

void Emulator8080::Op0x5D()
{
  registers.e = registers.l;
  pc++;
}

void Emulator8080::Op0x5E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.e = memory[address];
  pc++;
}

void Emulator8080::Op0x5F()
{
  registers.e = registers.a;
  pc++;
}

void Emulator8080::Op0x60()
{
  registers.h = registers.b;
  pc++;
}

void Emulator8080::Op0x61()
{
  registers.h = registers.c;
  pc++;
}

void Emulator8080::Op0x62()
{
  registers.h = registers.d;
  pc++;
}

void Emulator8080::Op0x63()
{
  registers.h = registers.e;
  pc++;
}

void Emulator8080::Op0x64()
{
  registers.h = registers.h;
  pc++;
}

void Emulator8080::Op0x65()
{
  registers.h = registers.l;
  pc++;
}

void Emulator8080::Op0x66()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.h = memory[address];
  pc++;
}

void Emulator8080::Op0x67()
{
  registers.h = registers.a;
  pc++;
}

void Emulator8080::Op0x68()
{
  registers.l = registers.b;
  pc++;
}

void Emulator8080::Op0x69()
{
  registers.l = registers.c;
  pc++;
}

void Emulator8080::Op0x6A()
{
  registers.l = registers.d;
  pc++;
}

void Emulator8080::Op0x6B()
{
  registers.l = registers.e;
  pc++;
}

void Emulator8080::Op0x6C()
{
  registers.l = registers.h;
  pc++;
}

void Emulator8080::Op0x6D()
{
  registers.l = registers.l;
  pc++;
}

void Emulator8080::Op0x6E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.l = memory[address];
  pc++;
}

void Emulator8080::Op0x6F()
{
  registers.l = registers.a;
  pc++;
}

void Emulator8080::Op0x70()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.b;
  pc++;
}

void Emulator8080::Op0x71()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.c;
  pc++;
}

void Emulator8080::Op0x72()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.d;
  pc++;
}

void Emulator8080::Op0x73()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.e;
  pc++;
}

void Emulator8080::Op0x74()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.h;
  pc++;
}

void Emulator8080::Op0x75()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.l;
  pc++;
}

void Emulator8080::Op0x76()
{
  halt = true;
}

void Emulator8080::Op0x77()
{
  uint16_t address = (registers.h << 8) | registers.l;
  memory[address] = registers.a;
  pc++;
}

void Emulator8080::Op0x78()
{
  registers.a = registers.b;
  pc++;
}

void Emulator8080::Op0x79()
{
  registers.a = registers.c;
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

void Emulator8080::Op0x7D()
{
  registers.a = registers.l;
  pc++;
}

void Emulator8080::Op0x7E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.a = memory[address];
  pc++;
}

void Emulator8080::Op0x7F()
{
  registers.a = registers.a;
  pc++;
}

void Emulator8080::Op0x80()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.b;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x81()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.c;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x82()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.d;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x83()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.e;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x84()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.h;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x85()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.l;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x86()
{
  uint16_t address = (registers.h << 8) | registers.l;
  uint16_t res = (uint16_t)registers.a + (uint16_t) memory[address];
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x87()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.a;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x88()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.b + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x89()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.c + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x8A()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.d + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x8B()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.e + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x8C()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.h + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x8D()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.l + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x8E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  uint16_t res = (uint16_t)registers.a + (uint16_t) memory[address] + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x8F()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.a + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x90()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.b;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x91()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.c;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x92()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.d;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x93()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.e;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x94()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.h;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x95()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.l;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x96()
{
  uint16_t address = (registers.h << 8) | registers.l;
  uint16_t res = (uint16_t)registers.a - (uint16_t) memory[address];
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x97()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.a;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x98()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.b - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x99()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.c - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x9A()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.d - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x9B()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.e - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x9C()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.h - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x9D()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.l - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x9E()
{
  uint16_t address = (registers.h << 8) | registers.l;
  uint16_t res = (uint16_t)registers.a - (uint16_t) memory[address] - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0x9F()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.a - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0xA0()
{
  registers.a = registers.a & registers.b;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA1()
{
  registers.a = registers.a & registers.c;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA2()
{
  registers.a = registers.a & registers.d;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA3()
{
  registers.a = registers.a & registers.e;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA4()
{
  registers.a = registers.a & registers.h;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA5()
{
  registers.a = registers.a & registers.l;
  LogicFlagsA();
  pc++;
}


void Emulator8080::Op0xA6()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.a = registers.a & memory[address];
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA7()
{
  registers.a = registers.a & registers.a;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA8()
{
  registers.a = registers.a ^ registers.b;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xA9()
{
  registers.a = registers.a ^ registers.c;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xAA()
{
  registers.a = registers.a ^ registers.d;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xAB()
{
  registers.a = registers.a ^ registers.e;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xAC()
{
  registers.a = registers.a ^ registers.h;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xAD()
{
  registers.a = registers.a ^ registers.l;
  LogicFlagsA();
  pc++;
}


void Emulator8080::Op0xAE()
{
  uint16_t address = (registers.h << 8) | registers.l;
  registers.a = registers.a ^ memory[address];
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xAF()
{
  registers.a = registers.a ^ registers.a;
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xB0()
{
  uint8_t res = registers.a | registers.b;
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

void Emulator8080::Op0xC8()
{
  if (conditionCodes.z == 1)
  {
    pc = (memory[sp+ 1] << 8) | memory[sp];
    sp += 2;
  }
  else
  {
    pc++;
  }
}

void Emulator8080::Op0xC9()
{
  pc = (memory[sp + 1] << 8) | memory[sp];
  sp += 2;
}

void Emulator8080::Op0xCA(uint8_t* code)
{
  if (conditionCodes.z == 1)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 3;
  }
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

void Emulator8080::Op0xD2(uint8_t* code)
{
  if (conditionCodes.cy != 1)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 3;
  }
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

void Emulator8080::Op0xDA(uint8_t* code)
{
  if (conditionCodes.cy == 1)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 3;
  }
}

void Emulator8080::Op0xDB(uint8_t* code)
{
  //TODO: Implement IN
  pc += 2;
}

void Emulator8080::Op0xE1()
{
  registers.h = memory[sp + 1];
  registers.l = memory[sp];
  sp += 2;
  pc++;
}

void Emulator8080::Op0xE3()
{
  uint8_t temp;

  temp = registers.l;
  registers.l = memory[sp];
  memory[sp] = temp;

  temp = registers.h;
  registers.h = memory[sp + 1];
  memory[sp + 1] = temp;

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

void Emulator8080::Op0xE9()
{
  pc = (registers.h << 8) | registers.l;
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
  printf("Unimplemented Op. Stopping Program\n");
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