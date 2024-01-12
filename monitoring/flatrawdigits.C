#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "TFile.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "lardataobj/RawData/RawDigit.h"
#include <vector>
#include <set>
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"

using namespace art;
using namespace std;

// make a ROOT file with a TTree with flat raw digits in a format proposed by Mike Mooney for
// use with a 39Ar standalone study

void flatrawdigits(std::string const& infilename="iceberg_r008272_sr01_20210228T023206_1_dl1_decode.root", 
		   std::string const& outputfilename="run8272_flatrawdigits.root",
		   std::string const& inputtag="tpcrawdecoder:daq")
{

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, infilename);
  TFile outputfile(outputfilename.c_str(),"RECREATE");

  int runno=0;
  int subrunno=0;
  int eventno=0;
  short channo = 0;
  short plane = 0;
  std::vector<short> adcs;
  
  TTree otree("flatraw","Flattened Raw Digits");
  otree.Branch("run",&runno,"run/I");
  otree.Branch("subrun",&subrunno,"subrun/I");
  otree.Branch("event",&eventno,"event/I");
  otree.Branch("chan",&channo,"chan/S");
  otree.Branch("plane",&plane,"plane/S");
  otree.Branch("adc",&adcs);
  
  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
    if (!rawdigits.empty())
      {
	auto const& evaux = ev.eventAuxiliary();
	runno = evaux.run();
	subrunno = evaux.subRun();
	eventno = evaux.event();
	
	for (size_t ichan=0;ichan<rawdigits.size(); ++ichan) 
	  {
	    channo = (short) rawdigits[ichan].Channel();
            if (channo < 400)  // in lieu of a channel map
	      { plane = 0; }
	    else if (channo < 800)
	      { plane = 1; }
	    else
	      { plane = 2; }
	    adcs.clear();
	    for (size_t isample = 0; isample < rawdigits[ichan].Samples(); ++ isample)
	      { adcs.push_back(rawdigits[ichan].ADCs().at(isample)); }
	    otree.Fill();
	  }
      }
  }
  outputfile.cd();
  otree.Write();
  outputfile.Close();
}


