
#include "TString.h"
#include "Riostream.h"
void makesummaryntuple(TString infilename) {

   ifstream in;

   TString outfilename=infilename;
   outfilename += "_ntuple.root";

   in.open(infilename);

   Float_t chan,negmean,negsigma,posmean,possigma;
   Int_t nlines = 0;
   auto f = TFile::Open(outfilename,"RECREATE");
   TNtuple ntuple("chansum","Channel Summaries","chan:negmean:negsigma:posmean:possigma");

   while (1) {
     in >> chan >> negmean >> negsigma >> posmean >> possigma;
      if (!in.good()) break;
      if (nlines < 5) printf("chan=%8f, negmean=%8f, negsigma=%8f, posmean=%8f, possigma=%8f\n",chan,negmean,negsigma,posmean,possigma);
      ntuple.Fill(chan,negmean,negsigma,posmean,possigma);
      nlines++;
   }
   printf(" found %d points\n",nlines);

   in.close();

   f->Write();
}
