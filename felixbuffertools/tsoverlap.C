#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "TFile.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/RDTimeStamp.h"

using namespace art;
using namespace std;

// arguments:  decoded spy-buffer root file, and one of the FELIX root files, tag name for getting timestamps
// from the spy-buffer decoded rootfile


void tsoverlap(std::string const& filename="iceberg_r008956_sr01_20210323T170134_1_dl1_decode.root", 
	     std::string const& felixfilename="slr1-0-data.bin", 
	     std::string const& inputtag="tpcrawdecoder:daq")
{

  ULong64_t firstFELIXts=0;
  ULong64_t lastFELIXts=0;

  FILE *fptr = fopen(felixfilename.data(),"r");
  uint32_t framebuf[117];
    fread(framebuf, sizeof(uint32_t), 117, fptr);
  if (feof(fptr)) 
    {exit(0);}
  firstFELIXts = framebuf[3];
  firstFELIXts <<= 32;
  firstFELIXts += framebuf[2];
  fclose(fptr);
  
  std::cout << "First FELIX Timestamp: " << std::dec << firstFELIXts <<std::endl;

  struct stat st;
  stat(felixfilename.data(),&st);
  size_t felixsize = st.st_size;

  lastFELIXts = firstFELIXts + 32*felixsize/(117*4);

  std::cout << "Last FELIX Timestamp: " << std::dec << firstFELIXts <<std::endl;
  std::cout << "FELIX run has: " << felixsize/(117*4) << " frames." << std::endl;

  // now get timestamps for the spy buffer file

  InputTag rdtimestamp_tag{ inputtag };
  vector<string> filenames(1, filename);

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    auto const& rdtimestamps = *ev.getValidHandle<vector<raw::RDTimeStamp>>(rdtimestamp_tag);
    if (!rdtimestamps.empty())
      {
	const size_t nrdtimestamps = rdtimestamps.size();
	if (nrdtimestamps == 0) continue;
	ULong64_t timestamp = rdtimestamps[0].GetTimeStamp();

	auto const &evaux = ev.eventAuxiliary();
	int runno = evaux.run();
	int event = evaux.event();

	TString outflag=" Event is not in FELIX run";
	if (timestamp > firstFELIXts && timestamp < lastFELIXts)
	  {
	    outflag = " Event is in FELIX run";
	  }
	std::cout << "Spy Run, event, timestamp: " << runno << " " << event << " " << timestamp << outflag << std::endl;
      }
  }
}
