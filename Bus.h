#pragma once
#include <cstdint>
#include <array>

#include "nes6502.h"
#include "nes2C02.h"
#include "Cartridge.h"
class Bus
{
	public:
	Bus();
	~Bus();

	public: // Devices on bus
	nes6502 cpu;
	nes2C02 ppu;

	// RAM
	std::array<uint8_t, 64 * 1024> cpuRam;
	std::shared_ptr<Cartridge> cart;


	public: // Bus Read & Write
	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

	public: // System Interface
	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void reset(); // reset button of the front of the nes
	void clock(); // one system tick of emulation

	private:
	// 경과한 system clock 수
	uint32_t nSystemClockCounter = 0;
};