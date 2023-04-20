#include "instr.h"
#include "cpu.h"
#include <QRandomGenerator>
#include <sstream>

std::map<std::string, uint32_t> Instr::m_instrsDict;

void Instr::clear() {
  m_opcode = 0;
  m_r1 = 0;
  m_r2 = 0;
  m_f = 0;
  m_r3_imm = 0;
}

void Instr::executeCode(CPU *cpu, uint32_t code) {
  decode(code);
  execute(cpu);
}

void Instr::execute(CPU *cpu) {
  switch (m_opcode) {
  default:
    cpu->stop();
    cpu->showMsg("[Error] Wrong opcode on execution");
    break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs, _dumpregs)       \
  case _opcode: {                                                              \
    _execute                                                                   \
  } break;
#include "ISA.h"
#undef _ISA
  }
}

std::string Instr::assembler(std::stringstream &input) {
  clear();
  std::string name;
  input >> name;
  m_opcode = m_instrsDict[name];
  std::string arg;
  switch (m_opcode) {
  default:
    // Return label
    return name;
    break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs, _dumpregs)       \
  case _opcode: {                                                              \
    _asmargs                                                                   \
  } break;
#include "ISA.h"
#undef _ISA
  }
  return std::string();
}

std::string Instr::disasm() {
  std::stringstream args;
  switch (m_opcode) {
  default:
    args << "[Opcode Error]";
    break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs, _dumpregs)       \
  case _opcode: {                                                              \
    args << #_name;                                                            \
    _disasmargs                                                                \
  } break;
#include "ISA.h"
#undef _ISA
  }
  return args.str();
}

std::string Instr::dumpRegs(CPU *cpu) {
  std::stringstream args;
  switch (m_opcode) {
  default:
    args << "[Opcode Error]";
    break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs, _dumpregs)       \
  case _opcode: {                                                              \
    _dumpregs                                                                  \
  } break;
#include "ISA.h"
#undef _ISA
  }
  return args.str();
}

//uint32_t Instr::code() {
//  return (m_opcode << 24) | (m_r1 << 20) | (m_r2 << 16) | (m_r3_imm & 0xFFFF);
//}

void Instr::decode(uint32_t code) {
  m_opcode = code >> 25;

  if (m_opcode == 0x3 || m_opcode == 0x13) {
  // [i] 0000011 | 0010011
  m_r1 = (code >> 20) & 0x1F; //rd
  m_r2 = (code >> 12) & 0x1F;
  m_f = (code >> 17) & 0x3;
  m_r3_imm = (code & 0xFFF);
  m_opcode |= m_f << 7;
  }
  if (m_opcode == 0x23) {
  // [s] // 0100011
  m_r3_imm = ((code >> 5) & 0x3F) | (((code >> 20) & 0x3F) << 6);
  m_r1 = (code >> 12) & 0x1F;
  m_r2 = (code >> 7) & 0x1F;
  m_f = (code >> 17) & 0x3;
  m_opcode |= m_f << 7;
  }
  if (m_opcode == 0x37) {
  // [u] 0110111
  m_r3_imm = (code & 0xFFFFF);
  m_r1 = (code >> 20) & 0x1F; //rd
  }
  if (m_opcode == 0x63) {
  // [b] op = 1100011
  m_r1 = (code >> 12) & 0x1F;
  m_r2 = (code >> 7) & 0x1F;
  m_f = (code >> 17) & 0x3;
  m_opcode |= m_f << 7;
  m_r3_imm = (code & 0x1) | ((code >> 1) & 0x3F << 1) | ((code>>20)&0xF << (6+1)) | (((code>>24)&0x1) << (1+6+4));
  }
  if (m_opcode == 0x6F) {
  // [J] 1101111
  m_r1 = (code >> 20) & 0x1F; //rd
  m_r3_imm = (code & 0x1) | ((code >> 1) & 0x3FF << 1) | ((code>>11)&0x1 << (10+1)) | (((code>>12)&0xFF) << (1+10+1));
  }
}