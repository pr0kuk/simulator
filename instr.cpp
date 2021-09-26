#include "instr.h"
#include "cpu.h"
#include <sstream>
#include <QDebug>

std::map<std::string, uint32_t> Instr::m_instrsDict;

void Instr::clear() {
    m_opcode = 0;
    m_r1 = 0;
    m_r2 = 0;
    m_r3_imm = 0;
}

void Instr::executeCode(CPU *cpu, uint32_t code) {
    decode(code);
    execute(cpu);
}

void Instr::execute(CPU *cpu) {
    qDebug() << "[EXEC] " << QString::fromStdString(disasm());
    switch (m_opcode) {
    default:
        qDebug() << "[Error] Wrong opcode";
        break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs)\
    case _opcode: { _execute } break;
#include "ISA.h"
#undef _ISA
    }
}

void Instr::assembler(std::stringstream &input) {
    clear();
    std::string name;
    input >> name;
    m_opcode = m_instrsDict[name];
    std::string arg;
    switch (m_opcode) {
    default:
        // Add labels
        qDebug() << "[Error] Wrong opcode";
        break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs)\
    case _opcode: { _asmargs } break;
#include "ISA.h"
#undef _ISA
    }
}

std::string Instr::disasm() {
    std::stringstream args;
    switch (m_opcode) {
    default:
        qDebug() << "[Error] Wrong opcode";
        break;
#define _ISA(_opcode, _name, _execute, _asmargs, _disasmargs)\
    case _opcode: { args << #_name; _disasmargs } break;
#include "ISA.h"
#undef _ISA
    }
    return args.str();
}

uint32_t Instr::code() {
    return (m_opcode << 24) | (m_r1 << 20) | (m_r2 << 16) | m_r3_imm;
}

void Instr::decode(uint32_t code) {
    m_opcode = code >> 24;
    m_r1 = (code >> 20) & 0xF;
    m_r2 = (code >> 16) & 0xF;
    m_r3_imm = code & 0xFF;
}
