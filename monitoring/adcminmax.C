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
#include <map>
#include "Riostream.h"
#include "TString.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TROOT.h"

using namespace art;
using namespace std;


void adcminmax(
	    std::string const& filename="/nvme2/dunecet/decoded-hdf5/iceberghd_raw_run012893-decoded.root",
            std::string const& inputtag="tpcrawdecoder:daq"
	       )
{
  gStyle->SetOptStat(0);

  size_t evcounter=0;

  TH1F *adcmin = (TH1F*) new TH1F("adcmin",";Channel;Min ADC",1280,-0.5,1279.5);
  TH1F *adcmax = (TH1F*) new TH1F("adcmax",";Channel;Max ADC",1280,-0.5,1279.5);

  for (int i=1;i<=1280;++i)
    {
      adcmin->SetBinContent(i,-1);    // default value when we haven't seen this channel yet.
    }

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
      {
	auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
	if (!rawdigits.empty())
	  {
	    const size_t nrawdigits = rawdigits.size();
	    for (size_t ichan=0;ichan<nrawdigits; ++ichan) 
	      { 
	        size_t thigh = rawdigits[ichan].Samples()-1; // assume uncompressed
		size_t ic = rawdigits[ichan].Channel();

		float oldmin = adcmin->GetBinContent(ic);
		float oldmax = adcmax->GetBinContent(ic);

                for (size_t itick=0;itick<=thigh;++itick)
	          {
		    //std::cout << "filling histo: " << ichan << " " << ic << " " << itick << std::endl;
		    float adc = rawdigits[ichan].ADC(itick);
		    if (adc < oldmin || oldmin <0) oldmin = adc;
		    if (adc > oldmax) oldmax = adc;
		  }
		adcmin->SetBinContent(ic,oldmin);
		adcmax->SetBinContent(ic,oldmax);
	      }
	  }
      }
    ++evcounter;
  }
  //std::cout << "opening output file" << std::endl;
  TFile outputfile("adcminmax.root","RECREATE");
  outputfile.cd();
  adcmin->Write();
  adcmax->Write();
  std::cout << "closing output file" << std::endl;
  //outputfile.Close();
}


