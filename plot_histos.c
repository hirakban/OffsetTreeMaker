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
//void plot_histos(TString mcName="/root_files_R48/SingleNeutrino_MC_R4.root", TString dataName="/root_files_R48/Legacy_BCD_R4.root", TString dataName2="/root_files_R48/Legacy_BCD_R4.root", TString dataName3="/root_files_R48/Legacy_BCD_R4.root", TString outName = "Run2018D_DataMC_R4_", TString label = "Run 2018D - 10.28 fb^{-1} (13 TeV)"){

void plot_histos(TString mcName="/root_files_R48/SingleNeutrino_MC_R4.root", TString dataName="/root_files_R48/Legacy_BCD_R4.root", TString outName = "Run2018D_DataMC_R4_", TString label = "Run 2018D - 10.28 fb^{-1} (13 TeV)"){

  	setStyle();

	TFile* mc = new TFile(mcName);
	TFile* data = new TFile(dataName);
	//TFile* data2 = new TFile(dataName2);
	//TFile* data3 = new TFile(dataName3);

	// For nPU
        TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
	// For nPV
	TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
	// For rho
	TCanvas* c3 = new TCanvas("c3", "c3", 600, 600);
	// For pv_z
	//TCanvas* c4 = new TCanvas("c4", "c4", 600, 600);
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
	npu ->SetLineWidth(2);
	npu ->Draw("HIST");

	// Plotting + scaling nPU for Data
	TH1F* npu_data = 0;
	data -> GetObject("nPU", npu_data);
	float mean_data = npu_data ->GetMean(1);
	npu_data ->SetLineStyle(1);
	npu_data ->SetLineWidth(2);
	npu_data -> SetLineColor(kRed);
	Double_t scale_data = norm/(npu_data->Integral());
	npu_data->Scale(scale_data);
	npu_data ->Draw("HIST SAME");
	float max_npu_data = npu_data->GetMaximum();
	float max_npu_mc = npu ->GetMaximum();
	if ((max_npu_data/max_npu_mc) > 1) npu -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npu_data);
	else npu -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npu_mc);
	
/*
	TH1F* npu_data2 = 0;
	data2 -> GetObject("nPU", npu_data2);
	float mean_data2 = npu_data2 ->GetMean(1);
	npu_data2 ->SetLineStyle(1);
	npu_data2 ->SetLineWidth(2);
	npu_data2 -> SetLineColor(kGreen+3);
	Double_t scale_data2 = norm/(npu_data2->Integral());
	npu_data2->Scale(scale_data2);
	npu_data2 ->Draw("HIST SAME");
	float max_npu_data2 = npu_data2->GetMaximum();
	//float max_npu_mc = npu ->GetMaximum();
	if ((max_npu_data2/max_npu_mc) > 1) npu -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npu_data2);
	else npu -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npu_mc);

	TH1F* npu_data3 = 0;
	data3 -> GetObject("nPU", npu_data3);
	float mean_data3 = npu_data3 ->GetMean(1);
	npu_data3 ->SetLineStyle(1);
	npu_data3 ->SetLineWidth(2);
	npu_data3 -> SetLineColor(kViolet);
	Double_t scale_data3 = norm/(npu_data3->Integral());
	npu_data3->Scale(scale_data3);
	npu_data3 ->Draw("HIST SAME");
	float max_npu_data3 = npu_data3->GetMaximum();
	//float max_npu_mc = npu ->GetMaximum();
	if ((max_npu_data3/max_npu_mc) > 1) npu -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npu_data3);
	else npu -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npu_mc);
*/
  	c1->Update();
	npu ->GetYaxis()->SetTitleOffset(1.2);

	TLatex text;
	text.SetNDC();
	text.SetTextSize(0.048);;
	text.DrawLatex(0.17, 0.96, "CMS");

	TLegend* leg = new TLegend(.45,.73,.9,.92);
	leg->SetBorderSize(0);
	leg->SetFillColor(0);
	leg->SetFillStyle(0);
	leg->SetTextSize(0.03);
	leg->SetTextFont(42);

	text.SetTextSize(0.035);
	text.SetTextColor(1);
	text.SetTextFont(42);
	text.DrawLatex(0.6, 0.96, label );
	
//	leg->AddEntry(npu,Form("MC (Mean %2.2f)", mean_mc));
//	leg->AddEntry(npu_data,Form("Data (Mean %2.2f)", mean_data));
	leg->AddEntry(npu,Form("MC: Summer23BPix (Mean %2.2f)", mean_mc));
	leg->AddEntry(npu_data,Form("Data: Run2023D (Mean %2.2f)", mean_data));

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
	npv ->SetLineWidth(2);
	npv ->Draw("HIST");

	// Plotting + scaling nPV for Data
	TH1F* npv_data = 0;
	data -> GetObject("nPV", npv_data);
	float npv_mean_data = npv_data ->GetMean(1);
	npv_data ->SetLineStyle(1);
	npv_data ->SetLineWidth(2);
	npv_data -> SetLineColor(kRed);
	Double_t npv_scale_data = norm/(npv_data->Integral());
	npv_data->Scale(npv_scale_data);
	npv_data ->Draw("HIST SAME");
	float max_npv_data = npv_data->GetMaximum();
	float max_npv_mc = npv ->GetMaximum();
	if ((max_npv_data/max_npv_mc) > 1) npv -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npv_data);
	else npv -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npv_mc);
