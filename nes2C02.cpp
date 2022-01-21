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
	// 렌더링된 화면을 들고 있는 현재 스프라이트를 반환
	return sprScreen;
}

vts::Sprite& nes2C02::GetNameTable(uint8_t i)
{
	// As of now unused, but a placeholder for nametable visualisation in teh future
	return sprNameTable[i];
}

vts::Sprite& nes2C02::GetPatternTable(uint8_t i, uint8_t palette)
{
	// 주어진 Pattern table에 대한 Character ROM을 Sprite형태로 그린다.(만든다.)
	// 패턴 테이블은 16x16 "타일 또는 문자"로 구성된다. 실행 중인 에뮬레이션과는 독립적이며, 
	// 이 에뮬레이션을 사용하는 것은 시스템 상태를 변경하지 않는다. 
	// 따라서 게임이 팔레트를 구축하지 않았거나 관련 CHR ROM 뱅크에 매핑하지 않은 경우 스프라이트가 비어 보일 수 있다. 
	// 이 접근법은 패턴 테이블의 실시간 추출을 허용하며, NES와 플레이어가 이를 볼 수 있다.

	// 타일은 8x8 픽셀로 구성됩니다. NES에서 픽셀은 2비트이며 이것은 곧 특정 팔레트의 4가지 다른 색상을 나타내는 인덱스이다. (값이 0,1,2,3)
	// 팔레트는 총 8개이며, 각 팔레트의 색상 "0"은 투명색으로 간주되는데, 전역적으로 사용되는 배경색의 메모리 주소를 "mirror"하기 때문이다. 
	// 이 메커니즘은 nes2C02.h의 주석, ppuRead() 및 ppuWrite()에 자세히 나온다.

	// Characters on NES
	// ~~~~~~~~~~~~~~~~~
	// NES는 2bit 픽셀을 사용하여 문자(character)를 저장한다. 
	// 하지만 이를 순차적으로 저장하는 것이 아닌 각각의 비트 평면(bit plane)에 저장한다.
	// 예를 들면:
	// LSB - Least Significant Bit (low bit)
	// MSB - Most Significant Bit (high bit)
	// 2-Bit Pixels       LSB Bit Plane     MSB Bit Plane
	// 0 0 0 0 0 0 0 0	  0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
	// 0 1 1 0 0 1 1 0	  0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
	// 0 1 2 0 0 2 1 0	  0 1 1 0 0 1 1 0   0 0 1 0 0 1 0 0
	// 0 0 0 0 0 0 0 0 =  0 0 0 0 0 0 0 0 + 0 0 0 0 0 0 0 0
	// 0 1 1 0 0 1 1 0	  0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
	// 0 0 1 1 1 1 0 0	  0 0 1 1 1 1 0 0   0 0 0 0 0 0 0 0
	// 0 0 0 2 2 0 0 0	  0 0 0 1 1 0 0 0   0 0 0 1 1 0 0 0
	// 0 0 0 0 0 0 0 0	  0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
	//
	// 평면은 LSB의 8바이트와 MSB의 8바이트로 저장된다.

	// 16x16의 타일(스프라이트) 그리드 순회
	for (uint16_t nTileY = 0; nTileY < 16; nTileY++)
	{
		for (uint16_t nTileX = 0; nTileX < 16; nTileX++)
		{
			// 2D 타일 좌표를 pattern table memory의 1D offset으로 전환
			// 한 타일이 16바이트이고 16개의 타일이 한 row에 있으므로 width는 256 
			uint16_t nOffset = nTileY * 256 + nTileX * 16;

			// 타일(8x8 pixel)의 8개 pixel row 순회
			for (uint16_t row = 0; row < 8; row++)
			{
				// 각각의 row에 대해서 LSB와 MSB의 2 bit 픽셀값을 알아야함
				// 이 값이 64 bits의 LSB와 64 bits의 MSB에 저장되므로 
				// 같은 좌표에 대해서 LSB와 MSB는 메모리 상에서 8bytes씩 떨어져있다는 것을 의미
				// i: pattern table index
				// 0x1000: pattern table size(4KB)
				// nOffset: 위에서 계산한 tile offset 값
				// row: row offset
				// 0x0000: LSB plane의 row는 0x0000 - 0x0007
				// 0x0008: MSB plane의 row는 0x0008 - 0x0015
				uint8_t tile_lsb = ppuRead(i * 0x1000 + nOffset + row + 0x0000);
				uint8_t tile_msb = ppuRead(i * 0x1000 + nOffset + row + 0x0008);
				// LSB와 MSB plane에 대해서 하나의 행을 구했으므로 이것을 순회해서
				// 최종적인 픽셀 인덱스(픽셀값)를 구함
				// 타일의 8개 pixel column 순회
				for (uint16_t col = 0; col < 8; col++)
				{
					// 단순히 비트값을 더하는 것으로도 픽셀의 인덱스 값을 얻을 수 있지만 각 값의 LSB에만 관심이 있음
					// 왜냐하면...
					uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);
					// ... 각 열에 대해서 행을 1비트씩 오른쪽으로 이동시킬 것 이기 때문
					tile_lsb >>= 1; tile_msb >>= 1;

					// 이제 패턴 테이블의 특정 위치에 대한 위치와 NES 픽셀 값을 알게 되었고, 
					// 이를 화면의 색상과 스프라이트의 (x,y) 위치로 변환할 수 있다.
					sprPatternTable[i].SetPixel
					(
						// 행의 LSB를 먼저 사용하기 때문에 행을 오른쪽에서 왼쪽으로 효과적으로 읽고 있으므로 
						// 스크린은 왼쪽에서 오른쪽으로 그리기 때문에 행을 "뒤로" 그려야 한다.
						nTileX * 8 + (7 - col), 
						nTileY * 8 + row,
						GetColorFromPaletteRam(palette, pixel)
					);
				}
			}
		}
	}

	// 마지막으로 패턴 테이블을 나타내는 업데이트된 스프라이트를 반환한다.
	return sprPatternTable[i];
}

