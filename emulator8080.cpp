#include "emulator8080.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>

unsigned char cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
};

Emulator8080::Emulator8080()
{
  memory = new uint8_t[1024 * 16]();
  display = &memory[0x2400];
  pc = 0;
  halt = false;
  interuptEnabled = false;
}

bool Emulator8080::LoadRom()
{
  FILE* rom;
  int size;
  rom = fopen("invaders/invaders.rom", "rb");
  if (rom == NULL)
  {
    printf("Error opening rom file\n");
    return false;
  }

  fseek(rom, 0, SEEK_END);
  size = ftell(rom);
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

  for (i=0; i < size; i++)
  {
    if (x & 0x1)
    {
      p++;
    }
    x = x >> 1;
  }
  return (p % 2 == 0);
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

void Emulator8080::FlagsZSP(uint8_t value)
{
  conditionCodes.z = (value == 0);
  conditionCodes.s = (0x80 == (value & 0x80));
  conditionCodes.p = Parity(value, 8);
}

void Emulator8080::Push(uint8_t high, uint8_t low)
{
  WriteMem(sp - 1, high);
  WriteMem(sp - 2, low);
  sp -= 2;
}

void Emulator8080::Pop(uint8_t* high, uint8_t* low)
{
  *high = memory[sp + 1];
  *low = memory[sp];
  sp += 2;
}

uint8_t Emulator8080::ReadFromHL()
{
  uint16_t address = (registers.h << 8) | registers.l;
  return memory[address];
}

void Emulator8080::WriteToHL(uint8_t value)
{
  uint16_t address = (registers.h << 8) | registers.l;
  WriteMem(address, value);
}

void Emulator8080::WriteMem(uint16_t address, uint8_t value)
{
  if (address < 0x2000)
  {
    // printf("Cant write to ROM %x\n", address);
    return;
  }
  if (address >= 0x4000)
  {
    // printf("Cant write out of RAM %x\n", address);
    return;
  }

  memory[address] = value;
}

uint8_t Emulator8080::Cycle()
{
  uint8_t* code = &memory[pc];
  pc++;
  switch (code[0])
  {
    case 0x00: break;
    case 0x01: Op0x01(code); break;
    case 0x02: Op0x02(); break;
    case 0x03: Op0x03(); break;
    case 0x04: Op0x04(); break;
    case 0x05: Op0x05(); break;
    case 0x06: Op0x06(code); break;
    case 0x07: Op0x07(); break;
    case 0x09: Op0x09(); break;
    case 0x0A: Op0x0A(); break;
    case 0x0B: Op0x0B(); break;
    case 0x0C: Op0x0C(); break;
    case 0x0D: Op0x0D(); break;
    case 0x0E: Op0x0E(code); break;
    case 0x0F: Op0x0F(); break;
    case 0x11: Op0x11(code); break;
    case 0x12: Op0x12(); break;
    case 0x13: Op0x13(); break;
    case 0x14: Op0x14(); break;
    case 0x15: Op0x15(); break;
    case 0x16: Op0x16(code); break;
    case 0x17: Op0x17(); break;
    case 0x19: Op0x19(); break;
    case 0x1A: Op0x1A(); break;
    case 0x1B: Op0x1B(); break;
    case 0x1C: Op0x1C(); break;
    case 0x1D: Op0x1D(); break;
    case 0x1E: Op0x1E(code); break;
    case 0x1F: Op0x1F(); break;
    case 0x21: Op0x21(code); break;
    case 0x22: Op0x22(code); break;
    case 0x23: Op0x23(); break;
    case 0x24: Op0x24(); break;
    case 0x25: Op0x25(); break;
    case 0x26: Op0x26(code); break;
    case 0x27: Op0x27(); break;
    case 0x29: Op0x29(); break;
    case 0x2A: Op0x2A(code); break;
    case 0x2B: Op0x2B(); break;
    case 0x2C: Op0x2C(); break;
    case 0x2D: Op0x2D(); break;
    case 0x2E: Op0x2E(code); break;
    case 0x2F: Op0x2F(); break;
    case 0x31: Op0x31(code); break;
    case 0x32: Op0x32(code); break;
    case 0x33: Op0x33(); break;
    case 0x34: Op0x34(); break;
    case 0x35: Op0x35(); break;
    case 0x36: Op0x36(code); break;
    case 0x37: Op0x37(); break;
    case 0x39: Op0x39(); break;
    case 0x3A: Op0x3A(code); break;
    case 0x3B: Op0x3B(); break;
    case 0x3C: Op0x3C(code); break;
    case 0x3D: Op0x3D(); break;
    case 0x3E: Op0x3E(code); break;
    case 0x3F: Op0x3F(); break;
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
    case 0x9F: Op0x9F(); break;
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
    case 0xB1: Op0xB1(); break;
    case 0xB2: Op0xB2(); break;
    case 0xB3: Op0xB3(); break;
    case 0xB4: Op0xB4(); break;
    case 0xB5: Op0xB5(); break;
    case 0xB6: Op0xB6(); break;
    case 0xB7: Op0xB7(); break;
    case 0xB8: Op0xB8(); break;
    case 0xB9: Op0xB9(); break;
    case 0xBA: Op0xBA(); break;
    case 0xBB: Op0xBB(); break;
    case 0xBC: Op0xBC(); break;
    case 0xBD: Op0xBD(); break;
    case 0xBE: Op0xBE(); break;
    case 0xBF: Op0xBF(); break;
    case 0xC0: Op0xC0(); break;
    case 0xC1: Op0xC1(); break;
    case 0xC2: Op0xC2(code); break;
    case 0xC3: Op0xC3(code); break;
    case 0xC4: Op0xC4(code); break;
    case 0xC5: Op0xC5(); break;
    case 0xC6: Op0xC6(code); break;
    case 0xC7: Op0xC7(); break;
    case 0xC8: Op0xC8(); break;
    case 0xC9: Op0xC9(); break;
    case 0xCA: Op0xCA(code); break;
    case 0xCC: Op0xCC(code); break;
    case 0xCD: Op0xCD(code); break;
    case 0xCE: Op0xCE(code); break;
    case 0xCF: Op0xCF(); break;
    case 0xD0: Op0xD0(); break;
    case 0xD1: Op0xD1(); break;
    case 0xD2: Op0xD2(code); break;
    case 0xD3: Op0xD3(code); break;
    case 0xD4: Op0xD4(code); break;
    case 0xD5: Op0xD5(); break;
    case 0xD6: Op0xD6(code); break;
    case 0xD7: Op0xD7(); break;
    case 0xD8: Op0xD8(); break;
    case 0xDA: Op0xDA(code); break;
    case 0xDB: Op0xDB(code); break;
    case 0xDC: Op0xDC(code); break;
    case 0xDE: Op0xDE(code); break;
    case 0xDF: Op0xDF(); break;
    case 0xE0: Op0xE0(); break;
    case 0xE1: Op0xE1(); break;
    case 0xE2: Op0xE2(code); break;
    case 0xE3: Op0xE3(); break;
    case 0xE4: Op0xE4(code); break;
    case 0xE5: Op0xE5(); break;
    case 0xE6: Op0xE6(code); break;
    case 0xE7: Op0xE7(); break;
    case 0xE8: Op0xE8(); break;
    case 0xE9: Op0xE9(); break;
    case 0xEA: Op0xEA(code); break;
    case 0xEB: Op0xEB(); break;
    case 0xEC: Op0xEC(code); break;
    case 0xEE: Op0xEE(code); break;
    case 0xEF: Op0xEF(); break;
    case 0xF0: Op0xF0(); break;
    case 0xF1: Op0xF1(); break;
    case 0xF2: Op0xF2(code); break;
    case 0xF3: Op0xF3(); break;
    case 0xF4: Op0xF4(code); break;
    case 0xF5: Op0xF5(); break;
    case 0xF6: Op0xF6(code); break;
    case 0xF7: Op0xF7(); break;
    case 0xF8: Op0xF8(); break;
    case 0xF9: Op0xF9(); break;
    case 0xFA: Op0xFA(code); break;
    case 0xFB: Op0xFB(); break;
    case 0xFC: Op0xFC(code); break;
    case 0xFE: Op0xFE(code); break;
    case 0xFF: Op0xFF(); break;
    default: UnimplementedOp(code); break;
  }

  // printf("CY: %d, P: %d, S: %d, Z: %d\n", registers.a, conditionCodes.cy, conditionCodes.p, conditionCodes.s, conditionCodes.z);
  return cycles8080[code[0]];
}

void Emulator8080::Op0x01(uint8_t* code)
{
  registers.b = code[2];
  registers.c = code[1];
  pc += 2;
}

void Emulator8080::Op0x02()
{
  uint16_t address = (registers.b << 8) | registers.c;
  WriteMem(address, registers.a);
}

void Emulator8080::Op0x03()
{
  registers.c++;
  if (registers.c == 0)
  {
    registers.b++;
  }
}

void Emulator8080::Op0x04()
{
  registers.b += 1;
  FlagsZSP(registers.b);
}

void Emulator8080::Op0x05()
{
  registers.b -= 1;
  FlagsZSP(registers.b);
}


void Emulator8080::Op0x06(uint8_t* code)
{
  registers.b = code[1];
  pc++;
}

void Emulator8080::Op0x07()
{
  uint8_t res = registers.a;
  registers.a = ((res & 0x80) >> 7) | (res << 1);
  conditionCodes.cy = (0x80 == (res & 0x80));
}

void Emulator8080::Op0x09()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t bc = (registers.b <<8) | registers.c;
  uint32_t res = hl + bc;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
}