/*
	TH1F* npv_data2 = 0;
	data2 -> GetObject("nPV", npv_data2);
	float npv_mean_data2 = npv_data2 ->GetMean(1);
	npv_data2 ->SetLineStyle(1);
	npv_data2 ->SetLineWidth(2);
	npv_data2 -> SetLineColor(kGreen+3);
	Double_t npv_scale_data2 = norm/(npv_data2->Integral());
	npv_data2->Scale(npv_scale_data2);
	npv_data2 ->Draw("HIST SAME");
	float max_npv_data2 = npv_data2->GetMaximum();
	//float max_npv_mc = npv ->GetMaximum();
	if ((max_npv_data2/max_npv_mc) > 1) npv -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npv_data2);
	else npv -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npv_mc);

	TH1F* npv_data3 = 0;
	data3 -> GetObject("nPV", npv_data3);
	float npv_mean_data3 = npv_data3 ->GetMean(1);
	npv_data3 ->SetLineStyle(1);
	npv_data3 ->SetLineWidth(2);
	npv_data3 -> SetLineColor(kViolet);
	Double_t npv_scale_data3 = norm/(npv_data3->Integral());
	npv_data3->Scale(npv_scale_data3);
	npv_data3 ->Draw("HIST SAME");
	float max_npv_data3 = npv_data3->GetMaximum();
	//float max_npv_mc = npv ->GetMaximum();
	if ((max_npv_data3/max_npv_mc) > 1) npv -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npv_data3);
	else npv -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_npv_mc);
*/
  	c2->Update();
	npv ->GetYaxis()->SetTitleOffset(1.2);

	TLatex text2;
	text2.SetNDC();
	text2.SetTextSize(0.048);;
	text2.DrawLatex(0.17, 0.96, "CMS");

	TLegend* leg2 = new TLegend(.45,.72,.9,.9);
	leg2->SetBorderSize(0);
	leg2->SetFillColor(0);
	leg2->SetFillStyle(0);
	leg2->SetTextSize(0.03);
	leg2->SetTextFont(42);

	text2.SetTextSize(0.035);
	text2.SetTextColor(1);
	text2.SetTextFont(42);
	text2.DrawLatex(0.6, 0.96, label );
	
//	leg2->AddEntry(npv,Form("MC (Mean %2.2f)", npv_mean_mc));
//	leg2->AddEntry(npv_data,Form("Data (Mean %2.2f)", npv_mean_data));
	leg2->AddEntry(npv,Form("MC: Summer23BPix (Mean %2.2f)", npv_mean_mc));
	leg2->AddEntry(npv_data,Form("Data: Run2023D (Mean %2.2f)", npv_mean_data));

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
	rho ->SetLineWidth(2);
	rho ->Draw("HIST");

	// Plotting + scaling rho for Data
	TH1F* rho_data = 0;
	data -> GetObject("rho", rho_data);
	float rho_mean_data = rho_data ->GetMean(1);
	rho_data ->SetLineStyle(1);
	rho_data ->SetLineWidth(2);
	rho_data -> SetLineColor(kRed);
	Double_t rho_scale_data = norm/(rho_data->Integral());
	rho_data->Scale(rho_scale_data);
	rho_data ->Draw("HIST SAME");
	float max_rho_data = rho_data->GetMaximum();
	float max_rho_mc = rho ->GetMaximum();
	if ((max_rho_data/max_rho_mc) > 1) rho -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_rho_data);
	else rho -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_rho_mc);
/*
	TH1F* rho_data2 = 0;
	data2 -> GetObject("rho", rho_data2);
	float rho_mean_data2 = rho_data2 ->GetMean(1);
	rho_data2 ->SetLineStyle(1);
	rho_data2 ->SetLineWidth(2);
	rho_data2 -> SetLineColor(kGreen+3);
	Double_t rho_scale_data2 = norm/(rho_data2->Integral());
	rho_data2->Scale(rho_scale_data2);
	rho_data2 ->Draw("HIST SAME");
	float max_rho_data2 = rho_data2->GetMaximum();
	//float max_rho_mc = rho ->GetMaximum();
	if ((max_rho_data2/max_rho_mc) > 1) rho -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_rho_data2);
	else rho -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_rho_mc);

	TH1F* rho_data3 = 0;
	data3 -> GetObject("rho", rho_data3);
	float rho_mean_data3 = rho_data3 ->GetMean(1);
	rho_data3 ->SetLineStyle(1);
	rho_data3 ->SetLineWidth(2);
	rho_data3 -> SetLineColor(kViolet);
	Double_t rho_scale_data3 = norm/(rho_data3->Integral());
	rho_data3->Scale(rho_scale_data3);
	rho_data3 ->Draw("HIST SAME");
	float max_rho_data3 = rho_data3->GetMaximum();
	//float max_rho_mc = rho ->GetMaximum();
	if ((max_rho_data3/max_rho_mc) > 1) rho -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_rho_data3);
	else rho -> GetYaxis()->SetRangeUser(0.,(1.4 )* max_rho_mc);
*/
  	c3->Update();
	rho ->GetYaxis()->SetTitleOffset(1.2);

	TLatex text3;
	text3.SetNDC();
	text3.SetTextSize(0.048);;
	text3.DrawLatex(0.17, 0.96, "CMS");

	TLegend* leg3 = new TLegend(.45,.72,.9,.9);
	leg3->SetBorderSize(0);
	leg3->SetFillColor(0);
	leg3->SetFillStyle(0);
	leg3->SetTextSize(0.03);
	leg3->SetTextFont(42);

	text3.SetTextSize(0.035);
	text3.SetTextColor(1);
	text3.SetTextFont(42);
	text3.DrawLatex(0.6, 0.96, label );
	
