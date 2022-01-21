#pragma once
#include <cstdint>
#include <memory>

#include "Engine.h"

#include "Cartridge.h"
// 0x0000 - 0x1FFF : Pattern Memory ("CHR ROM") Contains sprites and these stored as bitmapped images
// 0x2000 - 0x3EFF : Name Table Memory ("VRAM") Describes layout of the background
// 0x3F00 - 0x3FFF : Palette Memory ("Color")

// https://steemit.com/kr/@drgkim/retro-games-or-nes-03
// Pattern Memory
// 8KB의 크기이며 2개의 섹션으로 나눠져 있음(0KB - 4KB / 4KB - 8KB)
// 나눠진 섹션은 하나는 캐릭터 스프라이트, 하나는 배경으로 사용함
// 한 섹션은 다시 16x16의 그리드(128x128 pixel)로 나뉨
// 따라서 그리드의 한 칸은 8x8 픽셀(스프라이트 혹은 타일)임
// 사이즈가 작아보일 수 있지만 여러 타일을 이어붙이거나 재활용해서 게임 그래픽을 표현함
// 
// Pixels 
// 한 픽셀 당 2bits를 사용해 4개의 색을 사용 가능함(00, 01, 10, 11)
// 타일에 대한 정보는 2개의 bit planes에 저장됨
// LSB(Least Significant Bit) plane, MSB(Most Significant Bit) plane
// 특정 픽셀에 대한 값은 이 두 plane의 특정 픽셀을 더한 값
// 예를 들면 (x,y) 픽셀의 값이 value이라면 value = LSB의 (x,y)픽셀 value + MSB의 (x,y)픽셀 value
// 픽셀값 0은 transparent 효과를 냄(밑에 자세히 설명)
// 이런 방식을 사용함으로써 데이터를 추출하기 위해 많은 shift나 bitwise연산을 하지 않아도 됨

// Palettes (Color)
// 2 bits만을 사용해서 색을 표현하기엔 부족하기 때문에 이 값을 팔레트 값과 함께 사용
// 예를들어 팔레트 인덱스가 1, 2bits 픽셀값이 3이라 하면 각각의 팔레트가 4 bytes인 것을 알기 때문에
// $3F00로부터의 offset은 1*4 + 3 = 7, 즉 Palettes[1]의 3번째 색임
// 
// 픽셀값이 0인 각 팔레트의 4번째 색(마지막 색)은 배경 색의 mirror로 사용해 transparent효과를 냄
// 이렇게 하면 아무 인덱스의 팔레트나 선택해도 항상 배경 색을 접근할 수 있음
// 에를들어 팔레트 인덱스가 1, 2bits 픽셀값이 0이면 offset은 1*4 + 0 = 4, 즉 Palettes[1]의 4번째 색(배경색)
// https://namu.wiki/w/%ED%8C%A8%EB%B0%80%EB%A6%AC%20%EC%BB%B4%ED%93%A8%ED%84%B0#fn-12
// 
//			$3F00		 : 1 background color
// Background palettes
// index[0] $3F01 - $3F04: 3 color, 1 mirrors background color
// index[1] $3F05 - $3F08: 3 color, 1 mirrors background color
// index[2] $3F09 - $3F0C: ...
// index[3] $3F0D - $3F10: ...
// 
// Foreground(Sprites) palettes
// index[4] $3F11 - $3F14: ...
// index[5] $3F15 - $3F18: ...
// index[6] $3F19 - $3F1C: ...
// index[7] $3F1D - $3F20: ...

class nes2C02
{
	public:
	nes2C02();
	~nes2C02();

	public:
	uint8_t tblName[2][1024]; // vram used to hold the name table information

	private:
	uint8_t tblPalette[32]; // palette information
	uint8_t tblPattern[2][4096]; // pattern memory, not necessary

	private:
	SDL_Color  palScreen[0x40]; // Palette Screen
	vts::Sprite sprScreen = vts::Sprite(256, 240);
	vts::Sprite sprNameTable[2] = { vts::Sprite(256, 240), vts::Sprite(256, 240) };
	vts::Sprite sprPatternTable[2] = { vts::Sprite(128, 128), vts::Sprite(128, 128) };

	public:
	// Debug Utilities
	vts::Sprite& GetScreen();
	vts::Sprite& GetNameTable(uint8_t i);
	vts::Sprite& GetPatternTable(uint8_t i, uint8_t palette);
	SDL_Color& GetColorFromPaletteRam(uint8_t palette, uint8_t pixel);
	bool frame_complete = false;

	private:
	// Registers
	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg;
	}status;

	union
	{
		struct
		{
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};

		uint8_t reg;
	} mask;

	union PPUCTRL
	{
		struct
		{
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1; // unused
			uint8_t enable_nmi : 1;
		};

		uint8_t reg;
	} control;

	union loopy_register
	{
		// Credit to Loopy for working this out :D
		// https://wiki.nesdev.org/w/index.php/PPU_scrolling#PPU_internal_registers
		struct
		{

			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};

		uint16_t reg = 0x0000;
	};

	loopy_register vram_addr; // 배경 타일 정보를 추출하기 위한 Name table에 대한 포인터 주소
	loopy_register tram_addr; // Temporary store of information to be "transferred" into "pointer" at various times

	// Pixel offset horizontally
	uint8_t fine_x = 0x00;

	// Internal communications
	uint8_t address_latch = 0x00; // 낮은 바이트(low byte)에 쓰는지 높은 바이트(high byte)에 쓰는지 알기 위한 값
	uint8_t ppu_data_buffer = 0x00; // PPU로부터 데이터를 읽는 것은 한 사이클이 지연되므로 그 데이터를 버퍼에 보관해야함

	// Pixel "dot" position information
	int16_t scanline = 0; // row of the sreen
	int16_t cycle = 0; // column of the screen


	// Background rendering
	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;
	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;

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
	void reset();
	bool nmi = false;
};
