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
#include "TH2D.h"
#include "TStyle.h"
#include "TColor.h"
#include "TMath.h"
#include "TVectorT.h"
#include "TCanvas.h"
#include "lardataobj/RawData/RawDigit.h"
#include "TLegend.h"

using namespace art;
using namespace std;

void signalsum(std::string const& filename="iceberg_r002939_sr01_20200212T213206_1_dl1_decode.root", 
               std::string const& inputtag="tpcrawdecoder:daq",
	       float thresh=10)
{

  gStyle->SetOptStat(0);

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  size_t nchans=1280;
  size_t nticks=6000;

  TH1D *sumhist = (TH1D*) new TH1D("sumhist",";Ticks;Summed ADC-ped",nticks,-0.5,nticks-0.5);
  TH1D *threshsumhist = (TH1D*) new TH1D("threshsumhist",";Ticks;Sum of (ADC-ped)'s over threshold",nticks,-0.5,nticks-0.5);
  TH2D *sumbychans = (TH2D*) new TH2D("sumbychans","Summed ADC-ped;Chan;Ticks",nchans,-0.5,nchans-0.5,500,-0.5,nticks-0.5);
  TH2D *threshsumbychans = (TH2D*) new TH2D("threshsumbychnas","Thresholded Sum ADC-ped;Chan;Ticks",nchans,-0.5,nchans-0.5,500,-0.5,nticks-0.5);

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
    if (!rawdigits.empty())
      {
	for (size_t ird=0;ird<rawdigits.size();++ird)
	  {
	    size_t ichan = rawdigits[ird].Channel();
	    // skip the non-working FEMB
	    if ( (ichan>991 && ichan<1040) ||
		 (ichan>559 && ichan<600) ||
		 (ichan<40) ) continue;
	    if ( ichan == 1135 || ichan == 1129 || ichan==791) continue;  //very noisy

	    size_t ntc = rawdigits[ird].Samples();
	    if (ichan>1279) 
	      {
		std::cout << "channel bigger than 1279: " << ichan << std::endl;
		continue;
	      }
	    for (size_t itick=0; itick<ntc; ++itick)
	      {
		float psa = ((float) rawdigits[ird].ADC(itick)) - rawdigits[ird].GetPedestal();
		sumhist->Fill(itick,psa);
		sumbychans->Fill(ichan,itick,psa);
		if (psa>thresh)
		  {
		    threshsumhist->Fill(itick,psa);
		    threshsumbychans->Fill(ichan,itick,psa);
		  }
	      }
	  }
      }
  }


  TCanvas *c1 = new TCanvas("c1","c1",700,600);
  sumhist->Draw("hist");
  c1->Print("sumhist.png");
  threshsumhist->Draw("hist");
  c1->Print("threshsumhist.png");
  sumbychans->Draw("colz");
  c1->Print("sumbychans.png");
  threshsumbychans->Draw("colz");
  c1->Print("threshsumbychans.png");
}
