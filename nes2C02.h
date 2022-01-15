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

	private:
	SDL_Color  palScreen[0x40]; // Palette Screen
	vts::Sprite sprScreen = vts::Sprite(256,240);
	vts::Sprite sprNameTable[2] = { vts::Sprite(256, 240), vts::Sprite(256, 240) };
	vts::Sprite sprPatternTable[2] = { vts::Sprite(128, 128), vts::Sprite(128, 128) };

	public:
	// Debug Utilities
	vts::Sprite& GetScreen();
	vts::Sprite& GetNameTable(uint8_t i);
	vts::Sprite& GetPatternTable(uint8_t i);
	bool frame_complete = false;

	private:
	int16_t scanline = 0; // row of the sreen
	int16_t cycle = 0; // column of the screen
};

