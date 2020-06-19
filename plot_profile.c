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

void setStyle();

void plot_profile(TString mcName="/root_files_R48/SingleNeutrino_MC_R4.root", TString dataName="/root_files_R48/Legacy_BCD_R4.root", TString outName = "Run2018D_DataMC_R4_", int n1=30, int n2=39, float topX=39, float topY=60, TString label = "Run 2018D - 10.28 fb^{-1} (13 TeV)"){

  setStyle();
        
  const int nPoints = n2-n1;

  TFile* mcFile = TFile::Open(mcName);
  TFile* dataFile = TFile::Open(dataName);


  TString xvar_type = "nPU"; 
  vector<TString> yvar_type = {"nPV","rho"};

  vector<TProfile*> profile_mc;
  vector<TProfile*> profile_data;


  TString hname;

  TF1* f_mc = new TF1("f_mc", "1++x++x*x",30,39);
  TF1* f_data = new TF1("f_data", "1++x++x*x",30,39);
  f_mc->SetLineColor(2);
  f_mc->SetLineWidth(2);
  f_data->SetLineColor(1);
  f_data->SetLineWidth(2);

  for ( int i=0; i<int(yvar_type.size()); i++){

    if(yvar_type[i].EqualTo("nPV"))  hname = "p_nPV_nPU";
    else if(yvar_type[i].EqualTo("rho")) hname = "p_rho_nPU";
 
    profile_data.push_back((TProfile*) dataFile->Get(hname));
    profile_mc.push_back((TProfile*) mcFile->Get(hname));


    TProfile* profile_data2 = profile_data.back();
    TProfile* profile_mc2 = profile_mc.back();

    if (profile_data.back()->GetXaxis()->GetBinWidth(1) == 0.5){
        profile_data.back()->Rebin();
        profile_mc.back()->Rebin();
    }
   
    profile_mc.back()->Fit(f_mc, "R");
    profile_data.back()->Fit(f_data, "R");

    TString xTitle = "#mu";
    TString yTitle = "";
    if ( yvar_type[i].EqualTo("nPV") ) yTitle = "<N_{PV}>";
    else if ( yvar_type[i].EqualTo("rho") ) yTitle = "<#rho> (GeV)";


    vector<double> mc_x, data_x, mc_y, data_y, mc_error, data_error , mcx_error , ratio_y , ratio_error;

    for (int n=0; n<nPoints; n++){

      double mcX = n1+n+0.5;
      double dataX = n1+n+0.5;

      double mcY = profile_mc.back()->GetBinContent( profile_mc.back()->FindBin(mcX) );
      double dataY = profile_data.back()->GetBinContent( profile_data.back()->FindBin(dataX) );
      double ratioY = dataY / mcY;
      mc_x.push_back( mcX );
      data_x.push_back( dataX );

      mc_y.push_back( mcY );
      data_y.push_back( dataY );

      double mcxerror = 0.01 * mcX;

      mcx_error.push_back(0.005*mc_x.back()); 
      mc_error.push_back( 0.02 * mc_y.back() );
      data_error.push_back( 0.02 * data_y.back() );

      double ratioerror = 0.003;
      TMath::Sqrt( mc_y.back()*mc_y.back() + data_y.back() * data_y.back());
    //cout << "ratio error"<<ratioerror<<endl;
    //double ratioerror = 0.02 * ratioY;

      ratio_y.push_back(ratioY);
      ratio_error.push_back(ratioerror);
        
    } 

    TGraphErrors* mcGraph = new TGraphErrors(mc_x.size(), &mc_x[0], &mc_y[0], 0, &mc_error[0]);
    TGraphErrors* dataGraph = new TGraphErrors(data_x.size(), &data_x[0], &data_y[0], 0, &data_error[0]);
    TGraphErrors* ratioGraph = new TGraphErrors(data_x.size(), &data_x[0], &ratio_y[0], &mcx_error[0], &ratio_error[0]); 

    mcGraph->Fit(f_mc, "R");
    dataGraph->Fit(f_data, "R");


    TCanvas* c = new TCanvas("c", "c", 600, 600);

    TPad* top = new TPad("top", "top", 0, 0.3, 1, 1);
    TPad* bottom = new TPad("bottom", "bottom", 0, 0, 1, 0.3);
    top->SetTopMargin(0.05);
    top->SetBottomMargin(0.05);
    top->Draw();
    bottom->SetBottomMargin(0.35);
    bottom->SetGridy();
    bottom->Draw();
    top->cd();
    TH1F* h = new TH1F("h", "h", 100, n1, topX);
    //float topY = 30;

    h->GetXaxis()->SetTickLength(0.03/0.7);
    h->GetXaxis()->SetLabelSize(0);
    //h1->GetYaxis()->SetTitleSize(0.06/t_scale);
    h->GetYaxis()->SetTitleSize(0.07);
    h->GetYaxis()->SetTitleOffset(0.6);
    //h1->GetYaxis()->SetLabelSize(0.05/t_scale);
    h->GetYaxis()->SetLabelSize(0.06);

    h->GetXaxis()->SetTitle(xTitle);
    h->GetYaxis()->SetTitle(yTitle);
    h->GetYaxis()->SetTitleOffset(1.05);
    h->GetYaxis()->SetRangeUser(0, topY);

    h->Draw();


    dataGraph->SetMarkerStyle(20);
    dataGraph->SetMarkerColor(1);
    dataGraph->Draw("Psame");
    mcGraph->SetMarkerStyle(24);
    mcGraph->SetMarkerColor(2);
    mcGraph->SetLineColor(2);
    mcGraph->Draw("Psame");

    TLatex text;
    text.SetNDC();
    text.SetTextSize(0.04);

    text.DrawLatex(0.2, 0.88, "Data");
    text.DrawLatex(0.2, 0.84, Form("#chi^{2}/ndof = %4.2f/%i", f_data->GetChisquare(), f_data->GetNDF() ) );
    text.DrawLatex(0.2, 0.8, Form("p0 = %4.3f #pm %4.3f", f_data->GetParameter(0), f_data->GetParError(0) ) );
    text.DrawLatex(0.2, 0.76, Form("p1 = %4.3f #pm %4.3f", f_data->GetParameter(1), f_data->GetParError(1) ) );
    text.DrawLatex(0.2, 0.72, Form("p2 = %4.4f #pm %4.4f", f_data->GetParameter(2), f_data->GetParError(2) ) );
    text.SetTextColor(2);
    text.DrawLatex(0.2, 0.64, "MC");
    text.DrawLatex(0.2, 0.6, Form("#chi^{2}/ndof = %4.2f/%i", f_mc->GetChisquare(), f_mc->GetNDF() ) );
    text.DrawLatex(0.2, 0.56, Form("p0 = %4.3f #pm %4.3f", f_mc->GetParameter(0), f_mc->GetParError(0) ) );
    text.DrawLatex(0.2, 0.52, Form("p1 = %4.3f #pm %4.3f", f_mc->GetParameter(1), f_mc->GetParError(1) ) );
    text.DrawLatex(0.2, 0.48, Form("p2 = %4.4f #pm %4.4f", f_mc->GetParameter(2), f_mc->GetParError(2) ) );

    text.SetTextColor(1);
    text.SetTextSize(0.045);
    text.SetTextFont(42);
    text.DrawLatex(0.5, 0.96, label );

    bottom->cd();
    TH1D* h2 = new TH1D("h2", "h2", 100, n1, topX);

    h2->GetXaxis()->SetLabelSize(0.12);
    h2->GetXaxis()->SetTickLength(0.03/0.3);
    h2->GetXaxis()->SetTitleSize(0.06/0.3);
    h2->GetXaxis()->SetTitleOffset(0.75);
    h2->GetXaxis()->SetTitle("#mu");

    if(yvar_type[i].EqualTo("nPV")) h2->GetYaxis()->SetRangeUser(0.4, 2.0);
    else h2->GetYaxis()->SetRangeUser(0.8, 1.1);
    h2->GetYaxis()->SetNdivisions(5, 3, 0);

    h2->GetYaxis()->SetLabelSize(0.12);
    h2->GetYaxis()->SetTitle("Data/MC");

    h2->GetYaxis()->SetTitleSize(0.15);
    h2->GetYaxis()->SetTitleOffset(0.43);

    ratioGraph->SetMarkerStyle(24);
    ratioGraph->SetMarkerColor();
    ratioGraph->SetMarkerSize(0.8); 
    h2->Draw();
    ratioGraph->Draw("Psame");

    c->Print("./presentation_plots/p_"+ outName + yvar_type[i]+ "_mu.pdf");
    delete profile_data2 ;
    delete profile_mc2 ;
    mc_x.clear(); data_x.clear(); mc_y.clear(); data_y.clear(); mc_error.clear(); data_error.clear(); mcx_error.clear(); ratio_y.clear(); ratio_error.clear();
   
    delete mcGraph;
    delete dataGraph;
    delete ratioGraph;
    delete c; delete h; delete h2; 
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
}
