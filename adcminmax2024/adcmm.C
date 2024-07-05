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
#include <filesystem>

using namespace art;
using namespace std;


// find the min and max adc values in a file per channel and make histograms

void adcmm(int runno=13378)
{

  TString fnts = "/nvme2/dunecet/decoded-hdf5/iceberghd_raw_run0";
  fnts += runno;
  fnts += "-decoded.root";
  std::string const filename(fnts.Data());
  if (!std::filesystem::exists(filename)) return;

  std::string const& inputtag="tpcrawdecoder:daq";
  
  TCanvas *c1 = (TCanvas*) new TCanvas("c1","c1",800,1200);
  c1->Divide(2,3);

  size_t evcounter=0;

  TString minadctitle="Min ADC Run ";
  minadctitle += runno;
  minadctitle += ";Channel;Min ADC";

  TString maxadctitle="Max ADC Run ";
  maxadctitle += runno;
  maxadctitle += ";Channel;Max ADC";

  TH1F *adcmin = (TH1F*) new TH1F("adcmin",minadctitle,1280,-0.5,1279.5);
  TH1F *adcmax = (TH1F*) new TH1F("adcmax",maxadctitle,1280,-0.5,1279.5);

  for (int i=1;i<=1280;++i)
    {
      adcmin->SetBinContent(i,-1);    // default value when we haven't seen this channel yet.
      adcmax->SetBinContent(i,-1);    // default value when we haven't seen this channel yet.
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
    break; // put this in to run just one event.  Maybe better to do this as it reduces the cosmic-ray contamination and there are enough pulses.
  }

  // find the min and max min and max, for defining histogram boundaries

  int minmin=16385;
  int minmax=16385;
  int maxmin=0;
  int maxmax=0;
  
  for (int i=1; i<= adcmin->GetNbinsX(); ++i)
    {
      if (i<800)   // induction
	{
	  int lmx = adcmax->GetBinContent(i);
	  int lmn = adcmin->GetBinContent(i);
	  if (lmn >= 0)
	    {
	      if (minmin > lmn) minmin = lmn;
	      if (maxmin < lmn) maxmin = lmn;
	    }
	  if (lmx >= 0)
	    {
	      if (minmax > lmx) minmax = lmx;
	      if (maxmax < lmx) maxmax = lmx;
	    }
	}
    }
  
  TH1F *hmi = (TH1F*) new TH1F("hmi",";Max ADC Induction",TMath::Min(20,maxmax-minmax+1),minmax-0.5,maxmax+0.5);
  TH1F *hni = (TH1F*) new TH1F("hni",";Min ADC Induction",TMath::Min(20,maxmin-minmin+1),minmin-0.5,maxmin+0.5);

  minmin=16385;
  minmax=16385;
  maxmin=0;
  maxmax=0;
  
  for (int i=1; i<= adcmin->GetNbinsX(); ++i)
    {
      if (i>=800)   // collection
	{
	  int lmx = adcmax->GetBinContent(i);
	  int lmn = adcmin->GetBinContent(i);
	  if (lmn >= 0)
	    {
	      if (minmin > lmn) minmin = lmn;
	      if (maxmin < lmn) maxmin = lmn;
	    }
	  if (lmx >= 0)
	    {
	      if (minmax > lmx) minmax = lmx;
	      if (maxmax < lmx) maxmax = lmx;
	    }
	}
    }
  
  TH1F *hmc = (TH1F*) new TH1F("hmc",";Max ADC Collection",TMath::Min(20,maxmax-minmax+1),minmax-0.5,maxmax+0.5);
  TH1F *hnc = (TH1F*) new TH1F("hnc",";Min ADC Collection",TMath::Min(20,maxmin-minmin+1),minmin-0.5,maxmin+0.5);
    
  for (int i=1; i<= adcmin->GetNbinsX(); ++i)
    {
      if (i<800)
	{
	  hmi->Fill(adcmax->GetBinContent(i));
	  hni->Fill(adcmin->GetBinContent(i));
	}
      else
	{
	  hmc->Fill(adcmax->GetBinContent(i));
	  hnc->Fill(adcmin->GetBinContent(i));
	}
    }


  // once more to get the totals to set the histogram min and max for display

  minmin=16385;
  minmax=16385;
  maxmin=0;
  maxmax=0;
  for (int i=1; i<= adcmin->GetNbinsX(); ++i)
    {
      int lmx = adcmax->GetBinContent(i);
      int lmn = adcmin->GetBinContent(i);
      if (lmn >= 0)
	{
	  if (minmin > lmn) minmin = lmn;
	  if (maxmin < lmn) maxmin = lmn;
	}
      if (lmx >= 0)
	{
	  if (minmax > lmx) minmax = lmx;
	  if (maxmax < lmx) maxmax = lmx;
	}
    }

  // gStyle->SetOptStat(0);
  
  c1->cd(1);
  adcmin->SetStats(0);
  adcmin->Draw("hist");
  c1->cd(2);
  adcmax->SetStats(0);
  adcmax->SetMinimum(minmax-10.0);
  adcmax->SetMaximum(maxmax*1.05);
  adcmax->Draw("hist");
  c1->cd(3);
  hni->Draw("hist");
  c1->cd(4);
  hmi->Draw("hist");
  c1->cd(5);
  hnc->Draw("hist");
  c1->cd(6);
  hmc->Draw("hist");
  
  TString opicfile = "run_";
  opicfile += runno;
  opicfile += "_adcminmax.png";
  c1->Print(opicfile);

  std::cout << "Run adc stats: " << runno << " " 
            << hni->GetMean() << " " 
            << hni->GetStdDev() << " " 
            << hmi->GetMean() << " " 
	    << hmi->GetStdDev() << " " 
            << hnc->GetMean() << " " 
            << hnc->GetStdDev() << " " 
            << hmc->GetMean() << " " 
	    << hmc->GetStdDev() << " " 
	    << std::endl; 
}
