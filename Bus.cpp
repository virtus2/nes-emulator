#include "Bus.h"

Bus::Bus()
{
	// CPU를 버스에 연결
	cpu.ConnectBus(this);

	// RAM의 값을 모두 0으로 초기화
	for (auto& i : cpuRam) i = 0x00;
}


Bus::~Bus()
{
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (cart->cpuWrite(addr, data))
	{
		// 카트리지가 모든 write 작업을 확인하고 
		// 필요한 경우 Bus transaction을 가로챌 수 있음
		// 이를 통해 카트리지가 다른 물리적 장치와의 트랜잭션을 전환하거나
		// 주소를 다른 데이터에 매핑할 수 있음

	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// System RAM Address Range
		// 8KB 크기지만 mirroring된 2KB만 사용할 수 있음
		// bitwise AND를 사용하여 하위 11비트를 MASK함, addr % 2048과 같음
		// System memory at $0000-$07FF is mirrored at $0800-$0FFF, $1000-$17FF, and $1800-$1FFF 
		// attempting to access memory at, for example, $0173 is the same as accessing memory at $0973, $1173, or $1973.
		// https://wiki.nesdev.org/w/index.php?title=Mirroring
		cpuRam[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		// PPU Address Range
		// 8개의 레지스터를 갖고 있으며 이 범위 내에서 반복됨
		// bitwise AND를 사용하여 하위 3비트를 MASK, addr % 8과 같음
		// PPU I/O registers at $2000-$2007 are mirrored at $2008-$200F, $2010-$2017, $2018-$201F, and so forth, all the way up to $3FF8-$3FFF.
		// https://wiki.nesdev.org/w/index.php?title=Mirroring
		ppu.cpuWrite(addr & 0x0007, data);
	}
	else if (addr == 0x4014)
	{
		dma_page = data;
		dma_addr = 0x00;
		dma_transfer = true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		// "Lock In" controller state at this time
		controller_state[addr & 0x0001] = controller[addr & 0x0001];
	}
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;

	if (cart->cpuRead(addr, data))
	{
		// Cartridge Address Range
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// System RAM Address Range, mirrored every 2048
		data = cpuRam[addr & 0x07FF];
	} 
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		// PPU Address Range, mirrored every 8
		data = ppu.cpuRead(addr & 0x0007, bReadOnly);
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		// Read out the MSB of the controller status word
		data = (controller_state[addr & 0x0001] & 0x80) > 0;
		controller_state[addr & 0x0001] <<= 1;
	}
	return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	this->cart = cartridge;
	ppu.ConnectCartridge(cartridge);
}

void Bus::reset()
{
	cpu.reset();
	nSystemClockCounter = 0;
}

void Bus::clock()
{
	ppu.clock();
	// cpu clock은 ppu clock 보다 3배 느림
	if (nSystemClockCounter % 3 == 0)
	{
		// Is the system performing a DMA transfer form CPU memory to 
		// OAM memory on PPU?...
		if (dma_transfer)
		{
			// ...Yes! We need to wait until the next even CPU clock cycle
			// before it starts...
			if (dma_dummy)
			{
				// ...So hang around in here each clock until 1 or 2 cycles
				// have elapsed...
				if (nSystemClockCounter % 2 == 1)
				{
					// ...and finally allow DMA to start
					dma_dummy = false;
				}
			}
			else
			{
				// DMA can take place!
				if (nSystemClockCounter % 2 == 0)
				{
					// On even clock cycles, read from CPU bus
					dma_data = cpuRead(dma_page << 8 | dma_addr);
				}
				else
				{
					// On odd clock cycles, write to PPU OAM
					ppu.pOAM[dma_addr] = dma_data;
					// Increment the lo byte of the address
					dma_addr++;
					// If this wraps around, we know that 256
					// bytes have been written, so end the DMA
					// transfer, and proceed as normal
					if (dma_addr == 0x00)
					{
						dma_transfer = false;
						dma_dummy = true;
					}
				}
			}
		}
		else
		{
			// No DMA happening, the CPU is in control of its
			// own destiny. Go forth my friend and calculate
			// awesomeness for many generations to come...
			cpu.clock();
		}
	}

	// PPU로부터 NMI가 발생하면 플래그를 초기화시키고 CPU에게 NMI를 발생시킨다.
	if (ppu.nmi)
	{
		ppu.nmi = false;
		cpu.nmi();
	}



	nSystemClockCounter++;
}
