#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>

int main(int argc, char **argv)
{
  std::string infile("/mnt/nvme/dunecet/dunedaq/slr1-320-data.bin");
  uint32_t framebuf[120];

  FILE *f = fopen(infile.data(),"r");

  for (size_t iframe=0; iframe < 1000; ++iframe)
    {
      bool found=false;
      int nsearch = 0;
      while (!found)
	{
	  ++nsearch;
	  //fread(framebuf, sizeof(uint32_t), 120, f);
	  framebuf[0] = framebuf[1];
	  framebuf[1] = framebuf[2];
	  fread( &(framebuf[2]), sizeof(uint32_t), 1, f);
	  if (framebuf[2] == 0xbabeface)
	    {
	      fread( &(framebuf[3]),sizeof(uint32_t), 115, f);
	      found = true;
	    }
	}

      int slot = (framebuf[1] & 0x7000) >> 12;
      int fiber = (framebuf[1] & 0x8000) >> 15;
      uint64_t timestamp= framebuf[4];
      timestamp <<= 32;
      timestamp += framebuf[3];

      if (iframe < 10)
	{
          std::cout << std::dec << "iframe: " << iframe << "  Marker: " << std::hex << framebuf[2] << std::endl;
          std::cout << std::dec << "   Slot: " << slot << " Fiber: " << fiber 
	  	<< " Timestamp: " << std::hex << timestamp <<std::endl;
	  std::cout << "  nsearch: " << nsearch << std::endl;
	}
      if (slot != 0 || fiber != 0)
	{
	  std::cout << std::dec << "Found a slot and a fiber not equal to 0, 0: " << slot << " " << fiber << " " << iframe << std::endl;
	}
    }
}
