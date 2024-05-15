#include <stdint.h>

struct Registers
{
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
};

struct ConditionCodes
{
  uint8_t z;
  uint8_t s;
  uint8_t p;
  uint8_t cy;
  uint8_t ac;
};

class Emulator8080
{
  public:
    bool halt;
    bool interuptEnabled;
    uint8_t* display{};

    Emulator8080();
    bool LoadRom(char* rom);
    void Cycle();
    void GenerateInterupt(int interuptNum);

  private:
    struct Registers registers;
    struct ConditionCodes conditionCodes;
    uint16_t sp;
    uint16_t pc;
    uint8_t* memory;

    int Parity(int x, int size);

    void Op0x00();
    void Op0x01(uint8_t* code);
    void Op0x02();
    void Op0x03();
    void Op0x04();
    void Op0x05();
    void Op0x06(uint8_t* code);
    void Op0x07();
    void Op0x08();
    void Op0x09();
    void Op0x0A();
    void Op0x0B();
    void Op0x0C();
    void Op0x0D();
    void Op0x0E(uint8_t* code);
    void Op0x0F();
    void Op0x10();
    void Op0x11(uint8_t* code);
    void Op0x12();
    void Op0x13();
    void Op0x14();
    void Op0x15();
    void Op0x16();
    void Op0x17();
    void Op0x18();
    void Op0x19();
    void Op0x1A();
    void Op0x1B();
    void Op0x1C();
    void Op0x1D();
    void Op0x1E();
    void Op0x1F();
    void Op0x20();
    void Op0x21(uint8_t* code);
    void Op0x22();
    void Op0x23();
    void Op0x24();
    void Op0x25();
    void Op0x26(uint8_t* code);
    void Op0x27();
    void Op0x28();
    void Op0x29();
    void Op0x2A();
    void Op0x2B();
    void Op0x2C();
    void Op0x2D();
    void Op0x2E();
    void Op0x2F();
    void Op0x30();
    void Op0x31(uint8_t* code);
    void Op0x32(uint8_t* code);
    void Op0x33();
    void Op0x34();
    void Op0x35();
    void Op0x36(uint8_t* code);
    void Op0x37();
    void Op0x38();
    void Op0x39();
    void Op0x3A(uint8_t* code);
    void Op0x3B();
    void Op0x3C();
    void Op0x3D();
    void Op0x3E(uint8_t* code);
    void Op0x3F();
    void Op0x40();
    void Op0x41();
    void Op0x42();
    void Op0x43();
    void Op0x44();
    void Op0x45();
    void Op0x46();
    void Op0x47();
    void Op0x48();
    void Op0x49();
    void Op0x4A();
    void Op0x4B();
    void Op0x4C();
    void Op0x4D();
    void Op0x4E();
    void Op0x4F();
    void Op0x50();
    void Op0x51();
    void Op0x52();
    void Op0x53();
    void Op0x54();
    void Op0x55();
    void Op0x56();
    void Op0x57();
    void Op0x58();
    void Op0x59();
    void Op0x5A();
    void Op0x5B();
    void Op0x5C();
    void Op0x5D();
    void Op0x5E();
    void Op0x5F();
    void Op0x60();
    void Op0x61();
    void Op0x62();
    void Op0x63();
    void Op0x64();
    void Op0x65();
    void Op0x66();
    void Op0x67();
    void Op0x68();
    void Op0x69();
    void Op0x6A();
    void Op0x6B();
    void Op0x6C();
    void Op0x6D();
    void Op0x6E();
    void Op0x6F();
    void Op0x70();
    void Op0x71();
    void Op0x72();
    void Op0x73();
    void Op0x74();
    void Op0x75();
    void Op0x76();
    void Op0x77();
    void Op0x78();
    void Op0x79();
    void Op0x7A();
    void Op0x7B();
    void Op0x7C();
    void Op0x7D();
    void Op0x7E();
    void Op0x7F();
    void Op0x80();
    void Op0x81();
    void Op0x82();
    void Op0x83();
    void Op0x84();
    void Op0x85();
    void Op0x86();
    void Op0x87();
    void Op0x88();
    void Op0x89();
    void Op0x8A();
    void Op0x8B();
    void Op0x8C();
    void Op0x8D();
    void Op0x8E();
    void Op0x8F();
    void Op0x90();
    void Op0x92();
    void Op0x93();
    void Op0x94();
    void Op0x95();
    void Op0x96();
    void Op0x97();
    void Op0x98();
    void Op0x99();
    void Op0x9A();
    void Op0x9B();
    void Op0x9C();
    void Op0x9D();
    void Op0x9E();
    void Op0x9F();
    void Op0xA0();
    void Op0xA1();
    void Op0xA2();
    void Op0xA3();
    void Op0xA4();
    void Op0xA5();
    void Op0xA6();
    void Op0xA7();
    void Op0xA8();
    void Op0xA9();
    void Op0xAA();
    void Op0xAB();
    void Op0xAC();
    void Op0xAD();
    void Op0xAE();
    void Op0xAF();
    void Op0xB0();
    void Op0xB1();
    void Op0xB2();
    void Op0xB3();
    void Op0xB4();
    void Op0xB5();
    void Op0xB6();
    void Op0xB7();
    void Op0xB8();
    void Op0xB9();
    void Op0xBA();
    void Op0xBB();
    void Op0xBC();
    void Op0xBD();
    void Op0xBE();
    void Op0xBF();
    void Op0xC0();
    void Op0xC1();
    void Op0xC2(uint8_t* code);
    void Op0xC3(uint8_t* code);
    void Op0xC4();
    void Op0xC5();
    void Op0xC6(uint8_t* code);
    void Op0xC7();
    void Op0xC8();
    void Op0xC9();
    void Op0xCA(uint8_t* code);
    void Op0xCB();
    void Op0xCC();
    void Op0xCD(uint8_t* code);
    void Op0xCE();
    void Op0xCF();
    void Op0xD0();
    void Op0xD1();
    void Op0xD2(uint8_t* code);
    void Op0xD3(uint8_t* code);
    void Op0xD4();
    void Op0xD5();
    void Op0xD6();
    void Op0xD7();
    void Op0xD8();
    void Op0xD9();
    void Op0xDA(uint8_t* code);
    void Op0xDB(uint8_t* code);
    void Op0xDC();
    void Op0xDD();
    void Op0xDE();
    void Op0xDF();
    void Op0xE0();
    void Op0xE1();
    void Op0xE2();
    void Op0xE3();
    void Op0xE4();
    void Op0xE5();
    void Op0xE6(uint8_t* code);
    void Op0xE7();
    void Op0xE8();
    void Op0xE9();
    void Op0xEA();
    void Op0xEB();
    void Op0xEC();
    void Op0xED();
    void Op0xEE();
    void Op0xEF();
    void Op0xF0();
    void Op0xF1();
    void Op0xF2();
    void Op0xF3();
    void Op0xF4();
    void Op0xF5();
    void Op0xF6();
    void Op0xF7();
    void Op0xF8();
    void Op0xF9();
    void Op0xFA();
    void Op0xFB();
    void Op0xFC();
    void Op0xFD();
    void Op0xFE(uint8_t* code);
    void Op0xFF();
    void UnimplementedOp(uint8_t* code);
};