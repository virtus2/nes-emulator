#pragma once
#include <cstdint>
#include <memory>
#include "Engine.h"
#include "Cartridge.h"
class nes2C02
{
	public:
	nes2C02();
	~nes2C02();
	
	private:
	uint8_t tblname[2][1024]; // vram used to hold the name table information
	uint8_t tblPalette[32]; // palette information
	uint8_t tblPattern[2][4096]; // pattern memory, not necessary

	public:
	// Communications with Main Bus
	uint8_t cpuRead(uint16_t addr, bool rdonly = false);
	void    cpuWrite(uint16_t addr, uint8_t  data);

	// Communications with PPU Bus
	uint8_t ppuRead(uint16_t addr, bool rdonly = false);
	void    ppuWrite(uint16_t addr, uint8_t data);

	private:
	std::shared_ptr<Cartridge> cart;

	public:
	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void clock();
};

