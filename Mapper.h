#pragma once
#include <cstdint>
class Mapper
{
    public:
    // Prgraom ROM의 Bank 수, Charcter ROM의 Bank 수
    Mapper(uint8_t prgBanks, uint8_t chrBanks); 
    ~Mapper();
    
    public:
    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;
    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;



    protected:
    uint8_t nPRGBanks = 0;
    uint8_t nCHRBanks = 0;
};

