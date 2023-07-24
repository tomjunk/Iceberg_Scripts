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
#include "TH2F.h"
#include "TStyle.h"
#include "TColor.h"
#include "TMath.h"
#include "TVectorT.h"
#include "TCanvas.h"
#include "lardataobj/RawData/RawDigit.h"
#include "TLegend.h"
#include "Riostream.h"
#include "TLine.h"

using namespace art;
using namespace std;


// arguments:  filename -- input file, larsoft formatted
// ievcount:  which event to display the mean and RMS for.  This is the tree index in the file and not the event number
// inputtag: use "daq" for MC and 3x1x1 imported data.  It's SplitterInput:TPC for split 35t data


void noisemap(std::string const& filename="iceberg_r009409_sr01_20210330T213914_1_dl1_decode.root", 
	      size_t ievcount=0, 
	      std::string const& inputtag="tpcrawdecoder:daq")
{

  std::vector<int> wdc;
  std::vector<int> wdt;
  std::vector<int> wdp;
  std::vector<float> wdx1;
  std::vector<float> wdy1;
  std::vector<float> wdz1;
  std::vector<float> wdx2;
  std::vector<float> wdy2;
  std::vector<float> wdz2;

  ifstream inwd;
  inwd.open("iceberg_draft_v1_wiredump.txt");

  while (1) 
    {
      int ich=0;
      int icry=0;
      int itpc=0;
      int iplane=0;
      int iwire=0;
      float x1=0;
      float y1=0;
      float z1=0;
      float x2=0;
      float y2=0;
      float z2=0;
      
      inwd >> ich >> icry >> itpc >> iplane >> iwire >> x1 >> y1 >> z1 >> x2 >> 
	y2 >> z2;
      if (!inwd.good()) break;
      if (y1 < 173.8 && y2 < 173.8) continue;  // only save segments on the top
      wdc.push_back(ich);
      wdt.push_back(itpc);
      wdp.push_back(iplane);
      wdx1.push_back(x1);
      wdy1.push_back(y1);
      wdz1.push_back(z1);
      wdx2.push_back(x2);
      wdy2.push_back(y2);
      wdz2.push_back(z2);
    }

  std::cout << "wire map size: " << wdc.size() << std::endl;

  gStyle->SetOptStat(0);

  size_t evcounter=0;

  InputTag rawdigit_tag{ inputtag };
  vector<string> filenames(1, filename);

  TH2F *rmsmap = (TH2F*) new TH2F("rmsmap","rmsmap;Z(cm);X(cm)",500,0,120,20,-4,4);
  rmsmap->SetDirectory(0);

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
    if (evcounter == ievcount)
      {
	auto const& rawdigits = *ev.getValidHandle<vector<raw::RawDigit>>(rawdigit_tag);
	if (!rawdigits.empty())
	  {
	    const size_t nrawdigits = rawdigits.size();
	    for (size_t ichan=0;ichan<nrawdigits; ++ichan) 
	      { 
		size_t ic = rawdigits[ichan].Channel();
		//if (ic == 0) continue;
		float RMS = rawdigits[ichan].GetSigma();
		for (size_t iwd=0;iwd<wdc.size(); ++iwd)
		  {
		    if (wdc.at(iwd) == ic)
		      {
			float xa = wdx1.at(iwd);
			float za = wdz1.at(iwd);
		        if (wdy1.at(iwd) < wdy2.at(iwd))
			  {
			    xa = wdx2.at(iwd);
			    za = wdz2.at(iwd);
			  }
			//std::cout << "x, z, RMS: " << xa << " " << za << " " << RMS << std::endl;
			rmsmap->Fill(za,xa,RMS);
			if (za>110 && xa>2) 
			  {
			    std::cout << "icweird: " << ic << " " << xa << " " << za << std::endl;
			  }
		      }
		  }
	      }
	  }
      }
    ++evcounter;
  }
  TCanvas *mycanvas = new TCanvas("c","c",1000,300);
  int nbinsx = rmsmap->GetNbinsX();
  int nbinsy = rmsmap->GetNbinsX();
  for (int ibiny=1;ibiny<=nbinsy;++ibiny)
    {
      for (int ibinx=nbinsx;ibinx>1;--ibinx)
	{
	  if (rmsmap->GetBinContent(ibinx,ibiny) == 0)
	    {
	      for (int jbx=ibinx-1;jbx>ibinx-5; --jbx)
		{
		  if (jbx<1) break;
		  float lastrms = rmsmap->GetBinContent(jbx,ibiny);
		  if (lastrms>0)
		    {
		      rmsmap->SetBinContent(ibinx,ibiny,lastrms);
		      break;
		    }
		}
	    }
	}
    }
  rmsmap->Draw("colz");

  for (int iline=0; iline<5;++iline)
    {
      float z=(iline+1)*23.0 + 0.5 ;
      float x=-4;
      TLine *line = (TLine*) new TLine(z,x,z,-x);
      line->Draw();
    }

  mycanvas->Print("rmsmap_run8272.png");
}
