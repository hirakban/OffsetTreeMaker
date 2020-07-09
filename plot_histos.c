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
#include <boost/algorithm/string/replace.hpp>
#include <iomanip>
#include <sstream>
void setStyle();
void plot_histos(TString mcName="/root_files_R48/SingleNeutrino_MC_R4.root", TString dataName="/root_files_R48/Legacy_BCD_R4.root", TString outName = "Run2018D_DataMC_R4_", TString label = "Run 2018D - 10.28 fb^{-1} (13 TeV)"){

  	setStyle();

	TFile* mc = new TFile(mcName);
	TFile* data = new TFile(dataName);

	// For nPU
        TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
	// For nPV
	TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
	// For rho
	TCanvas* c3 = new TCanvas("c3", "c3", 600, 600);
	gStyle->SetOptStat(0);

	// VARIABLE: nPU
	
	// Plotting + scaling nPU for MC
	TH1F* npu = 0;
	mc -> GetObject("nPU", npu);
	float mean_mc = npu ->GetMean(1);
	c1->cd();	
	npu-> GetXaxis()->SetTitle("#mu");
	Double_t norm = 1;
	Double_t scale_mc = norm/(npu->Integral());
	npu ->Scale(scale_mc);
	npu ->SetLineStyle(1);
	npu ->Draw("HIST");

	// Plotting + scaling nPU for Data
	TH1F* npu_data = 0;
	data -> GetObject("nPU", npu_data);
	float mean_data = npu_data ->GetMean(1);
	npu_data ->SetLineStyle(1);
	npu_data -> SetLineColor(kRed);
	Double_t scale_data = norm/(npu_data->Integral());
	npu_data->Scale(scale_data);
	npu_data ->Draw("HIST SAME");
	float max_npu_data = npu_data->GetMaximum();
	float max_npu_mc = npu ->GetMaximum();
	if ((max_npu_data/max_npu_mc) > 1) npu -> GetYaxis()->SetRangeUser(0.,(1.1 )* max_npu_data);
	else npu -> GetYaxis()->SetRangeUser(0.,(1.1 )* max_npu_mc);
	

  	c1->Update();
	npu ->GetYaxis()->SetTitleOffset(1.2);

	TLatex text;
	text.SetNDC();
	text.SetTextSize(0.048);;
	text.DrawLatex(0.17, 0.96, "CMS");

	TLegend* leg = new TLegend(.6,.8,.9,.9);
	leg->SetBorderSize(0);
	leg->SetFillColor(0);
	leg->SetFillStyle(0);
	leg->SetTextSize(0.03);
	leg->SetTextFont(42);

	text.SetTextSize(0.035);
	text.SetTextColor(1);
	text.SetTextFont(42);
	text.DrawLatex(0.52, 0.96, label );
	
	leg->AddEntry(npu,Form("MC (Mean %2.2f)", mean_mc));
	leg->AddEntry(npu_data,Form("Data (Mean %2.2f)", mean_data));

	leg->Draw();
	c1->Update();
	c1->Print("./presentation_plots/h_"+outName+"nPU.pdf");

	// VARIABLE: nPV
	
	// Plotting + scaling nPV for MC
	TH1F* npv = 0;
	mc -> GetObject("nPV", npv);
	float npv_mean_mc = npv ->GetMean(1);
	c2->cd();	
	npv-> GetXaxis()->SetTitle("N_{PV}");
	//Double_t norm = 1;
	Double_t npv_scale_mc = norm/(npv->Integral());
	npv ->Scale(npv_scale_mc);
	npv ->SetLineStyle(1);
	npv ->Draw("HIST");

	// Plotting + scaling nPV for Data
	TH1F* npv_data = 0;
	data -> GetObject("nPV", npv_data);
	float npv_mean_data = npv_data ->GetMean(1);
	npv_data ->SetLineStyle(1);
	npv_data -> SetLineColor(kRed);
	Double_t npv_scale_data = norm/(npv_data->Integral());
	npv_data->Scale(npv_scale_data);
	npv_data ->Draw("HIST SAME");
	float max_npv_data = npv_data->GetMaximum();
	float max_npv_mc = npv ->GetMaximum();
	if ((max_npv_data/max_npv_mc) > 1) npv -> GetYaxis()->SetRangeUser(0.,(1.1 )* max_npv_data);
	else npv -> GetYaxis()->SetRangeUser(0.,(1.1 )* max_npv_mc);

  	c2->Update();
	npv ->GetYaxis()->SetTitleOffset(1.2);

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
	text2.DrawLatex(0.52, 0.96, label );
	
	leg2->AddEntry(npv,Form("MC (Mean %2.2f)", npv_mean_mc));
	leg2->AddEntry(npv_data,Form("Data (Mean %2.2f)", npv_mean_data));

	leg2->Draw();
	c2->Update();
	c2->Print("./presentation_plots/h_"+outName+"nPV.pdf");

	// VARIABLE: rho
	
	// Plotting + scaling rho for MC
	TH1F* rho = 0;
	mc -> GetObject("rho", rho);
	float rho_mean_mc = rho ->GetMean(1);
	c3->cd();	
	rho -> GetXaxis()->SetTitle("#rho");
	//Double_t norm = 1;
	Double_t rho_scale_mc = norm/(rho->Integral());
	rho ->Scale(rho_scale_mc);
	rho ->SetLineStyle(1);
	rho ->Draw("HIST");

	// Plotting + scaling rho for Data
	TH1F* rho_data = 0;
	data -> GetObject("rho", rho_data);
	float rho_mean_data = rho_data ->GetMean(1);
	rho_data ->SetLineStyle(1);
	rho_data -> SetLineColor(kRed);
	Double_t rho_scale_data = norm/(rho_data->Integral());
	rho_data->Scale(rho_scale_data);
	rho_data ->Draw("HIST SAME");
	float max_rho_data = rho_data->GetMaximum();
	float max_rho_mc = rho ->GetMaximum();
	if ((max_rho_data/max_rho_mc) > 1) rho -> GetYaxis()->SetRangeUser(0.,(1.1 )* max_rho_data);
	else rho -> GetYaxis()->SetRangeUser(0.,(1.1 )* max_rho_mc);

  	c3->Update();
	rho ->GetYaxis()->SetTitleOffset(1.2);

	TLatex text3;
	text3.SetNDC();
	text3.SetTextSize(0.048);;
	text3.DrawLatex(0.17, 0.96, "CMS");

	TLegend* leg3 = new TLegend(.6,.8,.9,.9);
	leg3->SetBorderSize(0);
	leg3->SetFillColor(0);
	leg3->SetFillStyle(0);
	leg3->SetTextSize(0.03);
	leg3->SetTextFont(42);

	text3.SetTextSize(0.035);
	text3.SetTextColor(1);
	text3.SetTextFont(42);
	text3.DrawLatex(0.52, 0.96, label );
	
	leg3->AddEntry(rho,Form("MC (Mean %2.2f)", rho_mean_mc));
	leg3->AddEntry(rho_data,Form("Data (Mean %2.2f)", rho_mean_data));

	leg3->Draw();
	c3->Update();
	c3->Print("./presentation_plots/h_"+outName+"rho.pdf");
		
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
  tdrStyle->SetPadRightMargin(0.03);

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
  tdrStyle->SetTitleYOffset(1.1);

// For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.007, "XYZ");
  tdrStyle->SetLabelSize(0.025, "XYZ");

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

  tdrStyle->cd();
}
