#include "Cartridge.h"

Cartridge::Cartridge(const std::string& sFileName)
{
    // iNES Format header
    // https://wiki.nesdev.org/w/index.php/INES#iNES_file_format
    struct sHeader
    {
        char name[4];
        uint8_t prg_rom_chunks;
        uint8_t chr_rom_chunks;
        uint8_t mapper1;
        uint8_t mapper2;
        uint8_t prg_ram_size;
        uint8_t tv_system1;
        uint8_t tv_system2;
        char unused[5];
    }header;

    bImageValid = false;
    // 바이너리 모드로 파일을 열음
    std::ifstream ifs;
    ifs.open(sFileName, std::ifstream::binary);
    if (ifs.is_open())
    {
        // 파일의 헤더를 읽음 
        ifs.read((char*)&header, sizeof(sHeader));
        // 다음 512바이트가 "training information"일 경우 건너뜀
        if (header.mapper1 & 0x04)
            ifs.seekg(512, std::ios_base::cur);
        // Mapper ID를 결정함
        nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
        // 카트리지의 미러링 방식을 결정함(PPU의 Name table)
        mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;
        // 파일 포맷을 읽음
        uint8_t nFileType = 1;

        if (nFileType == 0)
        {

        }

        if (nFileType == 1)
        {
            nPRGBanks = header.prg_rom_chunks;
            vPRGMemory.resize(nPRGBanks * 16384);
            ifs.read((char*)vPRGMemory.data(), vPRGMemory.size());

            nCHRBanks = header.chr_rom_chunks;
            vCHRMemory.resize(nCHRBanks * 8192);
            ifs.read((char*)vCHRMemory.data(), vCHRMemory.size());
        }

        if (nFileType == 2)
        {

        }
        // Mapper ID에 따라 해당하는 Mapper를 불러옴
        switch (nMapperID)
        {
        case 0: pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks); break;
        }

        bImageValid = true;
        ifs.close();
    }
}

Cartridge::~Cartridge()
{
}

bool Cartridge::ImageValid()
{
    return bImageValid;
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t & data)
{
    uint32_t mapped_addr = 0;
    if (pMapper->cpuMapRead(addr, mapped_addr))
    {
        data = vPRGMemory[mapped_addr];
        return true;
    }
    else
        return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    if (pMapper->cpuMapRead(addr, mapped_addr))
    {
        vPRGMemory[mapped_addr] = data;
        return true;
    }
    else
        return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t & data)
{
    uint32_t mapped_addr = 0;
    if (pMapper->ppuMapRead(addr, mapped_addr))
    {
        data = vCHRMemory[mapped_addr];
        return true;
    }
    else
        return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
    uint32_t mapped_addr = 0;
    if (pMapper->ppuMapRead(addr, mapped_addr))
    {
        vCHRMemory[mapped_addr] = data;
        return true;
    }
    else
        return false;
}