void Emulator8080::Op0x0A()
{
  uint16_t address = (registers.b << 8) | registers.c;
  registers.a = memory[address];
}

void Emulator8080::Op0x0B()
{
  registers.c--;
  if (registers.c == 0xFF)
  {
    registers.b--;
  }
}

void Emulator8080::Op0x0C()
{
  registers.c += 1;
  FlagsZSP(registers.c);
}

void Emulator8080::Op0x0D()
{
  registers.c -= 1;
  FlagsZSP(registers.c);
}

void Emulator8080::Op0x0E(uint8_t* code)
{
  registers.c = code[1];
  pc++;
}

void Emulator8080::Op0x0F()
{
  uint8_t res = registers.a;
  registers.a = ((res & 1) << 7) | (res >> 1);
  conditionCodes.cy = (1 == (res & 1));
}

void Emulator8080::Op0x11(uint8_t* code)
{
  registers.d = code[2];
  registers.e = code[1];
  pc += 2;
}

void Emulator8080::Op0x12()
{
  uint16_t address = (registers.d << 8) | registers.e;
  WriteMem(address, registers.a);
}

void Emulator8080::Op0x13()
{
  registers.e++;
  if (registers.e == 0)
  {
    registers.d++;
  }
}

void Emulator8080::Op0x14()
{
  registers.d += 1;
  FlagsZSP(registers.d);
}

