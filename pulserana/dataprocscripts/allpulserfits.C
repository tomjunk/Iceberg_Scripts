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
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"

using namespace art;
using namespace std;

//  ROOT script using gallery to fit Gaussians to pedestal-subtracted pulser data

void allpulserfits(std::string const& filename="iceberg_r009083_sr01_20210324T201514_1_dl1_decode.root", 
		   size_t ievcount=0, 
		   size_t tickmin=0, 
		   size_t tickmax=2000,  
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

  TH1F *collposamp = new TH1F("collposamp","Positive Collection Peaks",100,0,40000);
  TH1F *collnegamp = new TH1F("collnegamp","Negative Collection Peaks",100,-40000,0);
  TH1F *indposamp = new TH1F("indposamp","Positive Induction Peaks",100,0,40000);
  TH1F *indnegamp = new TH1F("indnegamp","Negative Induction Peaks",100,-40000,0);
  TH1F *pedhist = (TH1F*) new TH1F("pedhist","pedhist",100,0,4000);
  TF1 *fitfunc = (TF1*) new TF1("f1","[0]*exp(-0.5*((x-[1])/[2])^2)");

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    //    if (evcounter == ievcount) // do them all
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
	  //std::cout << "got here 1: " << evcounter << std::endl;


	  for (size_t ichan=0; ichan<nrawdigits; ++ichan)
	    {

	      size_t nsamples = rawdigits[ichan].Samples();
	      if (nsamples < 1000) continue;
	      size_t tlow = TMath::Max(tickmin, (size_t) 0);
	      size_t thigh = TMath::Min(tickmax, (size_t) rawdigits[ichan].Samples()-1); // assume uncompressed; all channels have the same number of samples
	      size_t nticks = thigh - tlow + 1;

	      size_t ic = rawdigits[ichan].Channel();
	      float pedestal = rawdigits[ichan].GetPedestal();
	      if (rawdigits[ichan].GetSigma() > 1000) continue;
	      size_t italreadyfit = 0;
	      //std::cout << " got here 2: " << ichan << " " << tlow << " " << thigh << " " << nsamples <<  std::endl;

	      for (size_t itick=tlow; itick<=thigh; ++itick)
		{
		  if (itick < italreadyfit) continue;
		  float psa = (float) rawdigits[ichan].ADC(itick) - pedestal;
	          //std::cout << "  got here 3: " << itick << std::endl;

		  if (std::abs(psa)>100)
		    {
		      float sign=1.0;  // seems to fit positive Gaussians more easily
		      if (psa<0) sign = -1.0;
		      size_t tm1=0;
		      if (itick>3) tm1=itick-3;
		      size_t tp1=0;
		      if (itick<tickmax-5) 
			{
			  tp1=itick+5;
			  if (tp1>tm1)
			    {
			      float amax=0;
			      int imax=tm1;
			      for (size_t itmp=tm1; itmp<=tp1; ++itmp)
				{
				  float adcsub = rawdigits[ichan].ADC(itmp) - pedestal;
				  float adcsuba = std::abs(adcsub);
				  if (adcsuba > amax)
				    {
				      imax = itmp;
				      amax = adcsuba;
				      sign = 1.0;
				      if (adcsub < 0) sign = -1.0;
				    }
				}
			      int tm3 = imax - 3;
			      int tp3 = imax + 3;
			      if (tm3<0) tm3 = 0;
			      if (tp3> (int) thigh) tp3=thigh;

			      float adcintegral = 0;
			      size_t ntfit = tp3-tm3;
			      TH1F hf("hfit","hfit",ntfit,tm3,tp3);
			      for (size_t it2=tm3; (int) it2<=tp3; ++it2)
				{
				  float adcsub = rawdigits[ichan].ADC(it2) - pedestal;
				  hf.SetBinContent(it2-tm3+1,sign*adcsub);
				  adcintegral += adcsub;
				}
			      int imaxbin = hf.GetMaximumBin();
			      fitfunc->SetParameter(0,hf.GetBinContent(imaxbin));
			      fitfunc->SetParameter(1,hf.GetXaxis()->GetBinCenter(imaxbin));
			      fitfunc->SetParameter(2,2.0);
			  
			      float fitheight = 0;
			      float fitwidth = 0;

			      bool dofits = false;
			      if (dofits)
				{
				  TFitResultPtr fitresults = hf.Fit("f1","SQ0");  // take off the zero to draw the fit
				  //mycanvas->Update();
				  //sleep(2);
				  float fitheight = fitresults->Parameter(0)*sign;
				  fitwidth = fitresults->Parameter(2);
				}
			      else
				{
				  //hf.Draw("hist");
				  //mycanvas->Update();
				  //sleep(2);
				  fitheight = adcintegral;
				  fitwidth = 0;
				}

			      std::cout << "Chanfit: " << ic << " " << fitheight << " " << fitwidth << std::endl;

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
