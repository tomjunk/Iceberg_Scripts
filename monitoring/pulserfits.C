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
#include "TH1F.h"

using namespace art;
using namespace std;

//  ROOT script using gallery to fit Gaussians to pedestal-subtracted pulser data

void pulserfits(std::string const& filename="iceberg_r000255_sr01_20190219T200655_1_dl1_decode.root", 
             size_t ievcount=0, 
             size_t tickmin=0, 
             size_t tickmax=6000,  
             std::string const& inputtag="tpcrawdecoder:daq")
{

  gStyle->SetOptStat(0);

  size_t evcounter=0;

  InputTag rawdigit_tag{ inputtag };

  // Create a vector of length 1, containing the given filename.
  vector<string> filenames(1, filename);


	    TCanvas *mycanvas = new TCanvas("c","c",400,400);
	    //mycanvas->Divide(1,2);
	    //mycanvas->cd(2);

	    TH1F *collposamp = new TH1F("collposamp","Positive Collection Peaks",100,0,4000);
	    TH1F *collnegamp = new TH1F("collnegamp","Negative Collection Peaks",100,-4000,0);
	    TH1F *indposamp = new TH1F("indposamp","Positive Induction Peaks",100,0,4000);
	    TH1F *indnegamp = new TH1F("indnegamp","Negative Induction Peaks",100,-4000,0);


  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    if (evcounter == ievcount)
      {
	auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
	if (!rawdigits.empty())
	  {
	    const size_t nrawdigits = rawdigits.size();
	    size_t nchans=0;
	    for (size_t i=0; i<nrawdigits; ++i)
	      {
		size_t ic = rawdigits[i].Channel();
		if (nchans<ic) nchans=ic;
	      }
	    nchans++;  // set plots to go from channel 0 to the maximum channel we find.

	    size_t tlow = TMath::Max(tickmin, (size_t) 0);
	    size_t thigh = TMath::Min(tickmax, (size_t) rawdigits[0].Samples()-1); // assume uncompressed; all channels have the same number of samples
	    size_t nticks = thigh - tlow + 1;

	    TH1F *pedhist = (TH1F*) new TH1F("pedhist","pedhist",100,0,4000);

	    for (size_t ichan=0; ichan<nrawdigits; ++ichan)
	      {
		size_t ic = rawdigits[ichan].Channel();
		float pedestal = rawdigits[ichan].GetPedestal();
		if (rawdigits[ichan].GetSigma() > 1000) continue;
		size_t italreadyfit = 0;
		for (size_t itick=tlow; itick<=thigh; ++itick)
		  {
		    if (itick < italreadyfit) continue;
		    float psa = (float) rawdigits[ichan].ADC(itick) - pedestal;
		    if (std::abs(psa)>100)
		      {
			float sign=1.0;  // seems to fit positive Gaussians more easily
			if (psa<0) sign = -1.0;
			size_t tm1=0;
			if (itick>25) tm1=itick-25;
			size_t tp1=tickmax-1;
			if (itick<tickmax-26) tp1=itick+25;
			if (tp1>tm1)
			  {
			    size_t ntfit = tp1-tm1;
			    TH1F hf("hfit","hfit",ntfit,tm1,tp1);
			    for (size_t it2=tm1; it2<=tp1; ++it2)
			      {
				hf.SetBinContent(it2-tm1+1,sign*(rawdigits[ichan].ADC(it2) - pedestal));
			      }
			    TFitResultPtr fitresults = hf.Fit("gaus","SQ0");
			    mycanvas->Update();
			    float fitheight = fitresults->Parameter(0)*sign;
			    float fitwidth = fitresults->Parameter(2);

			    if (ic<800)
			      {
				if (sign>0) 
				  { 
				    indposamp->Fill(fitheight);
				  }
				else
				  {
				    indnegamp->Fill(fitheight);
				  }
				
			      }
			    else
			      {
				if (sign>0) 
				  { 
				    collposamp->Fill(fitheight);
				  }
				else
				  {
				    collnegamp->Fill(fitheight);
				  }
			      }

			  }
			else
			  {
			    std::cout << "tp1 less than tm1: " << tp1 << " " << tm1 << std::endl;
			  }
			italreadyfit = tp1;
		      }
		  }
	      }

	  }
      }
    ++evcounter;
  }

	    TCanvas *mycanvas2 = new TCanvas("c2","c2",800,800);
	    mycanvas2->Divide(2,2);
	    mycanvas2->cd(1);
	    collposamp->Draw("hist");
	    mycanvas2->cd(2);
	    collnegamp->Draw("hist");
	    mycanvas2->cd(3);
	    indposamp->Draw("hist");
	    mycanvas2->cd(4);
	    indnegamp->Draw("hist");

}
