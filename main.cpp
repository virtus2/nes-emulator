#include "Bus.h"
#include "nes6502.h"
#include "Engine.h"

class Demo : public Engine
{
    Bus nes;
    std::map<uint16_t, std::string> mapAsm;
    std::string hex(uint32_t n, uint8_t d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    }
};

int main(int argc, char* args[])
{
    Demo demo;
    demo.Init(680, 480);
    demo.Start();
    return 0;
}
