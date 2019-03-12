//Bahareh Roozbahani August 2016
#include "TFile.h"
#include <vector>
#include "TProfile.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

void get_histos(TTree* tree, TH1F* h_z, TH1F* h_zf, TH1F* h_dzmin );
void setStyle();

void plot_dz(){

  setStyle();

  TFile* mcFile1 = TFile::Open("Total_MC_old.root");
  TFile* mcFile2 = TFile::Open("Total_MC_new.root");

  TTree* tree1 = (TTree*) mcFile1->Get("T");
  TTree* tree2 = (TTree*) mcFile2->Get("T");

  TH1F* h_z1 = new TH1F("h_z1", "h_z1", 50, -25, 25);
  TH1F* h_zf1 = new TH1F("h_zf1", "h_zf1", 50, -25, 25);
  TH1F* h_dzmin1 = new TH1F("h_dzmin1", "h_dzmin1", 100, -25, 25);

  TH1F* h_z2 = new TH1F("h_z2", "h_z2", 50, -25, 25);
  TH1F* h_zf2 = new TH1F("h_zf2", "h_zf2", 50, -25, 25);
  TH1F* h_dzmin2 = new TH1F("h_dzmin2", "h_dzmin2", 100, -25, 25);

//Getting histos for plotting

  get_histos(tree1, h_z1, h_zf1, h_dzmin1);
  get_histos(tree2, h_z2, h_zf2, h_dzmin2);

  TH1F* ratio1 = (TH1F*) h_zf1->Clone();
  ratio1 ->Divide(h_z1);

  TH1F* ratio2 = (TH1F*) h_zf2->Clone();
  ratio2 ->Divide(h_z2); 

  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
  TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
  TCanvas* c3 = new TCanvas("c3", "c3", 600, 600);

//////////////////////////////////

  c1->cd();

  TH1F* h = new TH1F("h", "h", 100, -20, 20);

  h->GetXaxis()->SetTickLength(0.03/0.7);
  h->GetXaxis()->SetLabelSize(0.043);
  h->GetXaxis()->SetLabelOffset(0.01);
  //h->GetYaxis()->SetTitle("");
  //h->GetYaxis()->SetTitleSize(0.07);
  //h->GetYaxis()->SetTitleOffset(0.6);
  h->GetYaxis()->SetLabelSize(0.043);
  h->GetYaxis()->SetRangeUser(0, 0.3);

  h->GetXaxis()->SetTitle("z(cm)");
  h->GetXaxis()->SetTitleSize(0.053);
  h->GetXaxis()->SetTitleOffset(0.9);
  
  h->Draw();

  h_z1->Scale(1./h_z1->GetEntries());
  h_zf1->Scale(1./h_z1->GetEntries());
  h_z1->SetLineWidth(2);
  h_z1->SetLineColor(1);
  h_z1->Draw("Hsame");
  h_zf1->SetLineWidth(2);
  h_zf1->SetLineColor(2);
  h_zf1->Draw("Hsame");

  TLatex text;
  text.SetNDC();
  text.SetTextSize(0.048);;
  text.DrawLatex(0.17, 0.96, "CMS");

  TLegend* leg = new TLegend(.45,.8,.9,.9);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.03);
  leg->SetTextFont(42);

  text.SetTextSize(0.035);
  text.SetTextColor(1);
  text.SetTextFont(42);
  text.DrawLatex(0.68, 0.96, "Run 2017 - (13 TeV)");

  leg->AddEntry(h_z1,Form("All true vertices (Mean %2.2f)", h_z1->GetMean(1)));
  leg->AddEntry(h_zf1,Form("Matched vertices (Mean %2.2f)", h_zf1->GetMean(1)));

  leg->Draw();
  c1->Update();

  c1->Print("histo_z_zf.pdf");

//////////////////////////////////

  c2->cd();

  TH1D* h2 = new TH1D("h2", "h2", 100, -20, 20);

  h2->GetXaxis()->SetLabelSize(0.043);
  h2->GetXaxis()->SetLabelOffset(0.01);
  h2->GetXaxis()->SetTickLength(0.03/0.7);
  h2->GetXaxis()->SetTitleSize(0.053);
  h2->GetXaxis()->SetTitleOffset(0.90);
  h2->GetXaxis()->SetTitle("z(cm)");

  h2->GetYaxis()->SetRangeUser(0.0, 1.5);
  h2->GetYaxis()->SetLabelSize(0.043);
  h2->GetYaxis()->SetLabelOffset(0.01);
  h2->GetYaxis()->SetTitle("PV reconstruction efficiency");

  h2->GetYaxis()->SetTitleSize(0.053);
  h2->GetYaxis()->SetTitleOffset(0.98);

  ratio1->SetMarkerStyle(24);
  ratio1->SetMarkerColor(1);
  ratio1->SetMarkerSize(0.8);
  
  ratio2->SetMarkerStyle(26);
  ratio2->SetMarkerColor(2);
  ratio2->SetMarkerSize(0.8); 
  h2->Draw();
  ratio1->Draw("Psame");
  ratio2->Draw("Psame");

  TLatex text1;
  text1.SetNDC();
  text1.SetTextSize(0.048);;
  text1.DrawLatex(0.17, 0.96, "CMS");

  TLegend* leg1 = new TLegend(.6,.8,.9,.9);
  leg1->SetBorderSize(0);
  leg1->SetFillColor(0);
  leg1->SetFillStyle(0);
  leg1->SetTextSize(0.03);
  leg1->SetTextFont(42);

  text1.SetTextSize(0.035);
  text1.SetTextColor(1);
  text1.SetTextFont(42);
  text1.DrawLatex(0.68, 0.96, "Run 2017 - (13 TeV)");

  leg1->AddEntry(ratio1,"MC_{old}");
  leg1->AddEntry(ratio2,"MC_{new}");

  leg1->Draw();
  c2->Update();

  c2->Print("PV_reco_eff.pdf");

