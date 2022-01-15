#include "nes2C02.h"
#include <SDL.h>

nes2C02::nes2C02()
{
	palScreen[0x00] = SDL_Color{ 84, 84, 84 };
	palScreen[0x01] = SDL_Color{0, 30, 116 };
	palScreen[0x02] = SDL_Color{8, 16, 144 };
	palScreen[0x03] = SDL_Color{48, 0, 136 };
	palScreen[0x04] = SDL_Color{68, 0, 100 };
	palScreen[0x05] = SDL_Color{92, 0, 48 };
	palScreen[0x06] = SDL_Color{84, 4, 0 };
	palScreen[0x07] = SDL_Color{60, 24, 0 };
	palScreen[0x08] = SDL_Color{32, 42, 0 };
	palScreen[0x09] = SDL_Color{8, 58, 0 };
	palScreen[0x0A] = SDL_Color{0, 64, 0 };
	palScreen[0x0B] = SDL_Color{0, 60, 0 };
	palScreen[0x0C] = SDL_Color{0, 50, 60 };
	palScreen[0x0D] = SDL_Color{0, 0, 0 };
	palScreen[0x0E] = SDL_Color{0, 0, 0 };
	palScreen[0x0F] = SDL_Color{0, 0, 0 };

	palScreen[0x10] = SDL_Color{152, 150, 152 };
	palScreen[0x11] = SDL_Color{8, 76, 196 };
	palScreen[0x12] = SDL_Color{48, 50, 236 };
	palScreen[0x13] = SDL_Color{92, 30, 228 };
	palScreen[0x14] = SDL_Color{136, 20, 176 };
	palScreen[0x15] = SDL_Color{160, 20, 100 };
	palScreen[0x16] = SDL_Color{152, 34, 32 };
	palScreen[0x17] = SDL_Color{120, 60, 0 };
	palScreen[0x18] = SDL_Color{84, 90, 0 };
	palScreen[0x19] = SDL_Color{40, 114, 0 };
	palScreen[0x1A] = SDL_Color{8, 124, 0 };
	palScreen[0x1B] = SDL_Color{0, 118, 40 };
	palScreen[0x1C] = SDL_Color{0, 102, 120 };
	palScreen[0x1D] = SDL_Color{0, 0, 0 };
	palScreen[0x1E] = SDL_Color{0, 0, 0 };
	palScreen[0x1F] = SDL_Color{0, 0, 0 };

	palScreen[0x20] = SDL_Color{236, 238, 236 };
	palScreen[0x21] = SDL_Color{76, 154, 236 };
	palScreen[0x22] = SDL_Color{120, 124, 236 };
	palScreen[0x23] = SDL_Color{176, 98, 236 };
	palScreen[0x24] = SDL_Color{228, 84, 236 };
	palScreen[0x25] = SDL_Color{236, 88, 180 };
	palScreen[0x26] = SDL_Color{236, 106, 100 };
	palScreen[0x27] = SDL_Color{212, 136, 32 };
	palScreen[0x28] = SDL_Color{160, 170, 0 };
	palScreen[0x29] = SDL_Color{116, 196, 0 };
	palScreen[0x2A] = SDL_Color{76, 208, 32 };
	palScreen[0x2B] = SDL_Color{56, 204, 108 };
	palScreen[0x2C] = SDL_Color{56, 180, 204 };
	palScreen[0x2D] = SDL_Color{60, 60, 60 };
	palScreen[0x2E] = SDL_Color{0, 0, 0 };
	palScreen[0x2F] = SDL_Color{0, 0, 0 };

	palScreen[0x30] = SDL_Color{236, 238, 236 };
	palScreen[0x31] = SDL_Color{168, 204, 236 };
	palScreen[0x32] = SDL_Color{188, 188, 236 };
	palScreen[0x33] = SDL_Color{212, 178, 236 };
	palScreen[0x34] = SDL_Color{236, 174, 236 };
	palScreen[0x35] = SDL_Color{236, 174, 212 };
	palScreen[0x36] = SDL_Color{236, 180, 176 };
	palScreen[0x37] = SDL_Color{228, 196, 144 };
	palScreen[0x38] = SDL_Color{204, 210, 120 };
	palScreen[0x39] = SDL_Color{180, 222, 120 };
	palScreen[0x3A] = SDL_Color{168, 226, 144 };
	palScreen[0x3B] = SDL_Color{152, 226, 180 };
	palScreen[0x3C] = SDL_Color{160, 214, 228 };
	palScreen[0x3D] = SDL_Color{160, 162, 160 };
	palScreen[0x3E] = SDL_Color{0, 0, 0 };
	palScreen[0x3F] = SDL_Color{0, 0, 0 };
}
nes2C02::~nes2C02()
{

}
vts::Sprite& nes2C02::GetScreen()
{
	return sprScreen;
}

vts::Sprite& nes2C02::GetNameTable(uint8_t i)
{
	return sprNameTable[i];
}

vts::Sprite& nes2C02::GetPatternTable(uint8_t i)
{
	return sprPatternTable[i];
}

uint8_t nes2C02::cpuRead(uint16_t addr, bool rdonly)
{
    uint8_t data = 0x00;
	switch (addr)
	{
	case 0x0000: // Control
		break;
	case 0x0001: // Mask
		break;
	case 0x0002: // Status
		break;
	case 0x0003: // OAM Address
		break;
	case 0x0004: // OAM Data
		break;
	case 0x0005: // Scroll
		break;
	case 0x0006: // PPU Address
		break;
	case 0x0007: // PPU Data
		break;
	}
    return data;
}

void nes2C02::cpuWrite(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x0000: // Control
		break;
	case 0x0001: // Mask
		break;
	case 0x0002: // Status
		break;
	case 0x0003: // OAM Address
		break;
	case 0x0004: // OAM Data
		break;
	case 0x0005: // Scroll
		break;
	case 0x0006: // PPU Address
		break;
	case 0x0007: // PPU Data
		break;
	}
}

uint8_t nes2C02::ppuRead(uint16_t addr, bool rdonly)
{
    uint8_t data = 0x00;
    addr &= 0x3FFF;
	
	if (cart->ppuRead(addr, data))
	{

	}
    return data;
}

void nes2C02::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;

	if (cart->ppuWrite(addr, data))
	{

	}
}

void nes2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	this->cart = cartridge;
}

void nes2C02::clock()
{
	// random noise for test
	bool b = sprScreen.SetPixel(cycle - 1, scanline, palScreen[(rand() % 2) ? 0x3F : 0x30]);

	cycle++;
	// hard-coded boundaries
	if (cycle >= 341)
	{
		cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frame_complete = true;
		}
	}
}
