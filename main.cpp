#include <iostream>
#include <sstream>
#include <deque>

#include "Bus.h"
#include "nes6502.h"

#define VTS_ENGINE
#include "Engine.h"

class Demo : public vts::Engine
{
	public:
	Demo() { sAppName = "nes emulator"; }

	private:
    Bus nes;
	std::shared_ptr<Cartridge> cart;
	bool bEmulationRun = false;
	float fResidualTime = 0.0f;

	uint8_t nSelectedPalette = 0x00;

	std::list<uint16_t> audio[4];
	float fAccumulatedTime = 0.0f;

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

	// This function is called by the underlying sound hardware
	// which runs in a different thread. It is automatically
	// synchronised with the sample rate of the sound card, and
	// expects a single "sample" to be returned, whcih ultimately
	// makes its way to your speakers, and then your ears, for that
	// lovely 8-bit bliss... but, that means we've some thread
	// handling to deal with, since we want both the PGE thread
	// and the sound system thread to interact with the emulator.

	static Demo* pInstance; // Static variable that will hold a pointer to "this"
	static float SoundOut(int nChannel, float fGlobalTime, float fTimeStep)
	{
		if (nChannel == 0)
		{
			// repeatedly clocking until there is a sample ready in real-time
			while (!(pInstance->nes.clock())) {}; 
			return static_cast<float>(pInstance->nes.dAudioSample);
		}
		else
			return 0.0f;
	}

