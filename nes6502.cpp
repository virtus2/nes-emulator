// 데이터시트: http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
// http://archive.6502.org/datasheets/rockwell_r65c00_microprocessors.pdf
#include "nes6502.h"
#include "Bus.h"

nes6502::nes6502()
{
	// 데이터시트의 10번째 슬라이드 참고
	// 16x16 = 256개의 명령어(행과 열에 각각 4비트 사용)
	using a = nes6502;
	lookup = 
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
}

nes6502::~nes6502()
{

}
///////////////////////////////////////////////////////////////////////////////
// BUS CONNECTIVITY
// 버스의 16비트 주소에서 데이터 8비트를 읽음
uint8_t nes6502::cpuRead(uint16_t a)
{
	// 정상 작동 시 "read only"는 false로 설정됨. 이상할 수 있지만
    // 버스상의 일부 장치들은 버스로부터 데이터를 읽을 때 상태를 변경할 수 있으며, 이는 정상적인 상황에서 의도된 것
    // 하지만 disassembler는 버스의 장치 상태를 변경하지 않고 주소의 데이터를 읽어야함
    return bus->cpuRead(a, false);
}
// 버스의 특정 주소에 byte를 씀
void nes6502::cpuWrite(uint16_t a, uint8_t d)
{
    bus->cpuWrite(a, d);
}
///////////////////////////////////////////////////////////////////////////////
// EXTERNAL INPUTS

// CPU 초기화
// 레지스터를 0으로 설정하고, 상태 레지스터를 초기화함(Unused bit는 1로 남겨둠)
// absolute address 0xFFFC는 Program Counter가 가리킬 주소를 담고 있음
// 이는 프로그래머가 실행을 시작할 메모리 내의 주소(프로그래머가 알고 있는, programmable한 주소)위치로 점프할 수 있게 해줌
// 일반적으로 프로그래머는 컴파일 타임에 0xFFFC의 값을 설정함
void nes6502::reset()
{
	// Program Counter를 설정할 주소를 가져옴
	addr_abs = 0xFFFC; // 
	uint16_t lo = cpuRead(addr_abs + 0);
	uint16_t hi = cpuRead(addr_abs + 1);
	// Set it
	pc = (hi << 8) | lo;

	// Reset internal registers
	a = 0;
	x = 0;
	y = 0;
	stkp = 0xFD;
	status = 0x00 | U;

	// Clear internal helper variables
	addr_rel = 0x0000;
	addr_abs = 0x0000;
	fetched = 0x00;

	// Reset도 마찬가지로 cycle을 소모함
	cycles = 8;
}


// Disable interrupt flag가 0일때만 작동
// IRQ는 언제든 발생할 수 있지만 현재 동작하고 있는 프로그램에 영향을 주어선 안됨
// 그러므로 현재 실행하고 있는 instruction이 끝날 수 있도록 하고(이를 위해서 cycles == 0 이 되었을때 모든 일을 처리함)
// 현재의 상태 레지스터와 Program Counter를 스택에 저장함
// Interrupt routine이 끝나면 "RTI" instruction을 통해 다시 스택에 저장되어있는 레지스터들을 복구함
// IRQ가 발생했을때 reset과 비슷하게 하드코딩된 주소 0xFFFE를 통해서 주소값을 읽어옴
// 이 주소값은 Program Counter에 저장됨
void nes6502::irq()
{
	// If interrupts are allowed
	if (GetFlag(I) == 0)
	{
		// Program Counter를 스택에 push
		// 16bit이므로 두 번 push해야함
		cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF);
		stkp--;
		cpuWrite(0x0100 + stkp, pc & 0x00FF);
		stkp--;

		// 상태 레지스터를 스택에 push
		SetFlag(B, 0); // Break
		SetFlag(U, 1); // Unused
		SetFlag(I, 1); // Disable Interrupts
		cpuWrite(0x0100 + stkp, status);
		stkp--;

		// 하드코딩된 주소에서 새로운 Program Counter 주소값을 읽어옴
		addr_abs = 0xFFFE;
		uint16_t lo = cpuRead(addr_abs + 0);
		uint16_t hi = cpuRead(addr_abs + 1);
		pc = (hi << 8) | lo;

		// IRQs take time
		cycles = 7;
	}
}


