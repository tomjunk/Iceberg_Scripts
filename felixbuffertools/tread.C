#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <vector>

void procframe(FILE *f);

int main(int argc, char **argv)
{
  std::vector<std::string> infiles;
  infiles.push_back("/mnt/nvme/dunecet/22nd-march/run2/slr1-0-data.bin");
  infiles.push_back("/mnt/nvme/dunecet/22nd-march/run2/slr1-64-data.bin");
  infiles.push_back("/mnt/nvme/dunecet/22nd-march/run2/slr1-128-data.bin");
  infiles.push_back("/mnt/nvme/dunecet/22nd-march/run2/slr1-256-data.bin");
  infiles.push_back("/mnt/nvme/dunecet/22nd-march/run2/slr1-320-data.bin");
  std::vector<FILE*> fileptrs;

  for (size_t ifile=0; ifile<infiles.size(); ++ifile)
    {
      fileptrs.push_back(fopen(infiles.at(ifile).data(),"r"));
    }

  for (size_t iframe=0; iframe<1000; ++iframe)
    {
      std::cout << "Frame #" << iframe << " -----------------" << std::endl;
      for (size_t ifile=0; ifile<infiles.size(); ++ifile)
	{
	  procframe(fileptrs.at(ifile));
	}
    }

  for (size_t ifile=0; ifile<infiles.size(); ++ifile)
    {
      fclose(fileptrs.at(ifile));
    }

}

void procframe(FILE *infileptr)
{
  uint32_t framebuf[117];

  fread(framebuf, sizeof(uint32_t), 117, infileptr);
  if (feof(infileptr)) 
    {exit(0);}
  int slot = (framebuf[0] & 0x7000) >> 12;
  int fiber = (framebuf[0] & 0x8000) >> 15;
  uint64_t timestamp= framebuf[3];
  timestamp <<= 32;
  timestamp += framebuf[2];
  std::cout << std::dec << "   Slot: " << slot << " Fiber: " << fiber 
	    << " Timestamp: " << std::dec << timestamp <<std::endl;
}
