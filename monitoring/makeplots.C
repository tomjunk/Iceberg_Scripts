// makeplots.C
// based on Stefan Schmitt's hadd.C in tutorials/io
// makes a directory called html, puts png files in it,
// with an index.html file.
// to execute non-interactively:  root -b -l -q 'makeplots.C("rawtpcmonitor.root");'

#include <string.h>
#include <map>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "TCanvas.h"
#include "stdio.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TPad.h"
#include "TString.h"
#include "TPRegexp.h"

TFile *Target;
TCanvas *c1;
FILE *indexhtml;
//std::map<TString, int> femb_map = {{"703", 1}, {"727", 2}, {"715",3},{"723",4},{"721",5},
//							   {"706",10},{"726",9},{"716",8},{"704",7},{"709",6}}; 
// new mapping Run 2  -- string replacement list
std::map<TString, int> femb_map = {{"727", 1}, {"726", 2}, {"716",3},{"723",4},{"703",5},
							   {"FM015",10},{"FM012",9},{"FM140",8},{"721",7},{"706",6}}; 

std::map<TString, TString> femb_replace = {
  {"FEMB_703 WIB1", "FEMB_727 W1F0"},
  {"FEMB_727 WIB1", "FEMB_726 W3F1"},
  {"FEMB_715 WIB1", "FEMB_716 W2F1"},
  {"FEMB_723 WIB1", "FEMB_723 W1F1"},
  {"FEMB_721 WIB2", "FEMB_703 W3F0"},
  {"FEMB_706 WIB2", "FEMB_706 W2F0"},
  {"FEMB_726 WIB2", "FEMB_721 W1F2"},
  {"FEMB_716 WIB2", "FEMB_715 W3F3"},
  {"FEMB_704 WIB3", "FEMB_FM012 W2F3"},
  {"FEMB_709 WIB3", "FEMB_FM015 W1F3"}
};

std::map<TString, TString> fembplotnamereplace = {
  {"FEMB_703", "FEMB_727"},
  {"FEMB_727", "FEMB_726"},
  {"FEMB_715", "FEMB_716"},
  {"FEMB_723", "FEMB_723"},
  {"FEMB_721", "FEMB_703"},
  {"FEMB_706", "FEMB_706"},
  {"FEMB_726", "FEMB_721"},
  {"FEMB_716", "FEMB_715"},
  {"FEMB_704", "FEMB_FM012"},
  {"FEMB_709", "FEMB_FM015"}
};

void mpa (TDirectory *dir);

void makeplots(TString infile="tmpmonitor.root")
{
  gSystem->mkdir("html");
  indexhtml = fopen("html/index.html","w");
  fprintf(indexhtml,"<h1>Histograms from %s</h1>\n",infile.Data());
  c1 = new TCanvas("c1","c1",800,800);
  TFile *f = new TFile(infile,"READ");
  TDirectory *current_sourcedir = gDirectory;
  mpa(current_sourcedir);
  fclose(indexhtml);
}

void mpa(TDirectory *current_sourcedir)
{

  // adding this part so that the most important channel plots show up first and are grouped together appropriately
  TList* listofkeys = (TList*)current_sourcedir->GetListOfKeys();
  listofkeys->Sort();
  TList* finallist = (TList*)listofkeys->Clone("finallist");
  TIter nextkeytemp( listofkeys );
  TKey *keytemp=0;
  TObject* putafter = (TObject*)finallist->FindObject("fChanRMSZ1_pfx");
  while ( (keytemp = (TKey*)nextkeytemp()))
    {
	  TObject *obj = keytemp->ReadObj();
	  if ( obj && !TString(keytemp->GetName()).Contains("fChan") ){
		finallist->AddLast(keytemp);
		finallist->Remove(finallist->FindObject(keytemp->GetName()));
	  }
	  else if ( obj && !TString(keytemp->GetName()).Contains("fChanRMS") ){
	    finallist->AddAfter(putafter, keytemp);
		finallist->Remove(finallist->FindObject(keytemp->GetName()));
		putafter = finallist->FindObject(keytemp->GetName());
	  }
	 }
  
  // loop over all keys in this directory
  TIter nextkey( finallist );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) 
    {
      //plot only the highest cycle number for each key
      if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

      // read object from  source file
      TObject *obj = key->ReadObj();

      if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) 
	{
	  // descendant of TH1 -> make a plot

	  TH1 *h1 = (TH1*)obj;
         
	  gPad->SetLogy(0);
	  if (h1->GetNbinsY()==1)
	    {
	      h1->SetStats(true);
	      if (obj->IsA()->InheritsFrom( TProfile::Class() ) )
		{
		  h1->SetStats(false);
		} 
	      TString htit = h1->GetTitle();
	      if (htit.Contains("RMS"))
		{
	          gPad->SetLogy(1);
		}
		  if (htit.Contains("Profiled FFT"))
		{
		  //TString newhtit = htit;
		  TPRegexp femb_old("(^.*FEMB_)([0-9]*)(.*$)");
	  	  TString prestr = ((TObjString*)femb_old.MatchS(htit)->At(1))->GetString();
  		  TString femb_n = ((TObjString*)femb_old.MatchS(htit)->At(2))->GetString();
	      TString new_title(Form(prestr+"%02d", femb_map[femb_n]));
		  h1->SetTitle(new_title);

		  //for (auto const &mapit : femb_replace)
		  //  {
		  //    newhtit.ReplaceAll(mapit.first,mapit.second);
		  //  }

		  //h1->SetTitle(newhtit);
		  h1->GetYaxis()->SetRangeUser(-50, 0);
		}
	      h1->Draw("hist,e0");
	    }
	  else
	    {
	      h1->SetStats(false);
	      h1->Draw("colz");
	    }
	  TString figname = key->GetName();
	  figname.ReplaceAll("#","_");
	  figname += ".png";
	  TString newfigname=figname;
	   //for (auto const &mapit : fembplotnamereplace)
	   //   {
		//      newfigname.ReplaceAll(mapit.first,mapit.second);
		//    }

	  TString outfilename="html/";
	  outfilename += newfigname;
	  c1->Print(outfilename);
	  fprintf(indexhtml,"<a href=%s><img src=%s width=300></a>\n",figname.Data(),figname.Data());
        
	}
      else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) 
	{
	  // it's a subdirectory

	  //cout << "Found subdirectory " << obj->GetName() << endl;
	  fprintf(indexhtml,"<h2>%s</h2>\n",obj->GetName());

	  current_sourcedir->cd(obj->GetName());
	  TDirectory *subdir = gDirectory;
	  mpa(subdir);
	  current_sourcedir->cd("..");
	} 
      else 
	{

	  // object is of no type that we know or can handle
	  cout << "Unknown object type, name: "
	       << obj->GetName() << " title: " << obj->GetTitle() << endl;
	}
    }
}