void Emulator8080::Op0x15()
{
  registers.d -= 1;
  FlagsZSP(registers.d);
}

void Emulator8080::Op0x16(uint8_t* code)
{
  registers.d = code[1];
  pc++;
}

void Emulator8080::Op0x17()
{
  uint8_t res = registers.a;
  registers.a = conditionCodes.cy | (res << 1);
  conditionCodes.cy = (0x80 == (res & 0x80));
}


void Emulator8080::Op0x19()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t de = (registers.d <<8) | registers.e;
  uint32_t res = hl + de;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
}

void Emulator8080::Op0x1A()
{
  uint16_t address = (registers.d << 8) | registers.e;
  registers.a = memory[address];
}

void Emulator8080::Op0x1B()
{
  registers.e--;
  if (registers.e == 0xFF){
    registers.d--;
  }
}

void Emulator8080::Op0x1C()
{
  registers.e += 1;
  FlagsZSP(registers.e);
}

void Emulator8080::Op0x1D()
{
  registers.e -= 1;
  FlagsZSP(registers.e);
}

void Emulator8080::Op0x1E(uint8_t* code)
{
  registers.e = code[1];
  pc++;
}

void Emulator8080::Op0x1F()
{
  uint8_t res = registers.a;
  registers.a = (conditionCodes.cy << 7) | (res >> 1);
  conditionCodes.cy = (1 == (res & 1));
}

void Emulator8080::Op0x21(uint8_t* code)
{
  registers.h = code[2];
  registers.l = code[1];
  pc += 2;
}

void Emulator8080::Op0x22(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  WriteMem(address + 1, registers.h);
  WriteMem(address, registers.l);
  pc += 2;
}

