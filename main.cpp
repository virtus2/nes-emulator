#include <iostream>
#include <sstream>

#include "Bus.h"
#include "nes6502.h"

#define VTS_ENGINE
#include "Engine.h"

class Demo : public vts::Engine
{
	public:
    Bus nes;
	std::shared_ptr<Cartridge> cart;
	bool bEmulationRun = false;
	float fResidualTime = 0.0f;

	private:
    std::map<uint16_t, std::string> mapAsm;
	
    std::string hex(uint32_t n, uint8_t d)
	{
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    }
    void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
    {
        int nRamX = x, nRamY = y;
        for (int row = 0; row < nRows; row++)
        {
            std::string sOffset = "$" + hex(nAddr, 4) + ":";
            for (int col = 0; col < nColumns; col++)
            {
                sOffset += " " + hex(nes.cpuRead(nAddr, true), 2);
                nAddr += 1;
            }
            DrawString(nRamX, nRamY, sOffset);
            nRamY += 10;
        }
    }

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", vts::WHITE);
		DrawString(x + 64, y, "N", nes.cpu.status & nes6502::N ? vts::WHITE : vts::RED);
		DrawString(x + 80, y, "V", nes.cpu.status & nes6502::V ? vts::GREEN : vts::RED);
		DrawString(x + 96, y, "-", nes.cpu.status & nes6502::U ? vts::GREEN : vts::RED);
		DrawString(x + 112, y, "B", nes.cpu.status & nes6502::B ? vts::GREEN : vts::RED);
		DrawString(x + 128, y, "D", nes.cpu.status & nes6502::D ? vts::GREEN : vts::RED);
		DrawString(x + 144, y, "I", nes.cpu.status & nes6502::I ? vts::GREEN : vts::RED);
		DrawString(x + 160, y, "Z", nes.cpu.status & nes6502::Z ? vts::GREEN : vts::RED);
		DrawString(x + 178, y, "C", nes.cpu.status & nes6502::C ? vts::GREEN : vts::RED);
		DrawString(x, y + 10, "PC: $" + hex(nes.cpu.pc, 4));
		DrawString(x, y + 20, "A: $" + hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(nes.cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, vts::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(nes.cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		// Load the cartridge
		cart = std::make_shared<Cartridge>("nestest.nes");
		if (!cart->ImageValid())
		{
			printf("Cartridge Not Valid!\n");
			return false;
		}

		// Insert into NES
		nes.insertCartridge(cart);

		// Extract dissassembly
		mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

		// Reset NES
		nes.reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(vts::DARK_BLUE);

		if (bEmulationRun)
		{
			if (fResidualTime > 0.0f)
				fResidualTime -= fElapsedTime;
			else
			{
				fResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do { nes.clock(); } while (!nes.ppu.frame_complete);
				nes.ppu.frame_complete = false;
			}
		}
		else
		{
			// Emulate code step-by-step
			if (GetKey(SDLK_c).bPressed)
			{
				// Clock enough times to execute a whole CPU instruction
				do { nes.clock(); } while (!nes.cpu.complete());
				// CPU clock runs slower than system clock, so it may be
				// complete for additional system clock cycles. Drain
				// those out
				do { nes.clock(); } while (nes.cpu.complete());
			}

			// Emulate one whole frame
			if (GetKey(SDLK_f).bPressed)
			{
				// Clock enough times to draw a single frame
				do { nes.clock(); } while (!nes.ppu.frame_complete);
				// Use residual clock cycles to complete current instruction
				do { nes.clock(); } while (!nes.cpu.complete());
				// Reset frame completion flag
				nes.ppu.frame_complete = false;
			}
		}


		if (GetKey(SDLK_SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(SDLK_r).bPressed) nes.reset();

		DrawCpu(516, 2);
		DrawCode(516, 72, 26);

		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
		return true;
	}
};

int main(int argc, char* args[])
{
    Demo demo;
    demo.Init(780, 480);
    demo.Start();
    return 0;
}
