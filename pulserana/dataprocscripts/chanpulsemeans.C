#include <iostream>
#include <string>
#include <vector>


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
#include "TLegend.h"
#include "TH1F.h"
#include "TF1.h"
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"

//  read fit peaks and fit a + peak and a - peak per channel from the distribution

void chanpulsemeans(TString filename="iceberg_r000249_sr01_20190219T173920_1_dl1_decode.root_fits.txt_ntuple.root")
{

  gStyle->SetOptStat(0);


  TCanvas *mycanvas = new TCanvas("c","c",400,400);

  TF1 *fitfunc = (TF1*) new TF1("f1","[0]*exp(-0.5*((x-[1])/[2])^2)");
  TFile infile(filename);
  
  std::vector<TH1F> neghist;
  std::vector<TH1F> poshist;

  for (size_t ichan=0; ichan<1280; ++ichan)
    {
      TString nhname="neghist";
      TString phname="poshist";
      nhname+=ichan;
      phname+=ichan;
      TH1F nh(nhname,nhname,1000,-40000,0);
      TH1F ph(phname,phname,1000,0,40000);
      neghist.push_back(nh);
      poshist.push_back(ph);
    }

  float amp=0;
  float chan=0;
  float sigma=0;
  TTree *peakfits = (TTree*) infile.Get("peakfits");
  TBranch *ampbranch = peakfits->GetBranch("amp");
  TBranch *chanbranch = peakfits->GetBranch("chan");
  TBranch *sigmabranch = peakfits->GetBranch("sigma");
  ampbranch->SetAddress(&amp);
  chanbranch->SetAddress(&chan);
  sigmabranch->SetAddress(&sigma);

  // first pass, just figure out where the boundaries are.

  size_t nentries = peakfits->GetEntries();
  for (size_t ientry=0; ientry<nentries; ++ientry)
    {
      peakfits->GetEntry(ientry);
      size_t ichan = chan + 0.1;  // just in case it's not exact -- the TNtuple only took floats
      if (ichan < 1280)
	{
	  if (amp<0)
	    {
	      neghist[ichan].Fill(amp);
	    }
	  else
	    {
	      poshist[ichan].Fill(amp);
	    }
	}
      else
	{
	  std::cout << "bad channel number, skipping: " << ichan << std::endl;
	} 
    }

  // now find the 2% and 98% quantiles and define new histograms to center on the peaks

  for (size_t ichan=0; ichan<1280; ++ichan)
    {
      size_t nbinsx = neghist[ichan].GetNbinsX();
      neghist[ichan].SetBinContent(nbinsx+1,0);
      neghist[ichan].SetBinContent(0,0);
      if (neghist[ichan].Integral()>10)
	{
	  size_t nbinsx = neghist[ichan].GetNbinsX();
	  float lowbound=0;
	  float highbound=0;
	  float sum=0;
	  float tot=neghist[ichan].Integral();
	  for (size_t ibin=1; ibin <= nbinsx; ++ibin)
	    {
	      sum += neghist[ichan].GetBinContent(ibin)/tot;
	      if (sum > 0.1)
		{
		  lowbound = neghist[ichan].GetXaxis()->GetBinCenter(ibin);
		  break;
		}
	    }
	  sum = 0;
	  for (size_t ibin=nbinsx; ibin >=1; --ibin)
	    {
	      sum += neghist[ichan].GetBinContent(ibin)/tot;
	      if (sum > 0.1)
		{
		  highbound = neghist[ichan].GetXaxis()->GetBinCenter(ibin);
		  break;
		}
	    }	  
	  TString nhname="neghist_";
	  nhname+=ichan;
	  //std::cout << "ichan " << ichan << " " << lowbound << " " << highbound << std::endl;
	  TH1F nh(nhname,nhname,50,lowbound-40,highbound+40);
	  neghist[ichan] = nh;
	}
      nbinsx = poshist[ichan].GetNbinsX();
      poshist[ichan].SetBinContent(nbinsx+1,0);
      poshist[ichan].SetBinContent(0,0);
      if (poshist[ichan].Integral()>10)
	{
	  float lowbound=0;
	  float highbound=0;
	  float sum=0;
	  float tot=poshist[ichan].Integral();
	  for (size_t ibin=1; ibin <= nbinsx; ++ibin)
	    {
	      sum += poshist[ichan].GetBinContent(ibin)/tot;
	      if (sum > 0.1)
		{
		  lowbound = poshist[ichan].GetXaxis()->GetBinCenter(ibin);
		  break;
		}
	    }
	  sum = 0;
	  for (size_t ibin=nbinsx; ibin >=1; --ibin)
	    {
	      sum += poshist[ichan].GetBinContent(ibin)/tot;
	      if (sum > 0.1)
		{
		  highbound = poshist[ichan].GetXaxis()->GetBinCenter(ibin);
		  break;
		}
	    }	  
	  TString phname="poshist_";
	  phname+=ichan;
	  TH1F ph(phname,phname,50,lowbound-40,highbound+40);
	  poshist[ichan] = ph;
	}
    }

  // fill the histos again

  for (size_t ientry=0; ientry<nentries; ++ientry)
    {
      peakfits->GetEntry(ientry);
      size_t ichan = chan + 0.1;  // just in case it's not exact -- the TNtuple only took floats
      if (ichan < 1280)
	{
	  if (amp<0)
	    {
	      neghist[ichan].Fill(amp);
	    }
	  else
	    {
	      poshist[ichan].Fill(amp);
	    }
	}
      else
	{
	  std::cout << "bad channel number, skipping: " << ichan << std::endl;
	} 
    }

  for (size_t ichan=0; ichan<1280; ++ichan)
    {
      float negmean = 0;
      float negwidth = 0;
      float posmean = 0;
      float poswidth = 0;
      bool hasneg=false;
      bool haspos=false;
      if (neghist[ichan].Integral() > 10)
	{
	  int imaxbin = neghist[ichan].GetMaximumBin();
	  fitfunc->SetParameter(0,neghist[ichan].GetBinContent(imaxbin));
	  fitfunc->SetParameter(1,neghist[ichan].GetXaxis()->GetBinCenter(imaxbin));
	  fitfunc->SetParameter(2,10.0);
	  //TFitResultPtr fitresults = neghist[ichan].Fit("f1","SQ0");  // take off the zero to draw the fit
	  TFitResultPtr fitresults = neghist[ichan].Fit("gaus","SQL0");  // take off the zero to draw the fit
	  //mycanvas->Update();
	  //sleep(2);
	  //float fitheight = fitresults->Parameter(0);
	  negmean = fitresults->Parameter(1);
	  negwidth = fitresults->Parameter(2);
	  hasneg = true;
	}
      if (poshist[ichan].Integral() > 10)
	{
	  int imaxbin = poshist[ichan].GetMaximumBin();
	  fitfunc->SetParameter(0,poshist[ichan].GetBinContent(imaxbin));
	  fitfunc->SetParameter(1,poshist[ichan].GetXaxis()->GetBinCenter(imaxbin));
	  fitfunc->SetParameter(2,10.0);
	  //TFitResultPtr fitresults = poshist[ichan].Fit("f1","SQ");  // take off the zero to draw the fit
	  TFitResultPtr fitresults = poshist[ichan].Fit("gaus","SQL");  // take off the zero to draw the fit
	  //mycanvas->Update();
	  //sleep(2);
	  //float fitheight = fitresults->Parameter(0);
	  posmean = fitresults->Parameter(1);
	  poswidth = fitresults->Parameter(2);
	  haspos = true;
	}
      if (hasneg && haspos)
	{
           std::cout << " Channel fit: " << ichan << " " << negmean << " " << negwidth << " " << posmean << " " << poswidth << std::endl;
	}
    }


}
