#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "TFile.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TString.h"
#include <map>

using namespace std;
void setStyle();
const int nEta = 82;
float etabins[nEta+1] =
  {-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, -3.489, -3.314, -3.139, -2.964, -2.853, -2.65,
   -2.5, -2.322, -2.172, -2.043, -1.93, -1.83, -1.74, -1.653, -1.566, -1.479, -1.392, -1.305, -1.218, -1.131, -1.044, -0.957,
   -0.879, -0.783, -0.696, -0.609, -0.522, -0.435, -0.348, -0.261, -0.174, -0.087, 0,
   0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.879, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479,
   1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013,
   4.191, 4.363, 4.538, 4.716, 4.889, 5.191};

void compare_sf(int Rlabel=4, TString pf_type="all"){

  map<TString, TString> files;

  files["Summer22_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/l1offset_UL16/CMSSW_10_6_12/src/test/OffsetTreeMaker/textfiles_Run3/scalefactor/R4/chs/Run3-Summer22_DataMC_R4_DataMcSF_L1RC_AK4PFchs.txt";

  files["Summer22EE_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/l1offset_UL16/CMSSW_10_6_12/src/test/OffsetTreeMaker/textfiles_Run3/scalefactor/R4/chs/Run3-Summer22EE_DataMC_R4_DataMcSF_L1RC_AK4PFchs.txt";

  files["Summer23_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/l1offset_UL16/CMSSW_10_6_12/src/test/OffsetTreeMaker/textfiles_Run3/scalefactor/R4/chs/Run3-Summer23_DataMC_R4_DataMcSF_L1RC_AK4PFchs.txt";

  files["Summer23BPix_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/l1offset_UL16/CMSSW_10_6_12/src/test/OffsetTreeMaker/textfiles_Run3/scalefactor/R4/chs/Run3-Summer23BPix_DataMC_R4_DataMcSF_L1RC_AK4PFchs.txt";


//-------------------------------------------------------------------------------
  map<TString, TString> rootfiles;



  rootfiles["Summer22_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2022CD-ReReco_R4.root";

  rootfiles["Summer22EE_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2022EFG_R4.root";

  rootfiles["Summer23_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2023C-PromptReco_R4.root";

  rootfiles["Summer23BPix_R4"] = "/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2023D-PromptReco_R4.root";




//---------------------------------------------------------------------------------------------

  map<TString, float[nEta]> p0, p1, p2;
  map<TString, TH1F*> hists;

  setStyle();
  TCanvas* c = new TCanvas("c", "c", 600, 600);
  TH1F* h = new TH1F("h", "h", nEta, etabins);

  h->GetXaxis()->SetTitle("#eta");
  h->GetYaxis()->SetTitle("Scale Factor");
  h->GetYaxis()->SetTitleOffset(1.2);
  h->GetYaxis()->SetRangeUser(0.6, 1.9);
  h->Draw();

  TLegend* leg = new TLegend(.65,.65,.85,.85);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.03);
  leg->SetTextFont(62);

// If we want the stackplots in a root file
/*
  TString outName = "compare_sf.root";
  TFile* outFile = new TFile(outName,"RECREATE");
  TDirectory* dir = (TDirectory*) outFile;
*/

  for (map<TString, TString>::iterator it = files.begin(); it != files.end(); it++){

    TString version = it->first;
    ifstream file(it->second);
    string line;

    //read first line
    getline(file, line);

    for (int i=0; getline(file,line); i++){

      string str;
      int delim_pos;

      while (line.at(0) == ' ')  //eta values have space in front of them
        line.erase(0, 1);

      //loop over strings in line
      for (int col_num=0; (delim_pos = line.find(' ')) != -1; col_num++){

        str = line.substr(0, delim_pos);
        line.erase(0, delim_pos + 1);

        while (line.at(0) == ' ')  //get rid of white space between columns
          line.erase(0, 1);

        if (col_num == 5) p0[version][i] = stof(str);
        else if (col_num == 6) p1[version][i] = stof(str);
      }
      //last column after loop
      p2[version][i] = stof(line);
    }
    file.close();

    TFile* rootfile = TFile::Open( rootfiles[version] );
    TH1F* h_nPU = (TH1F*) rootfile->Get("nPU");
    float mu = h_nPU->GetMean();
    TProfile* p_rho_nPU = (TProfile*) rootfile->Get("p_rho_nPU");
    float rho = p_rho_nPU->GetBinContent( p_rho_nPU->FindBin( mu ) );

    int j = distance( files.begin(), it );
//    hists[version] = new TH1F( Form("h%i",j), Form("h%i",j), nEta, etabins);
    hists[version] = new TH1F( version, version, nEta, etabins);

    for (int i=0; i<nEta; i++) hists[version]->SetBinContent(i+1, p0[version][i] + p1[version][i]*rho + p2[version][i]*rho*rho);

// If we want the stackplots in a root file
//    outFile->cd();

    hists[version]->SetMarkerStyle(20);
    hists[version]->SetMarkerSize(0.75);
    hists[version]->SetMarkerColor(j+1);
    hists[version]->Draw("psame");

// If we want the stackplots in a root file
//    hists[version]->Write();

    leg->AddEntry(hists[version], version,"p");
  }


    hists["Summer22_R4"]->SetMarkerColor(kRed);
    hists["Summer22_R4"]->SetMarkerStyle(20);
    hists["Summer22_R4"]->Draw("psame");

    hists["Summer22EE_R4"]->SetMarkerColor(kRed+1);
    hists["Summer22EE_R4"]->SetMarkerStyle(21);
    hists["Summer22EE_R4"]->Draw("psame");

    hists["Summer23_R4"]->SetMarkerColor(kBlue);
    hists["Summer23_R4"]->SetMarkerStyle(20);
    hists["Summer23_R4"]->Draw("psame");

    hists["Summer23BPix_R4"]->SetMarkerColor(kBlue+2);
    hists["Summer23BPix_R4"]->SetMarkerStyle(21);
    hists["Summer23BPix_R4"]->Draw("psame");


  leg->Draw();

  TLatex text;
  text.SetNDC();
  text.SetTextSize(0.03);

  if (pf_type.EqualTo("all"))
    text.DrawLatex(0.8, 0.89, Form("AK%i PFchs", Rlabel));    // Form("AK%i PFchs", Rlabel)
  else
    text.DrawLatex(0.8, 0.89, Form("AK%i PFchs%s", Rlabel, pf_type.Data()));

  //text.SetTextSize(0.035);
  //text.SetTextColor(1);
  //text.SetTextFont(42);
  //text.DrawLatex(0.5, 0.96, "Summer16_07Aug2017");

  text.SetTextSize(0.05);
  text.SetTextFont(61);
  text.DrawLatex(0.19, 0.88, "CMS");
  text.SetTextFont(42);
  //text.DrawLatex(0.30, 0.88,"#it{Preliminary}");


  text.SetTextSize(0.03);
  text.SetTextFont(42);
  text.DrawLatex(0.62, 0.96, "(13.6 TeV)");


// If we want the stackplots in a root file
/*
  outFile->Write();
  delete outFile;
  outFile = 0;
*/

  c->Print("Run3_DATA_L1FastJet_files/compare_sf_Run3_AK4PFchs.pdf");
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
  tdrStyle->SetLabelOffset(0.007, "XYZ");
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

//End Style//
}