// IRQ와 똑같지만 flag와 상관없이 작동하며
// 0xFFFA로부터 새로운 Program Counter 주소를 가져옴
void nes6502::nmi()
{
	cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF);
	stkp--;
	cpuWrite(0x0100 + stkp, pc & 0x00FF);
	stkp--;

	SetFlag(B, 0);
	SetFlag(U, 1);
	SetFlag(I, 1);
	cpuWrite(0x0100 + stkp, status);
	stkp--;

	addr_abs = 0xFFFA;
	uint16_t lo = cpuRead(addr_abs + 0);
	uint16_t hi = cpuRead(addr_abs + 1);
	pc = (hi << 8) | lo;

	cycles = 8;
}
void nes6502::clock()
{
	if (cycles == 0)
	{
		opcode = cpuRead(pc);
		pc++;

		cycles = lookup[opcode].cycles;
		uint8_t additional_cycle1 = (this->*lookup[opcode].addrmode)();
		uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();
		cycles += (additional_cycle1 & additional_cycle2);
	}
	cycles--;
}


///////////////////////////////////////////////////////////////////////////////
// FLAG FUNCTIONS
// 상태 레지스터의 특정 비트값을 반환
uint8_t nes6502::GetFlag(FLAGS6502 f)
{
	return ((status & f) > 0) ? 1 : 0;
}

// 상태 레지스터의 특정값을 설정하거나 초기화
void nes6502::SetFlag(FLAGS6502 f, bool v)
{
	if (v)
		status |= f;
	else
		status &= ~f;
}

///////////////////////////////////////////////////////////////////////////////
// ADDRESSING MODES
// 6502의 주소는 0x0000 - 0xFFFF로 이루어져 있음. 
// 앞의 두 자리 high byte는 page, 뒤의 두자리 low byte는 offset
// 따라서 256바이트의 페이지가 256개가 있다는 것을 의미(0x00 - 0xFF: 0~255이기 때문)
// 몇몇 addressing mode는 page의 경계를 넘어야할 때 추가적인 clock cycle을 필요로함
// 이를 가능하게 하는 몇몇 명령어가 있는데, 이를 위해서
// clock cycle을 필요로 할 가능성이 있다는 플래그를 반환함
// instruction과 addressing function 모두 1을 반환한다면 추가적인 clock cycle이 필요하다는 의미임


// Address Mode: Implied
// instruction에 추가적인 데이터가 필요하지 않음
// fetched 변수를 accumulator의 값으로 설정
uint8_t nes6502::IMP()
{
	fetched = a;
	return 0;
}


// Address Mode: Immediate
// instruction이 다음 바이트의 값을 사용할 것이므로
// 읽을 주소를 다음 바이트의 값을 가리키게함
uint8_t nes6502::IMM()
{
	addr_abs = pc++;
	return 0;
}

