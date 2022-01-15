#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>

#include "Mapper_000.h"

class Cartridge
{
	public:
	Cartridge(const std::string& sFileName);
	~Cartridge();

	private:
	bool bImageValid = false;

	std::vector<uint8_t> vPRGMemory; // Program Memory
	std::vector<uint8_t> vCHRMemory; // Character Memory

	uint8_t nMapperID = 0; // which mapper are we using
	uint8_t nPRGBanks = 0; // store how many banks of the respective memories
	uint8_t nCHRBanks = 0;

	std::shared_ptr<Mapper> pMapper;

	public:
	bool ImageValid();
	// Communications with Main Bus
	bool cpuRead(uint16_t addr,  uint8_t & data);
	bool cpuWrite(uint16_t addr, uint8_t  data);

	// Communications with PPU Bus
	bool ppuRead(uint16_t addr, uint8_t& data);
	bool ppuWrite(uint16_t addr, uint8_t data);
};

