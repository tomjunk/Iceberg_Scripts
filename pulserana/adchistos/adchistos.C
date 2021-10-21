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


void adchistos(
            size_t nevents=650, 
	    std::string const& filename="iceberg_r009078_sr01_20210324T195224_1_dl1_decode.root", 
            std::string const& inputtag="tpcrawdecoder:daq")
{
  gStyle->SetOptStat(0);

  size_t evcounter=0;
  std::map<size_t,TH1F*> histomap;
  std::map<size_t,int> pedestalmap;

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    if (evcounter < nevents)
      {
	auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
	if (!rawdigits.empty())
	  {
	    const size_t nrawdigits = rawdigits.size();
	    for (size_t ichan=0;ichan<nrawdigits; ++ichan) 
	      { 
	        size_t thigh = rawdigits[ichan].Samples()-1; // assume uncompressed
		size_t ic = rawdigits[ichan].Channel();
		if (histomap.find(ic) == histomap.end())
		  {
		    TString hid = "chan";
		    hid += ic;
		    TString htitle = "Channel ";
		    htitle += ic;
		    htitle += ";ADC;Samples";
		    float xmid = rawdigits[ichan].GetPedestal();
		    //std::cout << "creating histo: " << ic << std::endl;
		    histomap[ic] = (TH1F*) new TH1F(hid,htitle,201,xmid-100.5,xmid+100.5);
		    histomap[ic]->SetDirectory(0);
		    pedestalmap[ic] = rawdigits[ichan].GetPedestal();
		    //std::cout << "  created histo: " << ic << std::endl;
		  }
		TH1F *th = histomap[ic];
		int ped = pedestalmap[ic];
                for (size_t itick=0;itick<=thigh;++itick)
	          {
		    //std::cout << "filling histo: " << ichan << " " << ic << " " << itick << std::endl;
		     th->Fill(rawdigits[ichan].ADC(itick));
		  }
	      }
	  }
      }
    ++evcounter;
  }
  //std::cout << "opening output file" << std::endl;
  TFile outputfile("adchistos_run9078.root","RECREATE");
  outputfile.cd();
  for (auto const& iter : histomap)
    {
      iter.second->Write();
    }
  std::cout << "closing output file" << std::endl;
  //outputfile.Close();
}


