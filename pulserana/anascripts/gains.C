#include "TString.h"
#include "vector"
#include "array"
#include "Riostream.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TPad.h"
#include "TROOT.h"
#include "TLatex.h"
#include "TH1F.h"
#include "TFitResultPtr.h"
#include "TStyle.h"
#include <map>
#include "TMath.h"

TString psexec(TString cmd) {
  std::array<char, 128> buffer;
  TString result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.Data(), "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

void gains()
{
  gStyle->SetOptFit(1);
  std::vector<TString> filelist;
  std::vector<int> runlist;
  std::vector<int> daclist;

  std::map<size_t,float> posslopemap;
  std::map<size_t,float> negslopemap;
  
  ifstream infile;
  infile.open("rundac.dat");
  while (1)
    {
      int inum;
      infile >> inum;
      if (!infile.good()) break;
      if (inum<1000)
	{
	  //std::cout << "dac: " << inum << std::endl;
	  daclist.push_back(inum);
	}
      else
	{
	  runlist.push_back(inum);
	}
    }

  float dac[21];
  float ndac[21];

  for (size_t i=0; i<runlist.size(); ++i)
    {
      TString cmd = "ls iceberg_r00";
      cmd += runlist.at(i);
      cmd += "_sr01*";
      TString output=psexec(cmd);
      if (output.Contains("iceberg"))
	{
	  TString out2 = output.Chop();
          filelist.push_back(out2);
          dac[filelist.size()] = daclist.at(i);
          ndac[filelist.size()] = -daclist.at(i);
	  std::cout << "file: " << out2 << " dac: " << daclist.at(i) << std::endl;
	}
    }

  int nchans = 1280;
  int nfiles = filelist.size();
  int nptsfit = 5; // only take the first 5 points
  int firstpoint = 2;

  float negmean[1280][21];
  float negrms[1280][21];
  float posmean[1280][21];
  float posrms[1280][21];

  for (int i=0; i< nfiles; ++i)
    {
      dac[i] = daclist.at(i);
      ndac[i] = -daclist.at(i);
      for (int j=0; j<nchans; ++j)
	{
	  negmean[j][i]=0;
	  negrms[j][i]=0;
	  posmean[j][i]=0;
	  posrms[j][i]=0;
	}
    }

  for (int ifile=0; ifile<nfiles; ++ifile)
    {
      ifstream in;
      in.open(filelist[ifile]);
      int chan;
      float nm,ns,pm,ps;
      while (1)
	{
	  in >> chan >> nm >> ns >> pm >> ps;
	  if (!in.good()) break;
	  negmean[chan][ifile] = nm;
	  negrms[chan][ifile] = ns;
	  posmean[chan][ifile] = pm;
	  posrms[chan][ifile] = ps;

	}
      in.close();
    }

  TString outname;
  // divide up as appropriate
  TCanvas *mycanvas = (TCanvas*) new TCanvas("c1","c1",1000,1000);
  //mycanvas->Divide(8,8);

  TGraph grnm;
  TGraph grpm;
  TGraph grnfit;
  TGraph grpfit;
  std::vector<float> negoffsetv;
  std::vector<float> negslopev;
  std::vector<float> posoffsetv;
  std::vector<float> posslopev;

  TH1F *negoffset = (TH1F*) new TH1F("negoffset","negoffset",100,-2000,2000);
  TH1F *posoffset = (TH1F*) new TH1F("posoffset","posoffset",100,-2000,2000);
  TH1F *negslope  = (TH1F*) new TH1F("negslope","negslope",100,-2000,2000);
  TH1F *posslope  = (TH1F*) new TH1F("posslope","posslope",100,-2000,2000);
  TH1F *collgain  = (TH1F*) new TH1F("slopecoll","Collection Gain;slope [adc*ticks/DAC];Channels",100,0,2000);
  TH1F *indgain  = (TH1F*) new TH1F("slopeind","Induction Gain;slope [adc*ticks/DAC];Channels",100,0,2000);
  TH1F *pmslope = (TH1F*) new TH1F("pmnslope","posslope-negslope",100,-200,200);

  for (int ichan=0; ichan<nchans; ++ichan)
  //for (int ichan=500; ichan<501; ++ichan) // one-channel test
    {
      TString grnmtitle="Channel ";
      grnmtitle += ichan;
      //grnmtitle += ";DAC Setting;Pulse Mean";
      //mycanvas->SetTitle(grnmtitle);
      gPad->DrawFrame(-22,-30000,22,30000);
      grnm.Set(nfiles);
      grnfit.Set(nptsfit);
      for (int i=0; i<nfiles;++i)
	{
	  grnm.SetPoint(i,ndac[i],negmean[ichan][i]);
	  if (i>firstpoint && i<=nptsfit+firstpoint)
	    {
              grnfit.SetPoint(i-firstpoint-1,ndac[i],negmean[ichan][i]);
	    }
	}
      grnm.Draw("*");
      grnfit.Draw("*");
      TFitResultPtr negfitresults = grnfit.Fit("pol1","WS");
      negoffsetv.push_back(negfitresults->Value(0));
      negslopev.push_back(negfitresults->Value(1));
      negoffset->Fill(negoffsetv.back());
      negslope->Fill(negslopev.back());
      if (negslopev.back()>10)
	{
	  if (ichan>=800)
	    {
	      collgain->Fill(negslopev.back());
	    }
	  else
	    {
	      indgain->Fill(negslopev.back());
	    }
	  negslopemap[ichan] = negslopev.back();
	}

      grpm.Set(nfiles);
      grpfit.Set(nptsfit);
      for (int i=0; i<nfiles;++i)
	{
	  grpm.SetPoint(i,dac[i],posmean[ichan][i]);
	  if (i>firstpoint && i<=nptsfit+firstpoint)
	    {
              grpfit.SetPoint(i-firstpoint-1,dac[i],posmean[ichan][i]);
	    }
	}
      grpm.Draw("*");
      grpfit.Fit("pol1","W");
      grpfit.Draw("*");
      TFitResultPtr posfitresults = grpfit.Fit("pol1","WS");
      posoffsetv.push_back(posfitresults->Value(0));
      posslopev.push_back(posfitresults->Value(1));
      posoffset->Fill(posoffsetv.back());
      posslope->Fill(posslopev.back());
      pmslope->Fill(posslopev.back() - negslopev.back());
      if (posslopev.back()>10)
      {
	if (ichan>=800)
	  {
	    collgain->Fill(posslopev.back());
	  }
	else
	  {
	    indgain->Fill(posslopev.back());
	  }
	  posslopemap[ichan] = posslopev.back();
      }

      TLatex latex;
      latex.SetTextSize(0.03);
      latex.SetTextAlign(13);  //align at top
      latex.DrawLatex(10,-34000,"DAC Setting");
      //latex.DrawLatex(-22,0,"Mean Response");
      latex.SetTextSize(0.06);
      latex.DrawLatex(-10,34000,grnmtitle);
      TString outfilename="plots/chan";
      outfilename += ichan;
      outfilename += ".png";
      //mycanvas->Print(outfilename);
    }

  TCanvas *mycanvas2 = (TCanvas*) new TCanvas("c2","c2",1000,1000);
  mycanvas2->Divide(2,2);
  mycanvas2->cd(1);
  posoffset->Draw("hist");
  mycanvas2->cd(2);
  posslope->Draw("hist");
  mycanvas2->cd(3);
  negoffset->Draw("hist");
  mycanvas2->cd(4);
  negslope->Draw("hist");
  mycanvas2->Print("allslopesoffsets.png");


  TCanvas *mycanvas3 = (TCanvas*) new TCanvas("c3","c3",1000,1000);
  mycanvas3->Divide(2,2);
  mycanvas3->cd(1);
  pmslope->Draw("hist");
  mycanvas3->cd(3);
  collgain->Fit("gaus","");
  //collgain->Draw("hist");
  mycanvas3->cd(4);
  indgain->Fit("gaus","");
  //indgain->Draw("hist");
  mycanvas3->Print("allgains.png");

  for (const auto &imap : posslopemap)
    {
      size_t ichan = imap.first;
      float psl = imap.second;
      float avgslope = psl;
      if (negslopemap.find(ichan) != negslopemap.end())
	{
          float nsl = negslopemap[ichan];
	  if ( TMath::Abs(psl-nsl) < 30 )
	    {
	      avgslope = (nsl + psl)/2.0;
	    }
	}
      std::cout << "gainoutputtrj: " << ichan << " " << avgslope << std::endl;
    }
}