//	leg3->AddEntry(rho,Form("MC (Mean %2.2f)", rho_mean_mc));
//	leg3->AddEntry(rho_data,Form("Data (Mean %2.2f)", rho_mean_data));
	leg3->AddEntry(rho,Form("MC: Summer23BPix (Mean %2.2f)", rho_mean_mc));
	leg3->AddEntry(rho_data,Form("Data: Run2023D (Mean %2.2f)", rho_mean_data));

	leg3->Draw();
	c3->Update();
	c3->Print("./presentation_plots/h_"+outName+"rho.pdf");
		
/*	// VARIABLE: pv_z
	
	// Plotting + scaling pv_z for MC
	TH1F* pv_z = 0;
	mc -> GetObject("pv_z", pv_z);
	float pv_z_mean_mc = pv_z ->GetMean(1);
	float pv_z_rms_mc = pv_z ->GetRMS(1);
	c4->cd();	
	pv_z -> GetXaxis()->SetTitle("pv_{z} (cm)");
	//Double_t norm = 1;
	Double_t pv_z_scale_mc = norm/(pv_z->Integral());
	pv_z ->Scale(pv_z_scale_mc);
	pv_z ->SetLineStyle(1);
	pv_z ->Draw("HIST");

	// Plotting + scaling pv_z for Data
	TH1F* pv_z_data = 0;
	data -> GetObject("pv_z", pv_z_data);
	float pv_z_mean_data = pv_z_data ->GetMean(1);
	float pv_z_rms_data = pv_z_data ->GetRMS(1);
	pv_z_data ->SetLineStyle(1);
	pv_z_data -> SetLineColor(kRed);
	Double_t pv_z_scale_data = norm/(pv_z_data->Integral());
	pv_z_data->Scale(pv_z_scale_data);
	pv_z_data ->Draw("HIST SAME");
	float max_pv_z_data = pv_z_data->GetMaximum();
	float max_pv_z_mc = pv_z ->GetMaximum();
	if ((max_pv_z_data/max_pv_z_mc) > 1) pv_z -> GetYaxis()->SetRangeUser(0.,(1.3 )* max_pv_z_data);
	else pv_z -> GetYaxis()->SetRangeUser(0.,(1.3 )* max_pv_z_mc);

  	c4->Update();
	pv_z ->GetYaxis()->SetTitleOffset(1.2);

	TLatex text4;
	text4.SetNDC();
	text4.SetTextSize(0.048);;
	text4.DrawLatex(0.17, 0.96, "CMS");

	TLegend* leg4 = new TLegend(.25,.8,.6,.9);
	leg4->SetBorderSize(0);
	leg4->SetFillColor(0);
	leg4->SetFillStyle(0);
	leg4->SetTextSize(0.03);
	leg4->SetTextFont(42);

	text4.SetTextSize(0.035);
	text4.SetTextColor(1);
	text4.SetTextFont(42);
	text4.DrawLatex(0.45, 0.96, label );
	
	leg4->AddEntry(pv_z,Form("MC (Mean %2.2f)", pv_z_mean_mc));
	leg4->AddEntry(pv_z_data,Form("Data (Mean %2.2f)", pv_z_mean_data));
//	leg4->AddEntry(pv_z,Form("MC S20 (Mean = %2.2f, RMS = %2.2f)", pv_z_mean_mc, pv_z_rms_mc));
//	leg4->AddEntry(pv_z,Form("MC S20 (RMS %2.2f)", pv_z_rms_mc));
//	leg4->AddEntry(pv_z_data,Form("MC S19 (Mean = %2.2f, RMS = %2.2f)", pv_z_mean_data, pv_z_rms_data));
//	leg4->AddEntry(pv_z_data,Form("MC S19 (RMS %2.2f)", pv_z_rms_data));

	leg4->Draw();
	c4->Update();
	c4->Print("./presentation_plots/h_"+outName+"pv_z.pdf");
*/
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
