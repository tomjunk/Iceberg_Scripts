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


void wfchan(
            size_t ievcount=0, 
	    size_t ichanrequest=1022,
	    std::string const& filename="iceberg_r009078_sr01_20210324T195224_1_dl1_decode.root", 
            std::string const& inputtag="tpcrawdecoder:daq")
{
  gStyle->SetOptStat(0);
  TCanvas *mycanvas = (TCanvas*) new TCanvas("c1","c1",1000,500);

  size_t evcounter=0;

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    //std::cout << "evcounter: " << evcounter << " " << ievcount << std::endl;
    if (evcounter == ievcount)
      {
	auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
	std::cout << "raw digit vector size: " << rawdigits.size() << std::endl;
	if (!rawdigits.empty())
	  {
	    std::cout << "got here 0" << std::endl;
	    const size_t nrawdigits = rawdigits.size();
	    for (size_t ichan=0;ichan<nrawdigits; ++ichan) 
	      { 
	        size_t thigh = rawdigits[ichan].Samples()-1; // assume uncompressed
		size_t ic = rawdigits[ichan].Channel();
		//std::cout << "channel: " << ic << std::endl;
		if ( ic == ichanrequest )
		  {
		    std::cout << "got here 1" << std::endl;
		    TString title="Channel ";
		    title += ichanrequest;
		    title += ";tick;ADC";
                    TH1F *outhist = (TH1F*) new TH1F("ch",title,thigh,-0.5,thigh-0.5);
		    outhist->SetDirectory(0);
                    TH1F *outhist2 = (TH1F*) new TH1F("ch",title,40,884.5,884.5+40);
		    outhist2->SetDirectory(0);
                    for (size_t itick=0;itick<=thigh;++itick)
	              {
			outhist->SetBinContent(itick+1,rawdigits[ichan].ADC(itick));
			//std::cout << rawdigits[ichan].ADC(itick) << std::endl;
			outhist2->Fill(rawdigits[ichan].ADC(itick));
		      }
		    outhist->Draw("hist");
		    //outhist2->Draw("hist");
		  }
	      }
	  }
      }
    ++evcounter;
  }
}


