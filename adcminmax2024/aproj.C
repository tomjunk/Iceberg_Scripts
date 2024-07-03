{
  TCanvas *c1 = (TCanvas*) new TCanvas("c1","c1",800,400);
  c1->Divide(2,1);
  TH1F *hm = (TH1F*) new TH1F("hm",";Max ADC Value",100,15400,16400);
  TH1F *hn = (TH1F*) new TH1F("hn",";Min ADC Value",100,0,1000);
  TFile f("adcminmax.root","READ");
  auto *adcmin = (TH1F*) f.Get("adcmin");
  auto *adcmax = (TH1F*) f.Get("adcmax");
  for (int i=1; i<= adcmin->GetNbinsX(); ++i)
    {
      hm->Fill(adcmax->GetBinContent(i));
      hn->Fill(adcmin->GetBinContent(i));
    }
  c1->cd(1);
  hm->Draw("hist");
  c1->cd(2);
  hn->Draw("hist");
  c1->Print("adchistoprofiles.png");
}