// Address Mode: Zero Page
// 메모리의 처음 256바이트만을 사용해 주소를 지정한다.
// 예를들어 LDA $35는 메모리 주소 $35에 저장된 값을 A에 넣는다.
// zero page를 사용하면 instruction이 1바이트 적게 사용하고 clock cycle도 더 짧음
// 이처럼 프로그램의 용량을 절약할 수 있기 때문에 대부분의 프로그램이
// 자주 사용하는 변수를 page zero에 저장하는 경향이 있음
uint8_t nes6502::ZP0()
{
	addr_abs = cpuRead(pc);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

// Address Mode: Zero Page with X Offset
// ZP0와 유사하지만 주소값에 X Register의 값이 더해짐
// Zero Page 내부를 iterate하는데 유용함
uint8_t nes6502::ZPX()
{
	addr_abs = (cpuRead(pc) + x);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

// Address Mode: Zero Page with Y Offset
// ZPX와 같지만 Y Register를 사용함
uint8_t nes6502::ZPY()
{
	addr_abs = (cpuRead(pc) + y);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}


// Address Mode: Relative
// branch instruction에만 사용되며 아무데나 점프할 수 없고 -128에서 +127의 범위로만 가능
uint8_t nes6502::REL()
{
	addr_rel = cpuRead(pc);
	pc++;
	// 0~127범위의 점프는 unsigned number지만 
	// -128~0범위의 점프는 signed number여야함
	// signed number의 첫번째 비트는 1인 것을 이용해 검사함
	if (addr_rel & 0x80) // signed number일경우 
		addr_rel |= 0xFF00;
	return 0;
}


// Address Mode: Absolute 
// 16비트 주소 전체(ex: 0x1234)를 불러옴
uint8_t nes6502::ABS()
{
	uint16_t lo = cpuRead(pc);
	pc++;
	uint16_t hi = cpuRead(pc);
	pc++;

	addr_abs = (hi << 8) | lo;
	return 0;
}


// Address Mode: Absolute with X Offset
// ABS와 같지만 불러온 주소값에 X Register의 값을 더함
// 결과값이 page, 즉 high byte를 변경할 경우 추가적인 clock cycle이 필요함
uint8_t nes6502::ABX()
{
	uint16_t lo = cpuRead(pc);
	pc++;
	uint16_t hi = cpuRead(pc);
	pc++;

	addr_abs = (hi << 8) | lo;
	addr_abs += x;
	// 결과값이 page를 변경할 경우 추가적인 clock cycle이 필요함
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}


// Address Mode: Absolute with Y  Offset
// ABS와 같지만 불러온 주소값에 Y Register의 값을 더함
// 결과값이 page, 즉 high byte를 변경할 경우 추가적인 clock cycle이 필요함
uint8_t nes6502::ABY()
{
	uint16_t lo = cpuRead(pc);
	pc++;
	uint16_t hi = cpuRead(pc);
	pc++;

	addr_abs = (hi << 8) | lo;
	addr_abs += y;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}
// 다음 3개의 address mode는 indirection(Pointer)를 사용함
// Address Mode: Indirect
// 읽은 16비트 값을 이용해서 이 값이 가리키는 주소의 값을 가져옴
// 하지만 하드웨어상의 버그가 있는데 이것도 에뮬레이트해야함
// 버그는 low byte의 값이 0xFF일때 발생하는데, 실제 주소의 high byte를 읽으려면 page의 경계를 넘어야하기 때문임
// 하지만 칩에서 설계된 대로 page의 경계를 넘어 동작하지 않고, 대신 같은 페이지에서 주소를 읽음
// An indirect JMP (xxFF) will fail because the MSB will be fetched from
// address xx00 instead of page xx + 1.
// 출처: https://nesdev.com/6502bugs.txt
uint8_t nes6502::IND()
{
	uint16_t ptr_lo = cpuRead(pc);
	pc++;
	uint16_t ptr_hi = cpuRead(pc);
	pc++;
	
	uint16_t ptr = (ptr_hi << 8) | ptr_lo;

	if (ptr_lo == 0x00FF) // page boundary 하드웨어 버그를 시뮬레이션함
	{
		addr_abs = (cpuRead(ptr & 0xFF00) << 8) | cpuRead(ptr + 0);
	}
	else // 정상 작동했을 경우
	{
		addr_abs = (cpuRead(ptr + 1) << 8) | cpuRead(ptr + 0);
	}

	return 0;
}

// Address Mode: Indirect X (Indirect addressing of the Zero Page with X offset)
// 읽어들인 8비트 값에 X Register의 값을 더함
// 이 결과값으로 Zero Page의 주소의 값을 읽고 이 값에 해당하는 주소를 읽음
// 예를들어 명령어가 LDA ($20, X)이고 X가 $04, 0024의 값이 74 20일 경우
// $20에 X의 값 $4가 더해져 $24를 얻음
// $24에서 값을 가져옴, 이 값은 $2074임
// 주소 $2074의 값을 Register A에 불러옴
uint8_t nes6502::IZX()
{
	uint16_t t = cpuRead(pc);
	pc++;

	uint16_t lo = cpuRead((uint16_t)(t + (uint16_t)x) & 0x00FF);
	uint16_t hi = cpuRead((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;

	return 0;
}


// Address Mode: Indirect Y
// 읽은 8비트 값으로 Zero Page의 주소값을 읽고 이 값에 Y값을 더함
// 이 결과값에 해당하는 주소를 읽음
// 예를들어 명령어가 LDA ($86),Y이고 Y가 $10, 0086의 값이 28 40일 경우
// $86의 값 $4028을 가져와서 Y의 값 $10을 더해 $4038을 얻음
// 주소 $4038의 값을 Register A에 불러옴
// 먼저 X값을 더한 결과값으로 Zero Page를 읽는 IZX와는 다르게 
// IZY는 Zero Page에서 주소값을 가져온 뒤 Y값을 더함
uint8_t nes6502::IZY()
{
	uint16_t t = cpuRead(pc);
	pc++;

	uint16_t lo = cpuRead(t & 0x00FF);
	uint16_t hi = cpuRead((t + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;
	addr_abs += y;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}



// This function sources the data used by the instruction into 
// a convenient numeric variable. Some instructions dont have to 
// fetch data as the source is implied by the instruction. For example
// "INX" increments the X register. There is no additional data
// required. For all other addressing modes, the data resides at 
// the location held within addr_abs, so it is read from there. 
// Immediate adress mode exploits this slightly, as that has
// set addr_abs = pc + 1, so it fetches the data from the
// next byte for example "LDA $FF" just loads the accumulator with
// 256, i.e. no far reaching memory fetch is required. "fetched"
// is a variable global to the CPU, and is set by calling this 
// function. It also returns it for convenience.
uint8_t nes6502::fetch()
{
	// IMP는 fetch할 instrcution이 없으므로 제외
	if (!(lookup[opcode].addrmode == &nes6502::IMP))
		fetched = cpuRead(addr_abs);
	return fetched;
}
///////////////////////////////////////////////////////////////////////////////
// INSTRUCTION IMPLEMENTATIONS
// 
// Instruction: Add with Carry In
// Function:    A = A + M + C
// Flags Out:   C, V, N, Z
uint8_t nes6502::ADC()
{
	// 덧셈에 사용할 값을 가져옴
	fetch();
	// Carry bit를 캡쳐하기 편하게 하기 위해 16bit domain에서 덧셈을 계산함
	temp = (uint16_t)a + (uint16_t)fetched + (uint16_t)GetFlag(C);

	// Carry Flag, 8비트값인 255보다 클 경우 Carry가 발생
	SetFlag(C, temp > 255);
	// Zero Flag, 계산 결과값이 0일 경우
	SetFlag(Z, (temp & 0x00FF) == 0);
	// oVerflow Flag
	// A = Accumulator = a
	// M = Memory	   = fetched
	// R = Result      = temp
	// 0 = positive, 1 = negative
	// Positive Number + Positive Number = Negative Result -> Overflow
	// Negative Number + Negative Number = Positive Result -> Overflow
	// Positive Number + Negative Number = Either Result   -> Cannot Overflow
	// Positive Number + Positive Number = Positive Result -> No Overflow
	// Negative Number + Negative Number = Negative Result -> NO Overflow
	// A  M  R | V | A^R | A^M |~(A^M) | 
	// 0  0  0 | 0 |  0  |  0  |   1   |
	// 0  0  1 | 1 |  1  |  0  |   1   |
	// 0  1  0 | 0 |  0  |  1  |   0   |
	// 0  1  1 | 0 |  1  |  1  |   0   |  따라서 V = ~(A^M) & (A^R)
	// 1  0  0 | 0 |  1  |  1  |   0   |
	// 1  0  1 | 0 |  0  |  1  |   0   |
	// 1  1  0 | 1 |  1  |  0  |   1   |
	// 1  1  1 | 0 |  0  |  0  |   1   |
	SetFlag(V, (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)temp)) & 0x0080);
	// Negative Flag, 8비트 결과값의 최상위 비트가 1일경우
	SetFlag(N, temp & 0x80);
	// Accumulator에 덧셈 결과값을 저장(8비트만 저장해야함!)
	a = temp & 0x00FF;
	// 추가적인 clock cycle이 필요할 수도 있음
	return 1;
}


// Instruction: Subtraction with Borrow In
// Function:    A = A - M - (1 - C)
// Flags Out:   C, V, N, Z
//
// Explanation:
// Given the explanation for ADC above, we can reorganise our data
// to use the same computation for addition, for subtraction by multiplying
// the data by -1, i.e. make it negative
//
// A = A - M - (1 - C)  ->  A = A + -1 * (M - (1 - C))  ->  A = A + (-M + 1 + C)
//
// To make a signed positive number negative, we can invert the bits and add 1
// (OK, I lied, a little bit of 1 and 2s complement :P)
//
//  5 = 00000101
// -5 = 11111010 + 00000001 = 11111011 (or 251 in our 0 to 255 range)
//
// The range is actually unimportant, because if I take the value 15, and add 251
// to it, given we wrap around at 256, the result is 10, so it has effectively 
// subtracted 5, which was the original intention. (15 + 251) % 256 = 10
//
// Note that the equation above used (1-C), but this got converted to + 1 + C.
// This means we already have the +1, so all we need to do is invert the bits
// of M, the data(!) therfore we can simply add, exactly the same way we did 
// before.
uint8_t nes6502::SBC()
{
	// 뺄셈에 사용할 값을 가져옴
	fetch();
	// 덧셈과 마찬가지로 Carry를 알기 쉽게 16bit domain으로 계산
	// bitwise xor를 사용해 하위 8비트를 invert함
	// A = A - M - (1 - C)  ->  
	// A = A + -1 * (M - (1 - C))  ->  
	// A = A + (-M + 1 + C)
	// 이므로 M만 음수로 바꿔서 계산하면 됨
	// 최종 식이 +1 + C인 이유는 -에서 +로 바꿀때 bit를 invert시키고 1을 더하기 때문임(2의 보수의 음수 표현법)
	uint16_t value = ((uint16_t)fetched) ^ 0x00FF;
	// 나머지는 덧셈과 동일
	temp = (uint16_t)a + value + (uint16_t)GetFlag(C);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, ((temp & 0x00FF) == 0));
	SetFlag(V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080);
	SetFlag(N, temp & 0x0080);
	a = temp & 0x00FF;
	return 1;
}

// OK! Complicated operations are done! the following are much simpler
// and conventional. The typical order of events is:
// 1) Fetch the data you are working with
// 2) Perform calculation
// 3) Store the result in desired place
// 4) Set Flags of the status register
// 5) Return if instruction has potential to require additional 
//    clock cycle

// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
uint8_t nes6502::AND()
{
	fetch();
	a = a & fetched;
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 1;
}

// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
uint8_t nes6502::ASL()
{
	fetch();
	temp = (uint16_t)fetched << 1;
	SetFlag(C, (temp & 0xFF00) > 0);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x80);
	if (lookup[opcode].addrmode == &nes6502::IMP)
		a = temp & 0x00FF;
	else
		cpuWrite(addr_abs, temp & 0x00FF);
	return 0;
}

// Instruction: Branch if Carry Clear
// Function:    if(C == 0) pc = address 
uint8_t nes6502::BCC()
{
	if (GetFlag(C) == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;
		// if branch needs to cross a page boundary
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}


// Instruction: Branch if Carry Set
// Function:    if(C == 1) pc = address
uint8_t nes6502::BCS()
{
	if (GetFlag(C) == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// Instruction: Branch if Equal
// Function:    if(Z == 1) pc = address
uint8_t nes6502::BEQ()
{
	if (GetFlag(Z) == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

uint8_t nes6502::BIT()
{
	fetch();
	temp = a & fetched;
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, fetched & (1 << 7));
	SetFlag(V, fetched & (1 << 6));
	return 0;
}

// Instruction: Branch if Negative
// Function:    if(N == 1) pc = address
uint8_t nes6502::BMI()
{
	if (GetFlag(N) == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// Instruction: Branch if Not Equal
// Function:    if(Z == 0) pc = address
uint8_t nes6502::BNE()
{
	if (GetFlag(Z) == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// Instruction: Branch if Positive
// Function:    if(N == 0) pc = address
uint8_t nes6502::BPL()
{
	if (GetFlag(N) == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

// Instruction: Break
// Function:    Program Sourced Interrupt
uint8_t nes6502::BRK()
{
	pc++;

	SetFlag(I, 1);
	cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF);
	stkp--;
	cpuWrite(0x0100 + stkp, pc & 0x00FF);
	stkp--;

	SetFlag(B, 1);
	cpuWrite(0x0100 + stkp, status);
	stkp--;
	SetFlag(B, 0);

	pc = (uint16_t)cpuRead(0xFFFE) | ((uint16_t)cpuRead(0xFFFF) << 8);
	return 0;
}


// Instruction: Branch if Overflow Clear
// Function:    if(V == 0) pc = address
uint8_t nes6502::BVC()
{
	if (GetFlag(V) == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}


// Instruction: Branch if Overflow Set
// Function:    if(V == 1) pc = address
uint8_t nes6502::BVS()
{
	if (GetFlag(V) == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}


// Instruction: Clear Carry Flag
// Function:    C = 0
uint8_t nes6502::CLC()
{
	SetFlag(C, false);
	return 0;
}


// Instruction: Clear Decimal Flag
// Function:    D = 0
uint8_t nes6502::CLD()
{
	SetFlag(D, false);
	return 0;
}


// Instruction: Disable Interrupts / Clear Interrupt Flag
// Function:    I = 0
uint8_t nes6502::CLI()
{
	SetFlag(I, false);
	return 0;
}


// Instruction: Clear Overflow Flag
// Function:    V = 0
uint8_t nes6502::CLV()
{
	SetFlag(V, false);
	return 0;
}

// Instruction: Compare Accumulator
// Function:    C <- A >= M      Z <- (A - M) == 0
// Flags Out:   N, C, Z
uint8_t nes6502::CMP()
{
	fetch();
	temp = (uint16_t)a - (uint16_t)fetched;
	SetFlag(C, a >= fetched);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 1;
}


// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
uint8_t nes6502::CPX()
{
	fetch();
	temp = (uint16_t)x - (uint16_t)fetched;
	SetFlag(C, x >= fetched);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}


// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
uint8_t nes6502::CPY()
{
	fetch();
	temp = (uint16_t)y - (uint16_t)fetched;
	SetFlag(C, y >= fetched);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}


// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
uint8_t nes6502::DEC()
{
	fetch();
	temp = fetched - 1;
	cpuWrite(addr_abs, temp & 0x00FF);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}


// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
uint8_t nes6502::DEX()
{
	x--;
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
	return 0;
}


// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
uint8_t nes6502::DEY()
{
	y--;
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);
	return 0;
}


// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
uint8_t nes6502::EOR()
{
	fetch();
	a = a ^ fetched;
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 1;
}


// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
uint8_t nes6502::INC()
{
	fetch();
	temp = fetched + 1;
	cpuWrite(addr_abs, temp & 0x00FF);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}


// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
uint8_t nes6502::INX()
{
	x++;
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
	return 0;
}


// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
uint8_t nes6502::INY()
{
	y++;
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);
	return 0;
}


// Instruction: Jump To Location
// Function:    pc = address
uint8_t nes6502::JMP()
{
	pc = addr_abs;
	return 0;
}


// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, pc = address
uint8_t nes6502::JSR()
{
	pc--;

	cpuWrite(0x0100 + stkp, (pc >> 8) & 0x00FF);
	stkp--;
	cpuWrite(0x0100 + stkp, pc & 0x00FF);
	stkp--;

	pc = addr_abs;
	return 0;
}


// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
uint8_t nes6502::LDA()
{
	fetch();
	a = fetched;
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 1;
}


// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
uint8_t nes6502::LDX()
{
	fetch();
	x = fetched;
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
	return 1;
}


// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
uint8_t nes6502::LDY()
{
	fetch();
	y = fetched;
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);
	return 1;
}

uint8_t nes6502::LSR()
{
	fetch();
	SetFlag(C, fetched & 0x0001);
	temp = fetched >> 1;
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	if (lookup[opcode].addrmode == &nes6502::IMP)
		a = temp & 0x00FF;
	else
		cpuWrite(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t nes6502::NOP()
{
	// Sadly not all NOPs are equal, Ive added a few here
	// based on https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
	// and will add more based on game compatibility, and ultimately
	// I'd like to cover all illegal opcodes too
	switch (opcode)
	{
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
		break;
	}
	return 0;
}

// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
uint8_t nes6502::ORA()
{
	fetch();
	a = a | fetched;
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 1;
}

// Instruction: Push Accumulator to Stack
// Function:    A -> stack
uint8_t nes6502::PHA()
{
	// stack은 메모리에 존재하기 때문에 Bus에 메모리를 write해야함
	// 0x0100: 하드코딩된 스택포인터의 base location
	// stkp:   offest
	cpuWrite(0x0100 + stkp, a);
	stkp--;
	return 0;
}

// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
uint8_t nes6502::PHP()
{
	cpuWrite(0x0100 + stkp, status | B | U);
	SetFlag(B, 0);
	SetFlag(U, 0);
	stkp--;
	return 0;
}

// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
uint8_t nes6502::PLA()
{
	stkp++;
	a = cpuRead(0x0100 + stkp);
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 0;
}

// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
uint8_t nes6502::PLP()
{
	stkp++;
	status = cpuRead(0x0100 + stkp);
	SetFlag(U, 1);
	return 0;
}

uint8_t nes6502::ROL()
{
	fetch();
	temp = (uint16_t)(fetched << 1) | GetFlag(C);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	if (lookup[opcode].addrmode == &nes6502::IMP)
		a = temp & 0x00FF;
	else
		cpuWrite(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t nes6502::ROR()
{
	fetch();
	temp = (uint16_t)(GetFlag(C) << 7) | (fetched >> 1);
	SetFlag(C, fetched & 0x01);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x0080);
	if (lookup[opcode].addrmode == &nes6502::IMP)
		a = temp & 0x00FF;
	else
		cpuWrite(addr_abs, temp & 0x00FF);
	return 0;
}

uint8_t nes6502::RTI()
{
	stkp++;
	status = cpuRead(0x0100 + stkp);
	status &= ~B;
	status &= ~U;

	stkp++;
	pc = (uint16_t)cpuRead(0x0100 + stkp);
	stkp++;
	pc |= (uint16_t)cpuRead(0x0100 + stkp) << 8;
	return 0;
}

uint8_t nes6502::RTS()
{
	stkp++;
	pc = (uint16_t)cpuRead(0x0100 + stkp);
	stkp++;
	pc |= (uint16_t)cpuRead(0x0100 + stkp) << 8;

	pc++;
	return 0;
}




// Instruction: Set Carry Flag
// Function:    C = 1
uint8_t nes6502::SEC()
{
	SetFlag(C, true);
	return 0;
}


// Instruction: Set Decimal Flag
// Function:    D = 1
uint8_t nes6502::SED()
{
	SetFlag(D, true);
	return 0;
}


// Instruction: Set Interrupt Flag / Enable Interrupts
// Function:    I = 1
uint8_t nes6502::SEI()
{
	SetFlag(I, true);
	return 0;
}


// Instruction: Store Accumulator at Address
// Function:    M = A
uint8_t nes6502::STA()
{
	cpuWrite(addr_abs, a);
	return 0;
}


// Instruction: Store X Register at Address
// Function:    M = X
uint8_t nes6502::STX()
{
	cpuWrite(addr_abs, x);
	return 0;
}


// Instruction: Store Y Register at Address
// Function:    M = Y
uint8_t nes6502::STY()
{
	cpuWrite(addr_abs, y);
	return 0;
}


// Instruction: Transfer Accumulator to X Register
// Function:    X = A
// Flags Out:   N, Z
uint8_t nes6502::TAX()
{
	x = a;
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
	return 0;
}


// Instruction: Transfer Accumulator to Y Register
// Function:    Y = A
// Flags Out:   N, Z
uint8_t nes6502::TAY()
{
	y = a;
	SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);
	return 0;
}


// Instruction: Transfer Stack Pointer to X Register
// Function:    X = stack pointer
// Flags Out:   N, Z
uint8_t nes6502::TSX()
{
	x = stkp;
	SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
	return 0;
}


// Instruction: Transfer X Register to Accumulator
// Function:    A = X
// Flags Out:   N, Z
uint8_t nes6502::TXA()
{
	a = x;
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 0;
}


// Instruction: Transfer X Register to Stack Pointer
// Function:    stack pointer = X
uint8_t nes6502::TXS()
{
	stkp = x;
	return 0;
}


// Instruction: Transfer Y Register to Accumulator
// Function:    A = Y
// Flags Out:   N, Z
uint8_t nes6502::TYA()
{
	a = y;
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 0;
}

// This function captures illegal opcodes
uint8_t nes6502::XXX()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

bool nes6502::complete()
{
	return cycles == 0;
}

// This is the disassembly function. Its workings are not required for emulation.
// It is merely a convenience function to turn the binary instruction code into
// human readable form. Its included as part of the emulator because it can take
// advantage of many of the CPUs internal operations to do this.
std::map<uint16_t, std::string> nes6502::disassemble(uint16_t nStart, uint16_t nStop)
{
	uint32_t addr = nStart;
	uint8_t value = 0x00, lo = 0x00, hi = 0x00;
	std::map<uint16_t, std::string> mapLines;
	uint16_t line_addr = 0;

	// A convenient utility to convert variables into
	// hex strings because "modern C++"'s method with 
	// streams is atrocious
	auto hex = [](uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	// Starting at the specified address we read an instruction
	// byte, which in turn yields information from the lookup table
	// as to how many additional bytes we need to read and what the
	// addressing mode is. I need this info to assemble human readable
	// syntax, which is different depending upon the addressing mode

	// As the instruction is decoded, a std::string is assembled
	// with the readable output
	while (addr <= (uint32_t)nStop)
	{
		line_addr = addr;

		// Prefix line with instruction address
		std::string sInst = "$" + hex(addr, 4) + ": ";

		// Read instruction, and get its readable name
		uint8_t opcode = bus->cpuRead(addr, true); addr++;
		sInst += lookup[opcode].name + " ";

		// Get oprands from desired locations, and form the
		// instruction based upon its addressing mode. These
		// routines mimmick the actual fetch routine of the
		// 6502 in order to get accurate data as part of the
		// instruction
		if (lookup[opcode].addrmode == &nes6502::IMP)
		{
			sInst += " {IMP}";
		}
		else if (lookup[opcode].addrmode == &nes6502::IMM)
		{
			value = bus->cpuRead(addr, true); addr++;
			sInst += "#$" + hex(value, 2) + " {IMM}";
		}
		else if (lookup[opcode].addrmode == &nes6502::ZP0)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + " {ZP0}";
		}
		else if (lookup[opcode].addrmode == &nes6502::ZPX)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", X {ZPX}";
		}
		else if (lookup[opcode].addrmode == &nes6502::ZPY)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
		}
		else if (lookup[opcode].addrmode == &nes6502::IZX)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + ", X) {IZX}";
		}
		else if (lookup[opcode].addrmode == &nes6502::IZY)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + "), Y {IZY}";
		}
		else if (lookup[opcode].addrmode == &nes6502::ABS)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = bus->cpuRead(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
		}
		else if (lookup[opcode].addrmode == &nes6502::ABX)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = bus->cpuRead(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
		}
		else if (lookup[opcode].addrmode == &nes6502::ABY)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = bus->cpuRead(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
		}
		else if (lookup[opcode].addrmode == &nes6502::IND)
		{
			lo = bus->cpuRead(addr, true); addr++;
			hi = bus->cpuRead(addr, true); addr++;
			sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
		}
		else if (lookup[opcode].addrmode == &nes6502::REL)
		{
			value = bus->cpuRead(addr, true); addr++;
			sInst += "$" + hex(value, 2) + " [$" + hex(addr + value, 4) + "] {REL}";
		}

		// Add the formed string to a std::map, using the instruction's
		// address as the key. This makes it convenient to look for later
		// as the instructions are variable in length, so a straight up
		// incremental index is not sufficient.
		mapLines[line_addr] = sInst;
	}

	return mapLines;
}