#include "TString.h"
#include "vector"
#include "Riostream.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TPad.h"
#include "TROOT.h"
#include "TLatex.h"

void graphvals()
{
  std::vector<TString> filelist;
  filelist.push_back("iceberg_r008344_sr01_20210228T115852_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008378_sr01_20210228T155600_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008379_sr01_20210228T155827_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008380_sr01_20210228T160055_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008381_sr01_20210228T160322_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008382_sr01_20210228T160550_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008383_sr01_20210228T160816_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008384_sr01_20210228T161045_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008385_sr01_20210228T161312_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008386_sr01_20210228T161538_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008387_sr01_20210228T161806_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008388_sr01_20210228T162032_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008389_sr01_20210228T162259_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008390_sr01_20210228T162527_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008391_sr01_20210228T162755_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008392_sr01_20210228T163021_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008393_sr01_20210228T163246_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008395_sr01_20210228T163740_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008396_sr01_20210228T164006_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008397_sr01_20210228T164234_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");
  filelist.push_back("iceberg_r008398_sr01_20210228T164502_1_dl1_decode.root_fits.txt_ntuple.root_chans.txt");

  int nchans = 1280;
  int nfiles = filelist.size();

  float negmean[1280][21];
  float negrms[1280][21];
  float posmean[1280][21];
  float posrms[1280][21];
  float dac[21];
  float ndac[21];

  for (int i=0; i< nfiles; ++i)
    {
      dac[i] = i;
      ndac[i] = -i;
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
  mycanvas->Divide(8,8);
  int npanels=64;
  int ipanel = 0;
  int ioutfile=0;
  TGraph grnm[65];
  TGraph grpm[65];

  for (int ichan=0; ichan<nchans; ++ichan)
    {
      ipanel++;
      mycanvas->cd(ipanel);

      TString grnmtitle="Channel ";
      grnmtitle += ichan;
      //grnmtitle += ";DAC Setting;Pulse Mean";
      //mycanvas->SetTitle(grnmtitle);
      gPad->DrawFrame(-22,-30000,22,30000);
      grnm[ipanel].Set(nfiles);
      for (int i=0; i<nfiles;++i)
	{
	  grnm[ipanel].SetPoint(i,ndac[i],negmean[ichan][i]);
	}
      grnm[ipanel].Draw("L");

      grpm[ipanel].Set(nfiles);
      for (int i=0; i<nfiles;++i)
	{
	  grpm[ipanel].SetPoint(i,dac[i],posmean[ichan][i]);
	}
      grpm[ipanel].Draw("L");

      TLatex latex;
      latex.SetTextSize(0.03);
      latex.SetTextAlign(13);  //align at top
      latex.DrawLatex(10,-34000,"DAC Setting");
      //latex.DrawLatex(-22,0,"Mean Response");
      latex.SetTextSize(0.06);
      latex.DrawLatex(-10,34000,grnmtitle);

      if ( ipanel == npanels || ichan == nchans-1 )
	{
          ipanel = 0;
          outname="outplots/plots";
          outname += ioutfile;
          outname += ".pdf";
          mycanvas->Print(outname);
          ioutfile ++;
	}
    }
}
