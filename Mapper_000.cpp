#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
}

Mapper_000::~Mapper_000()
{
}

bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
    // if PRGROM is 16KB (nPRGBanks == 1)
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
	//     0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
	// if PRGROM is 32KB (nPRGBanks > 1)
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF	
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }

    return false;
}

bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
    // Mapper 0에서는 PPU에 mapping이 필요하지 않음
    // PPU Address Bus          CHR ROM
    // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
    // PPU ROM에다 Write하는건 이상함
    // 하지만 카트리지가 해당 위치에 ROM을 갖고 있는게 아닌 
    // RAM을 갖고 있을 수 있음(pattern memory를 write하고 싶은 경우)
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        if (nCHRBanks == 0)
        {
            // Treat as RAM
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}
