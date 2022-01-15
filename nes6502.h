#pragma once
#include <vector>
#include <string>
#include <map>
//Forward Declaration
class Bus;
class nes6502
{
    public:
    nes6502();
    ~nes6502();

    public:
    // CPU Core registers
    uint8_t a = 0x00; // Accumulator Register
    uint8_t x = 0x00; // X Register
    uint8_t y = 0x00; // Y Register
    uint8_t stkp = 0x00; // Stack Pointer(points to location on bus)
    uint16_t pc = 0x0000; // Program Counter
    uint8_t status = 0x00; // Status Register

    // External event functions
    void reset(); // Reset Interrupt
    void irq();   // Interrupt Request
    void nmi();   // Non-Maskable Interrupt Request
    void clock(); // 1 clock cycle update

    // 현재 명령어가 완료되었으면 true를 반환함
    // 매 cycle마다 clock을 시뮬레이트하는 대신 step-by-step 실행을 할 수 있게 해줌
    bool complete();

    void ConnectBus(Bus* n) { bus = n; }

    // Produces a map of strings, with keys equivalent to instruction start locations
    // in memory, for the specified address range
    // 특정 주소 범위의 바이너리 코드를 읽기 편한 string으로 바꿔줌
    std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);
    enum FLAGS6502
    {
        C = (1 << 0),	// Carry Bit
        Z = (1 << 1),	// Zero
        I = (1 << 2),	// Disable Interrupts
        D = (1 << 3),	// Decimal Mode (unused in this implementation)
        B = (1 << 4),	// Break
        U = (1 << 5),	// Unused
        V = (1 << 6),	// Overflow
        N = (1 << 7),	// Negative
    };

    private:
    uint8_t GetFlag(FLAGS6502 f);
    void SetFlag(FLAGS6502 f, bool v);

    // 에뮬레이션을 위한 변수
    uint8_t  fetched = 0x00;    // ALU에 입력된 input value
    uint16_t temp = 0x0000;     // 임시로 사용할 값
    uint16_t addr_abs = 0x0000; // 사용한 메모리의 끝주소
    uint16_t addr_rel = 0x00;   // branch이후의 절대 주소
    uint8_t  opcode = 0x00;     // Is the instruction byte
    uint8_t  cycles = 0;	    // Counts how many cycles the instruction has remaining
    uint32_t clock_count = 0;   // A global accumulation of the number of clocks

    // Linkage to the communications bus
    Bus* bus = nullptr;
    uint8_t cpuRead(uint16_t a);
    void cpuWrite(uint16_t a, uint8_t d);

    // 데이터는 메모리 주소 또는 instruction의 일부(즉시 사용할 수 있는 메모리 주소)에서 가져올 수 있음
    // instruction의 addressing mode에 따라 결정함
    uint8_t fetch();

    // opcode translation table를 컴파일하고 저장하는 데 사용할 구조체와 벡터
    // 숫자순으로 테이블에 저장되므로 디코딩 없이 명령어를 조회할 수 있음
    struct INSTRUCTION
    {
        std::string name; // 명령어의 이름(disassembly에 사용)
        uint8_t(nes6502::*operate)(void) = nullptr; // opcode의 실제 구현 함수를 가리키는 function pointer
        uint8_t(nes6502::* addrmode)(void) = nullptr; // addressing mechanism을 가리키는 function pointer
        uint8_t cycles = 0; // CPU가 명령어를 처리하는 데 필요한 clock cycles
    };
    std::vector<INSTRUCTION> lookup;

    private:
    // Addressing Modes =============================================
    // 6502는 메모리에 있는 데이터에 접근할 수 있는 다양한 주소 지정 모드를 가지고 있는데, 
    // 그 중 일부는 직접이고 일부는 간접적임(C++의 포인터처럼). 
    // 각 opcode는 어떤 addressing mode를 사용하여 명령을 처리하는지에 대한 정보를 담고 있음
    // address mode는 전체 명령을 구성하는 바이트 수를 변경하기 때문에 PC(Program Counter)가 올바른 위치에 있는지, 
    // 명령어가 필요한 주소로 준비되었는지, 명령어에 필요한 클럭 주기의 수를 계산해야함
    // 따라서 명령어를 실행하기 전에 먼저 addressing을 구현함
    // 메모리에 액세스하는 위치와 방법에 따라 필요한 사이클 수가 변경될 수 있으므로 사이클 수를 반환
    uint8_t IMP();	uint8_t IMM();
    uint8_t ZP0();	uint8_t ZPX();
    uint8_t ZPY();	uint8_t REL();
    uint8_t ABS();	uint8_t ABX();
    uint8_t ABY();	uint8_t IND();
    uint8_t IZX();	uint8_t IZY();
    private:
    // Opcodes ======================================================
	// There are 56 "legitimate" opcodes provided by the 6502 CPU. I
	// have not modelled "unofficial" opcodes. As each opcode is 
	// defined by 1 byte, there are potentially 256 possible codes.
	// Codes are not used in a "switch case" style on a processor,
	// instead they are repsonisble for switching individual parts of
	// CPU circuits on and off. The opcodes listed here are official, 
	// meaning that the functionality of the chip when provided with
	// these codes is as the developers intended it to be. Unofficial
	// codes will of course also influence the CPU circuitry in 
	// interesting ways, and can be exploited to gain additional
	// functionality!
	//
	// These functions return 0 normally, but some are capable of
	// requiring more clock cycles when executed under certain
	// conditions combined with certain addressing modes. If that is 
	// the case, they return 1.
	//
	// I have included detailed explanations of each function in 
	// the class implementation file. Note they are listed in
	// alphabetical order here for ease of finding.
    uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
    uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
    uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
    uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
    uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
    uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
    uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
    uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
    uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
    uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
    uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
    uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
    uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
    uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();
    // I capture all "unofficial" opcodes with this function. It is
    // functionally identical to a NOP
    uint8_t XXX(); 
#ifdef LOGMODE
    private:
    FILE* logfile = nullptr;
#endif
};