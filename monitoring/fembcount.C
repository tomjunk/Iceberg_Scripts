#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "TFile.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "lardataobj/RawData/RawDigit.h"
#include <vector>
#include <set>
#include "TString.h"

using namespace art;
//using namespace std;


void fembcount(
	       std::string const& filename="iceberg_r005777_sr01_20200719T180114_1_dl1_decode.root", 
	       std::string const& inputtag="tpcrawdecoder:daq")
{

  size_t evcounter=0;
  std::set<size_t> fembcountset;
  std::map<size_t,size_t> fcmap;

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  size_t totevents=0;
  int run_number;

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) 
    {
      run_number = ev.eventAuxiliary().run();
      totevents++;
      auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
      int fc = 0;
      int fakefc = 0;
      if (!rawdigits.empty())
	{
	  fc = rawdigits.size()/128;
	  int nfakechans=0;
	  for (size_t i=0;i<rawdigits.size(); ++i)
	    {
	      int nrun = 0;
	      size_t nticks=rawdigits[i].Samples();
	      if (nticks>20) nticks=25;
	      for (size_t j=1;j<nticks; ++j)
		{
		  if (rawdigits[i].ADC(j) == rawdigits[i].ADC(j-1) + 8) nrun++;
		}
	      //std::cout << "nrun: " << nrun << std::endl;
	      if (nrun>18) nfakechans++;
	    }
	  fakefc = nfakechans/128;
	}

      fc = 10000*fc + fakefc;
      if (fembcountset.count(fc) == 0)
	{
	  fcmap[fc] = 1;
	}
      else
	{
	  fcmap[fc]++;
	}
      fembcountset.insert(fc);
    }
  std::cout << "Run number: " << run_number << " has " << std::endl;
  for (const auto& mi : fcmap)
    {
      std::cout << mi.second << " event(s) with " << mi.first/10000 << " FEMBs (" <<
	mi.first % 10000 << " fake, " << mi.first/10000 - mi.first % 10000 << " non-fake)" << std::endl;
    }
  
}


