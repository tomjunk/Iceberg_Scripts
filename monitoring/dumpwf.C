#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "TFile.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TColor.h"
#include "TMath.h"
#include "TVectorT.h"
#include "TCanvas.h"
#include "lardataobj/RawData/RawDigit.h"
#include "TLegend.h"
#include <vector>
#include <set>
#include "Riostream.h"
#include "TString.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TROOT.h"

using namespace art;
using namespace std;


void dumpdigits(std::string const& infilename="iceberg_r008272_sr01_20210228T023206_1_dl1_decode.root", 
                std::string const& inputtag="tpcrawdecoder:daq",
		std::string const& outputfilename="run8272binary.dat")
{

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  FILE *outputfile = fopen(outputfilename,"w");
  
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
    if (!rawdigits.empty())
      {
	const size_t nrawdigits = rawdigits.size();
	auto const& evaux = ev.eventAuxiliary();
	int runno = evaux.run();
	int subrunno = evaux.subRun();
	int eventno = evaux.event();
	fwrite(&runno, sizeof(runno), outputfile);
	fwrite(&subrunno, sizeof(subrunno), outputfile);
	fwrite(&eventno, sizeof(eventno), outputfile);
	fwrite(&nrawdigits, sizeof(nrawdigits), outputfile);
	    
	for (size_t ichan=0;ichan<nrawdigits; ++ichan) 
	  {
	    size_t ic = rawdigits[ichan].Channel();
	    fwrite(&ic, sizeof(ic), outputfile);
	    size_t nticks = rawdigits[ichan].Samples();
	    fwrite(&nticks, sizeof(nticks), outputfile);
	    fwrite(rawdigits[ichan].ADCs(),nticks * sizeof(short), outputfile);
	  }
      }
  }
}


