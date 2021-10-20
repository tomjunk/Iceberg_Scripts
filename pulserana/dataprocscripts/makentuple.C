
#include "TString.h"
#include "Riostream.h"
void makentuple(TString infilename) {

   ifstream in;

   TString outfilename=infilename;
   outfilename += "_ntuple.root";

   in.open(infilename);

   Float_t chan,amp,sigma;
   Int_t nlines = 0;
   auto f = TFile::Open(outfilename,"RECREATE");
   TNtuple ntuple("peakfits","Peak Fits","chan:amp:sigma");

   while (1) {
      in >> chan >> amp >> sigma;
      if (!in.good()) break;
      if (nlines < 5) printf("x=%8f, y=%8f, z=%8f\n",chan,amp,sigma);
      ntuple.Fill(chan,amp,sigma);
      nlines++;
   }
   printf(" found %d points\n",nlines);

   in.close();

   f->Write();
}