void Emulator8080::Op0x23()
{
  registers.l++;
  if (registers.l == 0)
  {
    registers.h++;
  }
}

void Emulator8080::Op0x24()
{
  registers.h += 1;
  FlagsZSP(registers.h);
}

void Emulator8080::Op0x25()
{
  registers.h -= 1;
  FlagsZSP(registers.h);
}

void Emulator8080::Op0x26(uint8_t* code)
{
  registers.h = code[1];
  pc++;
}

void Emulator8080::Op0x27()
{
  if ((registers.a & 0xF) > 9)
  {
    registers.a += 6;
  }

  if ((registers.a & 0xF0) > 0x90)
  {
    uint16_t res = (uint16_t) registers.a + 0x60;
    registers.a = res & 0xFF;
    ArithFlagsA(res);
  }
}

void Emulator8080::Op0x29()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t res = hl + hl;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
}

void Emulator8080::Op0x2A(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  registers.h = memory[address + 1];
  registers.l = memory[address];
  pc += 2;
}

void Emulator8080::Op0x2B()
{
  registers.l--;
  if (registers.l == 0xFF)
  {
    registers.h--;
  }
}

void Emulator8080::Op0x2C()
{
  registers.l += 1;
  FlagsZSP(registers.l);
}

void Emulator8080::Op0x2D()
{
  registers.l -= 1;
  FlagsZSP(registers.l);
}

void Emulator8080::Op0x2E(uint8_t* code)
{
  registers.l = code[1];
  pc++;
}

void Emulator8080::Op0x2F()
{
  registers.a = ~registers.a;
}

void Emulator8080::Op0x31(uint8_t* code)
{
  sp = (code[2] << 8) | code[1];
  pc += 2;
}

void Emulator8080::Op0x32(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  WriteMem(address, registers.a);
  pc += 2;
}

void Emulator8080::Op0x33()
{
  sp++;
}

void Emulator8080::Op0x34()
{
  uint8_t res = ReadFromHL() + 1;
  FlagsZSP(res);
  WriteToHL(res);
}

void Emulator8080::Op0x35()
{
  uint8_t res = ReadFromHL() - 1;
  FlagsZSP(res);
  WriteToHL(res);
}

void Emulator8080::Op0x36(uint8_t* code)
{
  WriteToHL(code[1]);
  pc++;
}

void Emulator8080::Op0x37()
{
  conditionCodes.cy = 1;
}

void Emulator8080::Op0x39()
{
  uint32_t hl = (registers.h <<8) | registers.l;
  uint32_t res = hl + sp;
  registers.h = (res & 0xFF00) >> 8;
  registers.l = res & 0xFF;
  conditionCodes.cy = ((res & 0xFFFF0000) != 0);
}

void Emulator8080::Op0x3A(uint8_t* code)
{
  uint16_t address = (code[2] << 8) | code[1];
  registers.a = memory[address];
  pc += 2;
}

void Emulator8080::Op0x3B()
{
  sp--;
}

void Emulator8080::Op0x3C(uint8_t* code)
{
  registers.a += 1;
  FlagsZSP(registers.a);
}

void Emulator8080::Op0x3D()
{
  registers.a -= 1;
  FlagsZSP(registers.a);
}

void Emulator8080::Op0x3E(uint8_t* code)
{
  registers.a = code[1];
  pc++;
}

void Emulator8080::Op0x3F()
{
  conditionCodes.cy = 0;
}

void Emulator8080::Op0x40()
{
  registers.b = registers.b;
}

void Emulator8080::Op0x41()
{
  registers.b = registers.c;
}

void Emulator8080::Op0x42()
{
  registers.b = registers.d;
}

void Emulator8080::Op0x43()
{
  registers.b = registers.e;
}

void Emulator8080::Op0x44()
{
  registers.b = registers.h;
}

void Emulator8080::Op0x45()
{
  registers.b = registers.l;
}

void Emulator8080::Op0x46()
{
  registers.b = ReadFromHL();
}

void Emulator8080::Op0x47()
{
  registers.b = registers.a;
}

void Emulator8080::Op0x48()
{
  registers.c = registers.b;
}

void Emulator8080::Op0x49()
{
  registers.c = registers.c;
}

void Emulator8080::Op0x4A()
{
  registers.c = registers.d;
}

void Emulator8080::Op0x4B()
{
  registers.c = registers.e;
}