SDL_Color& nes2C02::GetColorFromPaletteRam(uint8_t palette, uint8_t pixel)
{
	// 지정된 팔레트와 픽셀 인덱스를 적절한 화면 색상으로 반환하는 편의 기능 함수
	// "0x3F00"       - Offset into PPU addressable range where palettes are stored
	// "palette << 2" - Each palette is 4 bytes in size
	// "pixel"        - Each pixel index is either 0, 1, 2 or 3
	// "& 0x3F"       - Stops us reading beyond the bounds of the palScreen array
	return palScreen[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
	// 여기서 tblPalette에 직접 접근하지 않고, 대신 ppuRead()가 PPU 버스에 부착된 별도의 작은 RAM에 주소를 매핑할 것
}

uint8_t nes2C02::cpuRead(uint16_t addr, bool rdonly)
{
    uint8_t data = 0x00;
	if (rdonly)
	{
		// PPU 레지스터를 읽으면 내용에 영향을 미칠 수 있으므로 
		// 읽기 전용 옵션은 상태를 변경하지 않고 PPU 상태를 검사하는 데 사용된다.
		// 이것은 디버그 모드에서만 실제로 사용됨.
		switch (addr)
		{
		case 0x0000: // Control
			data = control.reg;
			break;
		case 0x0001: // Mask
			data = mask.reg;
			break;
		case 0x0002: // Status
			data = status.reg;
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
	else
	{
		// 다양한 방법으로 읽혀지는 라이브 PPU 레지스터들
		// 일부 레지스터는 읽을 수 없으므로 0x00만 반환한다.
		switch (addr)
		{
			// Control - Not readable
		case 0x0000: break;

			// Mask - Not Readable
		case 0x0001: break;

			// Status
		case 0x0002:
			// 상태 레지스터에서 판독하면 회로의 여러 부분이 재설정되는 효과가 있다.
			// 상위 3개 비트만 상태 정보를 포함하지만 최근의 PPU 버스 트랜잭션을 나타내는 하위 5비트에서 일부 "노이즈"가 발생할 수 있다.
			// 몇몇 게임에서는 이 노이즈를 유효한 데이터로 사용할 수도 있다. (사용하면 안되는 것이라도)
			data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);

			// Clear the vertical blanking flag
			status.vertical_blank = 0;

			// Reset Loopy's Address latch flag
			address_latch = 0;
			break;

			// OAM Address
		case 0x0003: break;

			// OAM Data
		case 0x0004: break;

			// Scroll - Not Readable
		case 0x0005: break;

			// PPU Address - Not Readable
		case 0x0006: break;

			// PPU Data
		case 0x0007:
			// NameTable RAM의 읽기는 한 사이클 지연된다.
			// 그러므로 출력 버퍼는 이전 읽기 요청의 데이터를 담게 된다.
			data = ppu_data_buffer;
			// 다음에 사용할 버퍼를 업데이트한다.
			ppu_data_buffer = ppuRead(vram_addr.reg);
			// 그러나 주소가 팔레트 범위에 있으면 데이터가 지연되지 않으므로 즉시 반환된다.
			if (vram_addr.reg >= 0x3F00) data = ppu_data_buffer;
			// PPU 데이터의 모든 읽기는 제어(control) 레지스터에 설정된 모드에 따라 Name table 주소를 자동으로 증가시킨다.
			// 수직 모드로 설정하면 증분값이 32이므로 행 전체를 건너뛴다.
			// 수평 모드에서는 1씩 증가하여 다음 열로 이동한다.
			vram_addr.reg += (control.increment_mode ? 32 : 1);
			break;
		}
	}

    return data;
}

void nes2C02::cpuWrite(uint16_t addr, uint8_t data)
{
	switch (addr)
	{
	case 0x0000: // Control
		control.reg = data;
		tram_addr.nametable_x = control.nametable_x;
		tram_addr.nametable_y = control.nametable_y;
		break;
	case 0x0001: // Mask
		mask.reg = data;
		break;
	case 0x0002: // Status
		break;
	case 0x0003: // OAM Address
		break;
	case 0x0004: // OAM Data
		break;
	case 0x0005: // Scroll
		if (address_latch == 0)
		{
			// 스크롤(Scroll) 레지스터에 처음 쓰기는 픽셀 공간에 X 오프셋을 포함하고 있으며, 
			// 이 오프셋은 거친(coarse) x 값과 미세한(fine) x 값으로 나뉜다.
			fine_x = data & 0x07; // pixel offset
			tram_addr.coarse_x = data >> 3;
			address_latch = 1;
		}
		else
		{
			// 스크롤(Scroll) 레지스터에 처음 쓰기는 픽셀 공간에 Y 오프셋을 포함하고 있으며, 
			// 이 오프셋은 거친(coarse) y 값과 미세한(fine) y 값으로 나뉜다.
			tram_addr.fine_y = data & 0x07; // pixel offset
			tram_addr.coarse_y = data >> 3;
			address_latch = 0;
		}
		break;
	case 0x0006: // PPU Address
		if (address_latch == 0)
		{
			// PPU 주소 버스는 CPU가 ADDR 및 DATA 레지스터를 통해 액세스할 수 있다.
			// 이 레지스터에 처음 쓰는 것은 주소의 높은 바이트를 래칭하며, 두 번째는 낮은 바이트이다. 
			// 이것은 tram 레지스터에 임시로 저장된다.
			// PPU address bus can be accessed by CPU via the ADDR and DATA
			// registers. The fisrt write to this register latches the high byte
			// of the address, the second is the low byte. Note the writes
			// are stored in the tram register...
			tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);
			address_latch = 1;
		}
		else
		{
			// ...when a whole address has been written, the internal vram address
			// buffer is updated. Writing to the PPU is unwise during rendering
			// as the PPU will maintam the vram address automatically whilst
			// rendering the scanline position.
			tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
			vram_addr = tram_addr;
			address_latch = 0;
		}
		break;
	case 0x0007: // PPU Data
		ppuWrite(vram_addr.reg, data);
		// PPU 데이터의 모든 읽기는 제어(control) 레지스터에 설정된 모드에 따라 Name table 주소를 자동으로 증가시킨다.
		// 수직 모드로 설정하면 증분값이 32이므로 행 전체를 건너뛴다.
		// 수평 모드에서는 1씩 증가하여 다음 열로 이동한다.
		vram_addr.reg += (control.increment_mode ? 32 : 1);
		break;
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
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// Pattern Memory
		// addr & 0x1000 >> 12 - left or right side of table
		// aadr & 0x0FFF       - offset(calculated by masking the remaining bits of PPU address)
		data = tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF];
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		// Name Table Memory
		addr &= 0x0FFF;
		if (cart->mirror == Cartridge::MIRROR::VERTICAL)
		{
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = tblName[1][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = tblName[1][addr & 0x03FF];
		}
		else if (cart->mirror == Cartridge::MIRROR::HORIZONTAL)
		{
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0400 && addr <= 0x07FF)
				data = tblName[0][addr & 0x03FF];
			if (addr >= 0x0800 && addr <= 0x0BFF)
				data = tblName[1][addr & 0x03FF];
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				data = tblName[1][addr & 0x03FF];
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		// Palette Memory
		// 하위 5비트 mask로 팔레트의 인덱스값을 구함
		addr &= 0x001F;
		// 팔레트의 마지막 색깔들은 mirror of background color임
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		// 팔레트 테이블의 해당 주소로부터 값을 읽음
		data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
	}
    return data;
}

void nes2C02::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x3FFF;

	if (cart->ppuWrite(addr, data))
	{

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// Pattern Memory는 보통 ROM이지만 몇몇 카트리지에서는 RAM으로 사용하기 때문에 Write를 구현해둠
		tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3EFF)
	{
		addr &= 0x0FFF;
		if (cart->mirror == Cartridge::MIRROR::VERTICAL)
		{
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[1][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
		else if (cart->mirror == Cartridge::MIRROR::HORIZONTAL)
		{
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[0][addr & 0x03FF] = data;
			if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[1][addr & 0x03FF] = data;
			if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
	}
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
	{
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		tblPalette[addr] = data;
	}
}

void nes2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	this->cart = cartridge;
}

void nes2C02::reset()
{
	fine_x = 0x00;
	address_latch = 0x00;
	ppu_data_buffer = 0x00;
	scanline = 0;
	cycle = 0;
	bg_next_tile_id = 0x00;
	bg_next_tile_attrib = 0x00;
	bg_next_tile_lsb = 0x00;
	bg_next_tile_msb = 0x00;
	bg_shifter_pattern_lo = 0x0000;
	bg_shifter_pattern_hi = 0x0000;
	bg_shifter_attrib_lo = 0x0000;
	bg_shifter_attrib_hi = 0x0000;
	status.reg = 0x00;
	mask.reg = 0x00;
	control.reg = 0x00;
	vram_addr.reg = 0x0000;
	tram_addr.reg = 0x0000;
}

void nes2C02::clock()
{
	// Nesdev Wiki - PPU frame timing
	// https://wiki.nesdev.org/w/index.php?title=File:Ntsc_timing.png
	// cycle    - 1칸(1 pixel)
	// scanline - cycle로 이루어진 하나의 line
	// nes가 출력하는 스크린은 256x240 cycle로 구성되어있지만 실제 프레임은 341x261로 되어있음
	// 241 - 261의 보이지 않는 scanline을 Vertical blank라고 함
	// As we progress through scanlines and cycles, the PPU is effectively
	// a state machine going through the motions of fetching background 
	// information and sprite information, compositing them into a pixel
	// to be output.

	// The lambda functions (functions inside functions) contain the various
	// actions to be performed depending upon the output of the state machine
	// for a given scanline/cycle combination

	// ==============================================================================
	// Increment the background tile "pointer" one tile/column horizontally
	auto IncrementScrollX = [&]()
	{
		// Note: pixel perfect scrolling horizontally is handled by the 
		// data shifters. Here we are operating in the spatial domain of 
		// tiles, 8x8 pixel blocks.

		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites)
		{
			// A single name table is 32x30 tiles. As we increment horizontally
			// we may cross into a neighbouring nametable, or wrap around to
			// a neighbouring nametable
			if (vram_addr.coarse_x == 31)
			{
				// Leaving nametable so wrap address round
				vram_addr.coarse_x = 0;
				// Flip target nametable bit
				vram_addr.nametable_x = ~vram_addr.nametable_x;
			}
			else
			{
				// Staying in current nametable, so just increment
				vram_addr.coarse_x++;
			}
		}
	};

	// ==============================================================================
	// Increment the background tile "pointer" one scanline vertically
	auto IncrementScrollY = [&]()
	{
		// Incrementing vertically is more complicated. The visible nametable
		// is 32x30 tiles, but in memory there is enough room for 32x32 tiles.
		// The bottom two rows of tiles are in fact not tiles at all, they
		// contain the "attribute" information for the entire table. This is
		// information that describes which palettes are used for different 
		// regions of the nametable.

		// In addition, the NES doesnt scroll vertically in chunks of 8 pixels
		// i.e. the height of a tile, it can perform fine scrolling by using
		// the fine_y component of the register. This means an increment in Y
		// first adjusts the fine offset, but may need to adjust the whole
		// row offset, since fine_y is a value 0 to 7, and a row is 8 pixels high

		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites)
		{
			// If possible, just increment the fine y offset
			if (vram_addr.fine_y < 7)
			{
				vram_addr.fine_y++;
			}
			else
			{
				// If we have gone beyond the height of a row, we need to
				// increment the row, potentially wrapping into neighbouring
				// vertical nametables. Dont forget however, the bottom two rows
				// do not contain tile information. The coarse y offset is used
				// to identify which row of the nametable we want, and the fine
				// y offset is the specific "scanline"

				// Reset fine y offset
				vram_addr.fine_y = 0;

				// Check if we need to swap vertical nametable targets
				if (vram_addr.coarse_y == 29)
				{
					// We do, so reset coarse y offset
					vram_addr.coarse_y = 0;
					// And flip the target nametable bit
					vram_addr.nametable_y = ~vram_addr.nametable_y;
				}
				else if (vram_addr.coarse_y == 31)
				{
					// In case the pointer is in the attribute memory, we
					// just wrap around the current nametable
					vram_addr.coarse_y = 0;
				}
				else
				{
					// None of the above boundary/wrapping conditions apply
					// so just increment the coarse y offset
					vram_addr.coarse_y++;
				}
			}
		}
	};

	// ==============================================================================
	// Transfer the temporarily stored horizontal nametable access information
	// into the "pointer". Note that fine x scrolling is not part of the "pointer"
	// addressing mechanism
	auto TransferAddressX = [&]()
	{
		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites)
		{
			vram_addr.nametable_x = tram_addr.nametable_x;
			vram_addr.coarse_x = tram_addr.coarse_x;
		}
	};

	// ==============================================================================
	// Transfer the temporarily stored vertical nametable access information
	// into the "pointer". Note that fine y scrolling is part of the "pointer"
	// addressing mechanism
	auto TransferAddressY = [&]()
	{
		// Ony if rendering is enabled
		if (mask.render_background || mask.render_sprites)
		{
			vram_addr.fine_y = tram_addr.fine_y;
			vram_addr.nametable_y = tram_addr.nametable_y;
			vram_addr.coarse_y = tram_addr.coarse_y;
		}
	};


	// ==============================================================================
	// Prime the "in-effect" background tile shifters ready for outputting next
	// 8 pixels in scanline.
	auto LoadBackgroundShifters = [&]()
	{
		// Each PPU update we calculate one pixel. These shifters shift 1 bit along
		// feeding the pixel compositor with the binary information it needs. Its
		// 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
		// and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this means
		// the required bit is always the MSB of the shifter. However, "fine x" scrolling
		// plays a part in this too, whcih is seen later, so in fact we can choose
		// any one of the top 8 bits.
		bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

		// Attribute bits do not change per pixel, rather they change every 8 pixels
		// but are synchronised with the pattern shifters for convenience, so here
		// we take the bottom 2 bits of the attribute word which represent which 
		// palette is being used for the current 8 pixels and the next 8 pixels, and 
		// "inflate" them to 8 bit words.
		bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
		bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
	};


	// ==============================================================================
	// Every cycle the shifters storing pattern and attribute information shift
	// their contents by 1 bit. This is because every cycle, the output progresses
	// by 1 pixel. This means relatively, the state of the shifter is in sync
	// with the pixels being drawn for that 8 pixel section of the scanline.
	auto UpdateShifters = [&]()
	{
		if (mask.render_background)
		{
			// Shifting background tile pattern row
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;

			// Shifting palette attributes by 1
			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}
	};
	// All but 1 of the scanlines is visible to the user. The pre-render scanline	
	// at -1, is used to configure the "shifters" for the first visible scanline, 0.
	if (scanline >= -1 && scanline < 240)
	{
		if (scanline == 0 && cycle == 0)
		{
			// "Odd Frame" cycle skip
			cycle = 1;
		}
		if (scanline == -1 && cycle == 1)
		{
			// 새로운 프레임의 시작점일 때(vertical blank를 빠져나왔을 때, 스크린의 좌상단) 플래그를 0으로 설정
			// Effectively start of new frame, so clear vertical blank flag
			status.vertical_blank = 0;
		}
		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338))
		{
			UpdateShifters();
			// In these cycles we are collecting and working with visible data
			// The "shifters" have been preloaded by the end of the previous
			// scanline with the data for the start of this scanline. Once we
			// leave the visible region, we go dormant until the shifters are
			// preloaded for the next scanline.

			// Fortunately, for background rendering, we go through a fairly
			// repeatable sequence of events, every 2 clock cycles.
			switch ((cycle - 1) % 8)
			{
			case 0:
				// Load the current background tile pattern and attributes into the "shifter"
				LoadBackgroundShifters();
				// Fetch the next background tile ID
				// "(vram_addr.reg & 0x0FFF)" : Mask to 12 bits that are relevant
				// "| 0x2000"                 : Offset into nametable space on PPU address bus
				bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
				// Explanation:
				// The bottom 12 bits of the loopy register provide an index into
				// the 4 nametables, regardless of nametable mirroring configuration.
				// nametable_y(1) nametable_x(1) coarse_y(5) coarse_x(5)
				//
				// Consider a single nametable is a 32x32 array, and we have four of them
				//   0                1
				// 0 +----------------+----------------+
				//   |                |                |
				//   |                |                |
				//   |    (32x32)     |    (32x32)     |
				//   |                |                |
				//   |                |                |
				// 1 +----------------+----------------+
				//   |                |                |
				//   |                |                |
				//   |    (32x32)     |    (32x32)     |
				//   |                |                |
				//   |                |                |
				//   +----------------+----------------+
				//
				// This means there are 4096 potential locations in this array, which 
				// just so happens to be 2^12!
				break;
			case 2:
				// Fetch the next background tile attribute. OK, so this one is a bit
				// more involved :P

				// Recall that each nametable has two rows of cells that are not tile 
				// information, instead they represent the attribute information that
				// indicates which palettes are applied to which area on the screen.
				// Importantly (and frustratingly) there is not a 1 to 1 correspondance
				// between background tile and palette. Two rows of tile data holds
				// 64 attributes. Therfore we can assume that the attributes affect
				// 8x8 zones on the screen for that nametable. Given a working resolution
				// of 256x240, we can further assume that each zone is 32x32 pixels
				// in screen space, or 4x4 tiles. Four system palettes are allocated
				// to background rendering, so a palette can be specified using just
				// 2 bits. The attribute byte therefore can specify 4 distinct palettes.
				// Therefore we can even further assume that a single palette is
				// applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
				// that background tiles "share" a palette locally is the reason why
				// in some games you see distortion in the colours at screen edges.

				// As before when choosing the tile ID, we can use the bottom 12 bits of
				// the loopy register, but we need to make the implementation "coarser"
				// because instead of a specific tile, we want the attribute byte for a 
				// group of 4x4 tiles, or in other words, we divide our 32x32 address
				// by 4 to give us an equivalent 8x8 address, and we offset this address
				// into the attribute section of the target nametable.

				// Reconstruct the 12 bit loopy address into an offset into the
				// attribute memory

				// "(vram_addr.coarse_x >> 2)"        : integer divide coarse x by 4, 
				//                                      from 5 bits to 3 bits
				// "((vram_addr.coarse_y >> 2) << 3)" : integer divide coarse y by 4, 
				//                                      from 5 bits to 3 bits,
				//                                      shift to make room for coarse x

				// Result so far: YX00 00yy yxxx

				// All attribute memory begins at 0x03C0 within a nametable, so OR with
				// result to select target nametable, and attribute byte offset. Finally
				// OR with 0x2000 to offset into nametable address space on PPU bus.				
				bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11)
					| (vram_addr.nametable_x << 10)
					| ((vram_addr.coarse_y >> 2) << 3)
					| (vram_addr.coarse_x >> 2));

				// Right we've read the correct attribute byte for a specified address,
				// but the byte itself is broken down further into the 2x2 tile groups
				// in the 4x4 attribute zone.

				// The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
				//
				// +----+----+			    +----+----+
				// | TL | TR |			    | ID | ID |
				// +----+----+ where TL =   +----+----+
				// | BL | BR |			    | ID | ID |
				// +----+----+			    +----+----+
				//
				// Since we know we can access a tile directly from the 12 bit address, we
				// can analyse the bottom bits of the coarse coordinates to provide us with
				// the correct offset into the 8-bit word, to yield the 2 bits we are
				// actually interested in which specifies the palette for the 2x2 group of
				// tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
				// Likewise if "coarse x % 4" < 2 we are in the left half else right half.
				// Ultimately we want the bottom two bits of our attribute word to be the
				// palette selected. So shift as required...				
				if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
				if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
				bg_next_tile_attrib &= 0x03;
				break;

				// Compared to the last two, the next two are the easy ones... :P
			case 4:
				// Fetch the next background tile LSB bit plane from the pattern memory
				// The Tile ID has been read from the nametable. We will use this id to 
				// index into the pattern memory to find the correct sprite (assuming
				// the sprites lie on 8x8 pixel boundaries in that memory, which they do
				// even though 8x16 sprites exist, as background tiles are always 8x8).
				//
				// Since the sprites are effectively 1 bit deep, but 8 pixels wide, we 
				// can represent a whole sprite row as a single byte, so offsetting
				// into the pattern memory is easy. In total there is 8KB so we need a 
				// 13 bit address.

				// "(control.pattern_background << 12)"  : the pattern memory selector 
				//                                         from control register, either 0K
				//                                         or 4K offset
				// "((uint16_t)bg_next_tile_id << 4)"    : the tile id multiplied by 16, as
				//                                         2 lots of 8 rows of 8 bit pixels
				// "(vram_addr.fine_y)"                  : Offset into which row based on
				//                                         vertical scroll offset
				// "+ 0"                                 : Mental clarity for plane offset
				// Note: No PPU address bus offset required as it starts at 0x0000
				bg_next_tile_lsb = ppuRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 0);

				break;
			case 6:
				// Fetch the next background tile MSB bit plane from the pattern memory
				// This is the same as above, but has a +8 offset to select the next bit plane
				bg_next_tile_msb = ppuRead((control.pattern_background << 12)
					+ ((uint16_t)bg_next_tile_id << 4)
					+ (vram_addr.fine_y) + 8);
				break;
			case 7:
				// Increment the background tile "pointer" to the next tile horizontally
				// in the nametable memory. Note this may cross nametable boundaries which
				// is a little complex, but essential to implement scrolling
				IncrementScrollX();
				break;
			}
		}
		// End of a visible scanline, so increment downwards...
		if (cycle == 256)
		{
			IncrementScrollY();
		}

		//...and reset the x position
		if (cycle == 257)
		{
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of scanline
		if (cycle == 338 || cycle == 340)
		{
			bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305)
		{
			// End of vertical blank period so reset the Y address ready for rendering
			TransferAddressY();
		}
	}

	if (scanline == 240)
	{
		// Post Render Scanline - Do Nothing!
	}

	if (scanline >= 241 && scanline < 261)
	{
		if (scanline == 241 && cycle == 1)
		{
			// 프레임의 끝이므로 vertical blank flag를 1로 설정
			status.vertical_blank = 1;

			// vertical blank로 들어갈 때 제어(control) 레지스터에서 NMI를 설정하라고 하면 그렇게 한다.(인터럽트를 발생시킨다)
			// 이를 통해 CPU에 렌더링이 완료되었음을 알림으로써 눈에 보이는 부분을 그리지 않고 있다는 것을 알고
			// PPU와 함께 작업을 수행할 수 있다.
			if (control.enable_nmi)
				nmi = true;
		}
	}
	// Composition - We now have background pixel information for this cycle
	// At this point we are only interested in background

	uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

	// We only render backgrounds if the PPU is enabled to do so. Note if 
	// background rendering is disabled, the pixel and palette combine
	// to form 0x00. This will fall through the colour tables to yield
	// the current background colour in effect
	if (mask.render_background)
	{
		// Handle Pixel Selection by selecting the relevant bit
		// depending upon fine x scolling. This has the effect of
		// offsetting ALL background rendering by a set number
		// of pixels, permitting smooth scrolling
		uint16_t bit_mux = 0x8000 >> fine_x;

		// Select Plane pixels by extracting from the shifter 
		// at the required location. 
		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

		// Combine to form pixel index
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		// Get palette
		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}


	// Now we have a final pixel colour, and a palette for this cycle
	// of the current scanline. Let's at long last, draw that ^&%*er :P

	sprScreen.SetPixel(cycle - 1, scanline, GetColorFromPaletteRam(bg_palette, bg_pixel));

	// random noise for test
	// sprScreen.SetPixel(cycle - 1, scanline, palScreen[(rand() % 2) ? 0x3F : 0x30]);

	cycle++;
	// hard-coded boundaries
	// scanline = 341 pixel, row
	// cycle = 1 pixel, column
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