	bool OnUserCreate()
	{
		// Load the cartridge
		cart = std::make_shared<Cartridge>("ROMs/smb.nes");
		if (!cart->ImageValid())
		{
			printf("Cartridge Not Valid!\n");
			return false;
		}

		// Insert into NES
		nes.insertCartridge(cart);

		// Extract dissassembly
		mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 120; j++)
			{
				audio[i].push_back(0);
			}
		}

		// Reset NES
		nes.reset();

		pInstance = this;
		nes.SetSampleFrequency(44100);
		vts::Sound::InitializeAudio(44100, 1, 8, 512);
		vts::Sound::SetUserSynthFunction(SoundOut);


		SDL_AudioSpec wavSpec;
		SDL_memset(&wavSpec, 0, sizeof(wavSpec)); /* or SDL_zero(want) */

		wavSpec.userdata = nullptr;
		wavSpec.format = AUDIO_S16;
		wavSpec.channels = 2;
		wavSpec.samples = 2048;
		
		if (SDL_OpenAudio(&wavSpec, NULL) < 0)
		{
			fprintf(stderr, "Could not open audio: %s\n", SDL_GetError());
		}
		SDL_PauseAudio(0);
		return true;
	}

	bool OnUserDestroy()
	{
		vts::Sound::DestroyAudio();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		EmulatorUpdateWithoutAudio(fElapsedTime);
		// EmulatorUpdateWithAudio(fElapsedTime);
		return true;
	}

	bool EmulatorUpdateWithAudio(float fElapsedTime)
	{
		// Sample audio channel output roughly once per frame
		fAccumulatedTime += fElapsedTime;
		if (fAccumulatedTime >= 1.0f / 60.0f)
		{
			fAccumulatedTime -= (1.0f / 60.0f);
			audio[0].pop_front();
			audio[0].push_back(nes.apu.pulse1_visual);
			audio[1].pop_front();
			audio[1].push_back(nes.apu.pulse2_visual);
			audio[2].pop_front();
			audio[2].push_back(nes.apu.noise_visual);
		}

		Clear(vts::DARK_BLUE);

		// Handle input for controller in port #1
		nes.controller[0] = 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_X).bHeld ? 0x80 : 0x00; // A Button
		nes.controller[0] |= GetKey(SDL_SCANCODE_Z).bHeld ? 0x40 : 0x00; // B Button
		nes.controller[0] |= GetKey(SDL_SCANCODE_A).bHeld ? 0x20 : 0x00; // Select
		nes.controller[0] |= GetKey(SDL_SCANCODE_S).bHeld ? 0x10 : 0x00; // Start
		nes.controller[0] |= GetKey(SDL_SCANCODE_UP).bHeld ? 0x08 : 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_DOWN).bHeld ? 0x04 : 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_LEFT).bHeld ? 0x02 : 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_RIGHT).bHeld ? 0x01 : 0x00;

		if (GetKey(SDL_SCANCODE_R).bPressed) nes.reset();
		if (GetKey(SDL_SCANCODE_P).bPressed) (++nSelectedPalette) &= 0x07;

		DrawCpu(516, 2);
		DrawCode(516, 72, 26);
		/*
		// Draw OAM Contents (first 26 out of 64) ======================================
		for (int i = 0; i < 26; i++)
		{
			std::string s = hex(i, 2) + ": (" + std::to_string(nes.ppu.pOAM[i * 4 + 3])
				+ ", " + std::to_string(nes.ppu.pOAM[i * 4 + 0]) + ") "
				+ "ID: " + hex(nes.ppu.pOAM[i * 4 + 1], 2) +
				+" AT: " + hex(nes.ppu.pOAM[i * 4 + 2], 2);
			DrawString(516, 72 + i * 10, s);
		}
		*/

		// Draw Palettes & Pattern Tables
		const int nSwatchSize = 6;
		for (int p = 0; p < 8; p++) // For each palette
			for (int s = 0; s < 4; s++) // For each index
				FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 343,
					nSwatchSize, nSwatchSize, nes.ppu.GetColorFromPaletteRam(p, s));

		// Draw selection reticule around selected palette
		DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 342, (nSwatchSize * 4), nSwatchSize, vts::WHITE);

		// Generate Pattern Tables
		DrawSprite(516, 350, &nes.ppu.GetPatternTable(0, nSelectedPalette));
		DrawSprite(648, 350, &nes.ppu.GetPatternTable(1, nSelectedPalette));

		// Draw rendered output
		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
		return true;
	}

	bool EmulatorUpdateWithoutAudio(float fElapsedTime)
	{
		Clear(vts::DARK_BLUE);

		// Handle input for controller in port #1
		nes.controller[0] = 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_X).bHeld ? 0x80 : 0x00; // A Button
		nes.controller[0] |= GetKey(SDL_SCANCODE_Z).bHeld ? 0x40 : 0x00; // B Button
		nes.controller[0] |= GetKey(SDL_SCANCODE_A).bHeld ? 0x20 : 0x00; // Select
		nes.controller[0] |= GetKey(SDL_SCANCODE_S).bHeld ? 0x10 : 0x00; // Start
		nes.controller[0] |= GetKey(SDL_SCANCODE_UP).bHeld ? 0x08 : 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_DOWN).bHeld ? 0x04 : 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_LEFT).bHeld ? 0x02 : 0x00;
		nes.controller[0] |= GetKey(SDL_SCANCODE_RIGHT).bHeld ? 0x01 : 0x00;

		if (GetKey(SDL_SCANCODE_SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(SDL_SCANCODE_R).bPressed) nes.reset();
		if (GetKey(SDL_SCANCODE_P).bPressed) (++nSelectedPalette) &= 0x07;

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
			if (GetKey(SDL_SCANCODE_C).bPressed)
			{
				// Clock enough times to execute a whole CPU instruction
				do { nes.clock(); } while (!nes.cpu.complete());
				// CPU clock runs slower than system clock, so it may be
				// complete for additional system clock cycles. Drain
				// those out
				do { nes.clock(); } while (nes.cpu.complete());
			}

			// Emulate one whole frame
			if (GetKey(SDL_SCANCODE_F).bPressed)
			{
				// Clock enough times to draw a single frame
				do { nes.clock(); } while (!nes.ppu.frame_complete);
				// Use residual clock cycles to complete current instruction
				do { nes.clock(); } while (!nes.cpu.complete());
				// Reset frame completion flag
				nes.ppu.frame_complete = false;
			}
		}

		DrawCpu(516, 2);
		DrawCode(516, 72, 26);
		/*
		// Draw OAM Contents (first 26 out of 64) ======================================
		for (int i = 0; i < 26; i++)
		{
			std::string s = hex(i, 2) + ": (" + std::to_string(nes.ppu.pOAM[i * 4 + 3])
				+ ", " + std::to_string(nes.ppu.pOAM[i * 4 + 0]) + ") "
				+ "ID: " + hex(nes.ppu.pOAM[i * 4 + 1], 2) +
				+" AT: " + hex(nes.ppu.pOAM[i * 4 + 2], 2);
			DrawString(516, 72 + i * 10, s);
		}
		*/

		// Draw Palettes & Pattern Tables
		const int nSwatchSize = 6;
		for (int p = 0; p < 8; p++) // For each palette
			for (int s = 0; s < 4; s++) // For each index
				FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 343,
					nSwatchSize, nSwatchSize, nes.ppu.GetColorFromPaletteRam(p, s));

		// Draw selection reticule around selected palette
		DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 342, (nSwatchSize * 4), nSwatchSize, vts::WHITE);

		// Generate Pattern Tables
		DrawSprite(516, 350, &nes.ppu.GetPatternTable(0, nSelectedPalette));
		DrawSprite(648, 350, &nes.ppu.GetPatternTable(1, nSelectedPalette));

		// Draw rendered output
		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
		return true;
	}
};


// Provide implementation for our static pointer
Demo* Demo::pInstance = nullptr;

int main(int argc, char* args[])
{
    Demo demo;
    demo.Init(780, 480);
    demo.Start();
    return 0;
}