void Emulator8080::Op0x4C()
{
  registers.c = registers.h;
}

void Emulator8080::Op0x4D()
{
  registers.c = registers.l;
}

void Emulator8080::Op0x4E()
{
  registers.c = ReadFromHL();
}

void Emulator8080::Op0x4F()
{
  registers.c = registers.a;
}

void Emulator8080::Op0x50()
{
  registers.d = registers.b;
}

void Emulator8080::Op0x51()
{
  registers.d = registers.c;
}

void Emulator8080::Op0x52()
{
  registers.d = registers.d;
}

void Emulator8080::Op0x53()
{
  registers.d = registers.e;
}

void Emulator8080::Op0x54()
{
  registers.d = registers.h;
}

void Emulator8080::Op0x55()
{
  registers.d = registers.l;
}

void Emulator8080::Op0x56()
{
  registers.d = ReadFromHL();
}

void Emulator8080::Op0x57()
{
  registers.d = registers.a;
}

void Emulator8080::Op0x58()
{
  registers.e = registers.b;
}

void Emulator8080::Op0x59()
{
  registers.e = registers.c;
}

void Emulator8080::Op0x5A()
{
  registers.e = registers.d;
}

void Emulator8080::Op0x5B()
{
  registers.e = registers.e;
}

void Emulator8080::Op0x5C()
{
  registers.e = registers.h;
}

void Emulator8080::Op0x5D()
{
  registers.e = registers.l;
}

void Emulator8080::Op0x5E()
{
  registers.e = ReadFromHL();
}

void Emulator8080::Op0x5F()
{
  registers.e = registers.a;
}

void Emulator8080::Op0x60()
{
  registers.h = registers.b;
}

void Emulator8080::Op0x61()
{
  registers.h = registers.c;
}

void Emulator8080::Op0x62()
{
  registers.h = registers.d;
}

void Emulator8080::Op0x63()
{
  registers.h = registers.e;
}

void Emulator8080::Op0x64()
{
  registers.h = registers.h;
}

void Emulator8080::Op0x65()
{
  registers.h = registers.l;
}

void Emulator8080::Op0x66()
{
  registers.h = ReadFromHL();
}

void Emulator8080::Op0x67()
{
  registers.h = registers.a;
}

void Emulator8080::Op0x68()
{
  registers.l = registers.b;
}

void Emulator8080::Op0x69()
{
  registers.l = registers.c;
}

void Emulator8080::Op0x6A()
{
  registers.l = registers.d;
}

void Emulator8080::Op0x6B()
{
  registers.l = registers.e;
}

void Emulator8080::Op0x6C()
{
  registers.l = registers.h;
}

void Emulator8080::Op0x6D()
{
  registers.l = registers.l;
}

void Emulator8080::Op0x6E()
{
  registers.l = ReadFromHL();
}

void Emulator8080::Op0x6F()
{
  registers.l = registers.a;
}

void Emulator8080::Op0x70()
{
  WriteToHL(registers.b);
}

void Emulator8080::Op0x71()
{
  WriteToHL(registers.c);
}

void Emulator8080::Op0x72()
{
  WriteToHL(registers.d);
}

void Emulator8080::Op0x73()
{
  WriteToHL(registers.e);
}

void Emulator8080::Op0x74()
{
  WriteToHL(registers.h);
}

void Emulator8080::Op0x75()
{
  WriteToHL(registers.l);
}

void Emulator8080::Op0x76()
{
  halt = true;
}

void Emulator8080::Op0x77()
{
  WriteToHL(registers.a);
}

void Emulator8080::Op0x78()
{
  registers.a = registers.b;
}

void Emulator8080::Op0x79()
{
  registers.a = registers.c;
}

void Emulator8080::Op0x7A()
{
  registers.a = registers.d;
}

void Emulator8080::Op0x7B()
{
  registers.a = registers.e;
}

void Emulator8080::Op0x7C()
{
  registers.a = registers.h;
}

void Emulator8080::Op0x7D()
{
  registers.a = registers.l;
}

void Emulator8080::Op0x7E()
{
  registers.a = ReadFromHL();
}

void Emulator8080::Op0x7F()
{
  registers.a = registers.a;
}

