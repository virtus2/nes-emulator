#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
class Cartridge
{
	public:
	Cartridge(const std::string& sFileName);
	~Cartridge();

	private:
	std::vector<uint8_t> vPRGMemory; // Program Memory
	std::vector<uint8_t> vCHRMemory; // Character Memory

	uint8_t nMapperID = 0; // which mapper are we using
	uint8_t nPRGBanks = 0; // store how many banks of the respective memories
	uint8_t nCHRBanks = 0;
	public:
	// Communications with Main Bus
	bool cpuRead(uint16_t addr, bool rdonly = false);
	bool cpuWrite(uint16_t addr, uint8_t  data);

	// Communications with PPU Bus
	bool ppuRead(uint16_t addr, bool rdonly = false);
	bool ppuWrite(uint16_t addr, uint8_t data);
};