////////////////////////////////////////////////
  
  c3->cd();
  gPad->SetLogy();
  TH1F* h3 = new TH1F("h3", "h3", 100, -15, 15);
  
  h3->GetXaxis()->SetLabelOffset(0.01);
  h3->GetXaxis()->SetTitleSize(0.053);
  h3->GetXaxis()->SetTitleOffset(0.9);
  h3->GetXaxis()->SetTickLength(0.03/0.7);
  h3->GetXaxis()->SetLabelSize(0.043);

  h3->GetYaxis()->SetTitleSize(0.053);
  h3->GetYaxis()->SetTitleOffset(0.92);
  h3->GetYaxis()->SetLabelSize(0.043);

  h3->GetXaxis()->SetTitle("dz (cm)");
  //h3->GetYaxis()->SetTitleOffset(1.05);
  h3->GetYaxis()->SetRangeUser(0.001, 0.4);
  h3->Draw();

  h_dzmin1->SetLineWidth(2);
  h_dzmin1->SetLineColor(1);
  h_dzmin1->Scale(1./h_dzmin1->GetEntries());
  h_dzmin1->Draw("Hsame");

  h_dzmin2->SetLineWidth(2);
  h_dzmin2->SetLineColor(2);
  h_dzmin2->Scale(1./h_dzmin2->GetEntries());
  h_dzmin2->Draw("Hsame");

  TLatex text2;
  text2.SetNDC();
  text2.SetTextSize(0.048);;
  text2.DrawLatex(0.17, 0.96, "CMS");

  TLegend* leg2 = new TLegend(.6,.8,.9,.9);
  leg2->SetBorderSize(0);
  leg2->SetFillColor(0);
  leg2->SetFillStyle(0);
  leg2->SetTextSize(0.03);
  leg2->SetTextFont(42);

  text2.SetTextSize(0.035);
  text2.SetTextColor(1);
  text2.SetTextFont(42);
  text2.DrawLatex(0.68, 0.96, "Run 2017 - (13 TeV)");

  leg2->AddEntry(h_dzmin1,"MC_{old}");
  leg2->AddEntry(h_dzmin2,"MC_{new}");

  leg2->Draw();
  c3->Update();

  c3->Print("dzmin.pdf");
  
}

void get_histos(TTree* tree, TH1F* h_z, TH1F* h_zf, TH1F* h_dzmin){
  
  Long64_t nEntries = tree->GetEntries();
  cout << nEntries << " Events" << endl;
  
  int mua[16];
  int nPVall;
  float puz[50], pv_z[50];

  tree->SetBranchAddress("mua", mua);
  tree->SetBranchAddress("puz", puz);
  tree->SetBranchAddress("pv_z", pv_z);
  tree->SetBranchAddress("nPVall", &nPVall);

  double dzcut = 1.;

  for (Long64_t n=0; n<nEntries; n++) {
    if (n % 10000 == 0) cout << "Processing Event " << n+1 << endl;
    tree->GetEntry(n);
    //cout << " ==== Event # pu pv " << n << "\t" << mua[12] << "\t" << nPVall << endl;
    bool found[50]; for(int i =0; i<50; ++i) found[i] = false;
    if (mua[12] > 49) continue;
    for(int i = 0; i != mua[12] && i < 50 ; ++i){  // loop over true pu vertices
      //cout << " PU: "<< i <<endl;
      h_z->Fill(puz[i]);
      double dzmin = 100000.;
      int jc;
      for(int j = 0; j != nPVall; ++j){   // loop over reco pu vertices
        if(found[j]) continue;
        //cout << "   PV: "<< j << endl;
        double dz = puz[i]-pv_z[j] ;
        //cout << "   dz: " << dz << endl;
        if(fabs(dz) < fabs(dzmin)) {
          dzmin = dz;
          jc = j ;
        }
      }
      //cout << "jc, found and dzmin: "<< jc << "\t" << found[jc] << "\t" << dzmin <<endl;
      h_dzmin->Fill(dzmin);
      if(dzmin < dzcut){
        found[jc] = true ;
        h_zf->Fill(puz[i]);
      }
    }
  }
}

void setStyle(){

//Style//

  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

// For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //POsition on screen
  tdrStyle->SetCanvasDefY(0);

// For the Pad:
  tdrStyle->SetPadBorderMode(0);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);

// For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);

  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.02);

// For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFontSize(0.05);

// For the axis titles:

  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(0.06, "XYZ");
  tdrStyle->SetTitleXOffset(0.9);
  tdrStyle->SetTitleYOffset(0.9);

// For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.009, "XYZ");
  tdrStyle->SetLabelSize(0.05, "XYZ");

// For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  //tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(0.03, "XYZ");
  tdrStyle->SetNdivisions(510, "XYZ");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

  tdrStyle->SetPaperSize(20.,20.);

  tdrStyle->SetHatchesLineWidth(5);
  tdrStyle->SetHatchesSpacing(0.05);

  tdrStyle->SetOptStat(0);

  tdrStyle->cd();
}