void Emulator8080::Op0x80()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.b;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x81()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.c;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x82()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.d;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x83()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.e;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x84()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.h;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x85()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.l;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x86()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) ReadFromHL();
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x87()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.a;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x88()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.b + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x89()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.c + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x8A()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.d + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x8B()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.e + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x8C()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.h + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x8D()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.l + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x8E()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) ReadFromHL() + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x8F()
{
  uint16_t res = (uint16_t)registers.a + (uint16_t) registers.a + conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x90()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.b;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x91()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.c;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x92()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.d;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x93()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.e;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x94()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.h;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x95()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.l;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x96()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) ReadFromHL();
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x97()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.a;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x98()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.b - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x99()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.c - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x9A()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.d - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x9B()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.e - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x9C()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.h - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x9D()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.l - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x9E()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) ReadFromHL() - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0x9F()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.a - conditionCodes.cy;
  ArithFlagsA(res);
  registers.a = (res & 0xFF);
}

void Emulator8080::Op0xA0()
{
  registers.a = registers.a & registers.b;
  LogicFlagsA();
}

void Emulator8080::Op0xA1()
{
  registers.a = registers.a & registers.c;
  LogicFlagsA();
}

void Emulator8080::Op0xA2()
{
  registers.a = registers.a & registers.d;
  LogicFlagsA();
}

void Emulator8080::Op0xA3()
{
  registers.a = registers.a & registers.e;
  LogicFlagsA();
}

void Emulator8080::Op0xA4()
{
  registers.a = registers.a & registers.h;
  LogicFlagsA();
}

void Emulator8080::Op0xA5()
{
  registers.a = registers.a & registers.l;
  LogicFlagsA();
}


void Emulator8080::Op0xA6()
{
  registers.a = registers.a & ReadFromHL();
  LogicFlagsA();
}

void Emulator8080::Op0xA7()
{
  registers.a = registers.a & registers.a;
  LogicFlagsA();
}

void Emulator8080::Op0xA8()
{
  registers.a = registers.a ^ registers.b;
  LogicFlagsA();
}

void Emulator8080::Op0xA9()
{
  registers.a = registers.a ^ registers.c;
  LogicFlagsA();
}

void Emulator8080::Op0xAA()
{
  registers.a = registers.a ^ registers.d;
  LogicFlagsA();
}

void Emulator8080::Op0xAB()
{
  registers.a = registers.a ^ registers.e;
  LogicFlagsA();
}

void Emulator8080::Op0xAC()
{
  registers.a = registers.a ^ registers.h;
  LogicFlagsA();
}

void Emulator8080::Op0xAD()
{
  registers.a = registers.a ^ registers.l;
  LogicFlagsA();
}


void Emulator8080::Op0xAE()
{
  registers.a = registers.a ^ ReadFromHL();
  LogicFlagsA();
}

void Emulator8080::Op0xAF()
{
  registers.a = registers.a ^ registers.a;
  LogicFlagsA();
}

void Emulator8080::Op0xB0()
{
  registers.a = registers.a | registers.b;
  LogicFlagsA();
}

void Emulator8080::Op0xB1()
{
  registers.a = registers.a | registers.c;
  LogicFlagsA();
}

void Emulator8080::Op0xB2()
{
  registers.a = registers.a | registers.d;
  LogicFlagsA();
}

void Emulator8080::Op0xB3()
{
  registers.a = registers.a | registers.e;
  LogicFlagsA();
}

void Emulator8080::Op0xB4()
{
  registers.a = registers.a | registers.h;
  LogicFlagsA();
}

void Emulator8080::Op0xB5()
{
  registers.a = registers.a | registers.l;
  LogicFlagsA();
}


void Emulator8080::Op0xB6()
{
  registers.a = registers.a | ReadFromHL();
  LogicFlagsA();
}

void Emulator8080::Op0xB7()
{
  registers.a = registers.a | registers.a;
  LogicFlagsA();
}

void Emulator8080::Op0xB8()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.b;
  ArithFlagsA(res);
}

void Emulator8080::Op0xB9()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.c;
  ArithFlagsA(res);
}

void Emulator8080::Op0xBA()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.d;
  ArithFlagsA(res);
}

void Emulator8080::Op0xBB()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.e;
  ArithFlagsA(res);
}

void Emulator8080::Op0xBC()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.h;
  ArithFlagsA(res);
}

void Emulator8080::Op0xBD()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) registers.l;
  ArithFlagsA(res);
}

void Emulator8080::Op0xBE()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t) ReadFromHL();
  ArithFlagsA(res);
}

void Emulator8080::Op0xBF()
{
  uint16_t res = (uint16_t)registers.a - (uint16_t)registers.a;
  ArithFlagsA(res);
}

void Emulator8080::Op0xC0()
{
  if (conditionCodes.z == 0)
  {
    pc = (memory[sp + 1] << 8) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xC1()
{
  Pop(&registers.b, &registers.c);
}

void Emulator8080::Op0xC2(uint8_t* code)
{
  if (conditionCodes.z == 0)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xC3(uint8_t* code)
{
  pc = (code[2] << 8) | code[1];
}

void Emulator8080::Op0xC4(uint8_t* code)
{
  if (conditionCodes.z == 0)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, (retAddress & 0xFF));
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xC5()
{
  Push(registers.b, registers.c);
}

void Emulator8080::Op0xC6(uint8_t* code)
{
  uint16_t res = (uint16_t)registers.a + (uint16_t)code[1];
  FlagsZSP(res & 0xFF);
  conditionCodes.cy = (res > 0xFF);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0xC7()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0000;
}

void Emulator8080::Op0xC8()
{
  if (conditionCodes.z == 1)
  {
    pc = (memory[sp+ 1] << 8) | memory[sp];
    sp += 2;
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
    pc += 2;
  }
}

void Emulator8080::Op0xCC(uint8_t* code)
{
  if (conditionCodes.z == 1)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xCD(uint8_t* code)
{
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
}

void Emulator8080::Op0xCE(uint8_t* code)
{
  uint16_t res = (uint16_t)registers.a + (uint16_t)code[1] + conditionCodes.cy;
  FlagsZSP(res & 0xFF);
  conditionCodes.cy = (res > 0xFF);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0xCF()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0008;
}

void Emulator8080::Op0xD0()
{
  if (conditionCodes.cy == 0)
  {
    pc = (memory[sp + 1] << 8) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xD1()
{
  Pop(&registers.d, &registers.e);
}

void Emulator8080::Op0xD2(uint8_t* code)
{
  if (conditionCodes.cy == 0)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}


void Emulator8080::Op0xD3(uint8_t* code)
{
  //TODO: Implement OUT
  pc++;
}

void Emulator8080::Op0xD4(uint8_t* code)
{
  if (conditionCodes.cy == 0)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xD5()
{
  Push(registers.d, registers.e);
}

void Emulator8080::Op0xD6(uint8_t* code)
{
  uint16_t res = (uint16_t)registers.a - (uint16_t)code[1];
  FlagsZSP(res & 0xFF);
  conditionCodes.cy = (registers.a < code[1]);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0xD7()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0010;
}

void Emulator8080::Op0xD8()
{
  if (conditionCodes.cy)
  {
    pc = (memory[sp + 1] << 8) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xDA(uint8_t* code)
{
  if (conditionCodes.cy)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xDB(uint8_t* code)
{
  //TODO: Implement IN
  pc++;
}

void Emulator8080::Op0xDC(uint8_t* code)
{
  if (conditionCodes.cy)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xDE(uint8_t* code)
{
  uint16_t res = (uint16_t)registers.a - (uint16_t)code[1] - conditionCodes.cy;
  FlagsZSP(res & 0xFF);
  conditionCodes.cy = (res > 0xFF);
  registers.a = (res & 0xFF);
  pc++;
}

void Emulator8080::Op0xDF()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0018;
}

void Emulator8080::Op0xE0()
{
  if (conditionCodes.p == 0)
  {
    pc = (memory[sp + 1] << 8) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xE1()
{
  Pop(&registers.h, &registers.l);
}

void Emulator8080::Op0xE2(uint8_t* code)
{
  if (conditionCodes.p == 0)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xE3()
{

  uint8_t high = registers.h;
  uint8_t low = registers.l;
  registers.h = memory[sp + 1];
  registers.l = memory[sp];
  WriteMem(sp + 1, high);
  WriteMem(sp, low);
}

void Emulator8080::Op0xE4(uint8_t* code)
{
  if (conditionCodes.p == 0)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xE5()
{
  Push(registers.h, registers.l);
}

void Emulator8080::Op0xE6(uint8_t* code)
{
  registers.a =  registers.a & code[1];
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xE7()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0020;
}

void Emulator8080::Op0xE8()
{
  if (conditionCodes.p == 1)
  {
    pc = (memory[sp + 1] << 8) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xE9()
{
  pc = (registers.h << 8) | registers.l;
}

void Emulator8080::Op0xEA(uint8_t* code)
{
  if (conditionCodes.p == 1)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xEB()
{

  uint8_t save1 = registers.d;
  uint8_t save2 = registers.e;
  registers.d = registers.h;
  registers.e = registers.l;
  registers.h = save1;
  registers.l = save2;
}

void Emulator8080::Op0xEC(uint8_t* code)
{
  if (conditionCodes.p == 1)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xEE(uint8_t* code)
{
  registers.a =  registers.a ^ code[1];
  LogicFlagsA();
  pc++;
}

void Emulator8080::Op0xEF()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0028;
}

void Emulator8080::Op0xF0()
{
  if (conditionCodes.s == 0)
  {
    pc = (memory[sp + 1] << 8 ) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xF1()
{
  registers.a = memory[sp + 1];
  uint8_t psw = memory[sp];
  conditionCodes.s = (0x80 == (psw & 0x80));
  conditionCodes.z = (0x40 == (psw & 0x60));
  conditionCodes.ac = (0x10 == (psw & 0x10));
  conditionCodes.p = (0x02 == (psw & 0x02));
  conditionCodes.cy = (0x01 == (psw & 0x01));
  sp += 2;
}

void Emulator8080::Op0xF2(uint8_t* code)
{
  if (conditionCodes.s == 0)
  {
    pc = (code[2] << 8 ) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xF3()
{
  interuptEnabled = false;
}

void Emulator8080::Op0xF4(uint8_t* code)
{
  if (conditionCodes.s == 0)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xF5()
{
  uint8_t psw = (
    conditionCodes.s << 7 |
    conditionCodes.z << 6 |
    conditionCodes.ac << 4 |
    conditionCodes.p << 2 |
    1 << 1 |
    conditionCodes.cy
  );
  WriteMem(sp - 1, registers.a);
  WriteMem(sp - 2, psw);
  sp -= 2;
}

void Emulator8080::Op0xF6(uint8_t* code)
{
  uint8_t res = registers.a | code[1];
  FlagsZSP(res);
  conditionCodes.cy = 0;
  registers.a = res;
  pc++;
}

void Emulator8080::Op0xF7()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0030;
}


void Emulator8080::Op0xF8()
{
  if (conditionCodes.s == 1)
  {
    pc = (memory[sp + 1] << 8 ) | memory[sp];
    sp += 2;
  }
}

void Emulator8080::Op0xF9()
{
  sp = (registers.h << 8) | registers.l;
}

void Emulator8080::Op0xFA(uint8_t* code)
{
  if (conditionCodes.s)
  {
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xFB()
{
  interuptEnabled = true;
}

void Emulator8080::Op0xFC(uint8_t* code)
{
  if (conditionCodes.s)
  {
    uint16_t retAddress = pc + 2;
    WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
    WriteMem(sp - 2, retAddress & 0xFF);
    sp -= 2;
    pc = (code[2] << 8) | code[1];
  }
  else
  {
    pc += 2;
  }
}

void Emulator8080::Op0xFE(uint8_t* code)
{
  uint8_t res = registers.a - code[1];
  FlagsZSP(res);
  conditionCodes.cy = (registers.a < code[1]);
  pc++;
}

void Emulator8080::Op0xFF()
{
  uint16_t retAddress = pc + 2;
  WriteMem(sp - 1, (retAddress >> 8) & 0xFF);
  WriteMem(sp - 2, retAddress & 0xFF);
  sp -= 2;
  pc = 0x0038;
}

void Emulator8080::UnimplementedOp(uint8_t* code)
{
  pc--;
  printf("%04x %02x ", pc, code[0]);
  printf("Unimplemented Op. Stopping Program\n");
  halt = true;
}

void Emulator8080::GenerateInterupt(int interuptNum)
{
  Push((pc & 0xFF00) >> 8, pc & 0xFF);
  pc = 0x08 * interuptNum;

  interuptEnabled = false;
}