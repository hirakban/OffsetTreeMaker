//Chad Harrington and Bahareh Roozbahani Dec 2018
//EXECUTE as root -l -b -q offsetpT.c

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

const int ETA_BINS = 82;
float etabins[ETA_BINS+1] =
  {-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, -3.489, -3.314, -3.139, -2.964, -2.853, -2.65,
   -2.5, -2.322, -2.172, -2.043, -1.93, -1.83, -1.74, -1.653, -1.566, -1.479, -1.392, -1.305, -1.218, -1.131, -1.044, -0.957,
   -0.879, -0.783, -0.696, -0.609, -0.522, -0.435, -0.348, -0.261, -0.174, -0.087, 0,
   0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.879, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479,
   1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013,
   4.191, 4.363, 4.538, 4.716, 4.889, 5.191};

void offsetpT(TString mcName="/root_files_R48/SingleNeutrino_MC_R4.root", TString dataName="/root_files_R48/Legacy_BCD_R4.root", TString outName = "UL17_RunBCDEF_V1_", TString pull_subplot = "", TString datamc_subplot = "", const double R=0.4, int pf_choice=1, int n1=10, int n2=52, float topX=52, float topY=52){
  setStyle();
        
  int Rlabel = R*10;

  TFile* mcFile = TFile::Open(mcName);
  TFile* dataFile = TFile::Open(dataName);//_pTcut15GeV
  TString var_type = "nPU"; // nPU, nPV
  bool isIndirect = true;   // indirectRho
  bool rhoCentral = false;
  bool pull = false;
  if (pull_subplot == "true") pull = true ;
  bool DataOverMC = false;
  if (datamc_subplot == "true") DataOverMC = true;
  TString hp = "p";  // profiles (“p”) or histograms (“h”)
                
  const int nPoints = n2-n1;

  TString pf_type = "all"; //default choice
  if (pf_choice == 2) pf_type = "chm";
  else if (pf_choice == 3) pf_type = "lep";
  else if (pf_choice == 4) pf_type = "ne";
  else if (pf_choice == 5) pf_type = "nh";
  else if (pf_choice == 6) pf_type = "hfh";
  else if (pf_choice == 7) pf_type = "hfe";
  else if (pf_choice == 8) pf_type = "chs";
  else if (pf_choice == 9) pf_type = "MEDchs";
  else if (pf_choice == 10) pf_type = "MED";

  vector<TH1D*> mcProfiles;
  vector<TH1D*> dataProfiles;
  vector<TH1D*> mcIetas;
  vector<TH1D*> dataIetas;

  
  vector<TString> ids = {"ne", "hfe", "nh", "hfh", "chu", "chm"};

  for (int n=0; n<nPoints; n++){
    if(hp.EqualTo("h")){
      TString hieta = Form("h_ieta_nPU%i", n1+n); 

      TH1D* h_mc = (TH1D*) mcFile->FindObjectAny(hieta);
      h_mc->SetName(hieta+"_mc");
      mcIetas.push_back(h_mc);

      TH1D* h_data = (TH1D*) dataFile->FindObjectAny(hieta);
      h_data->SetName(hieta+"_data");
      dataIetas.push_back(h_data);
    }

    if (pf_type.EqualTo("all") || pf_type.EqualTo("chs")){  
      int kk = pf_type.EqualTo("all")  ? 1 : 2 ; // add up all histograms (“all”) or add up all histograms except chm (“chs”)
      TString hname = hp+Form("_offset_eta_%s%i_", var_type.Data(), n1+n) + ids[ids.size()-kk];
      if(hp.EqualTo("p")){
        mcProfiles.push_back( ((TProfile*) mcFile->FindObjectAny(hname))->ProjectionX( ids[ids.size()-kk]+Form("%i_mc",n1+n) ) );
        dataProfiles.push_back( ((TProfile*) dataFile->FindObjectAny(hname))->ProjectionX( ids[ids.size()-kk]+Form("%i_data",n1+n) ) );
      }
      else {
        TH1D* h_mc = (TH1D*) mcFile->FindObjectAny(hname);
        h_mc->SetName(ids[ids.size()-kk]+Form("%i_mc",n1+n) );
        mcProfiles.push_back(h_mc);
        TH1D* h_data = (TH1D*) dataFile->FindObjectAny(hname);
        h_data->SetName(ids[ids.size()-kk]+Form("%i_data",n1+n) );
        dataProfiles.push_back(h_data);
      }
      for (int i=0; i<ids.size()-kk; i++){
        hname = hp+Form("_offset_eta_%s%i_", var_type.Data(), n1+n) + ids[i];
        if(hp.EqualTo("p")){
          mcProfiles.back()->Add( ((TProfile*) mcFile->FindObjectAny(hname))->ProjectionX( ids[i]+Form("%i_mc",n1+n) ) );
          dataProfiles.back()->Add( ((TProfile*) dataFile->FindObjectAny(hname))->ProjectionX( ids[i]+Form("%i_data",n1+n) ) );
        }
        else {
          TH1D* h_mc = (TH1D*) mcFile->FindObjectAny(hname);
          h_mc->SetName(ids[i]+Form("%i_mc",n1+n));
          mcProfiles.push_back(h_mc);
          TH1D* h_data = (TH1D*) dataFile->FindObjectAny(hname);
          h_data->SetName(ids[i]+Form("%i_data",n1+n));
          dataProfiles.push_back(h_data);
        }
      }
    }

    else if (pf_type.EqualTo("MEDchs") || pf_type.EqualTo("MED") ){
      TString hname = hp+Form("_offset%s_eta_%s%i",pf_type.Data(), var_type.Data(), n1+n);
      if(hp.EqualTo("p")){
        mcProfiles.push_back( ((TProfile*) mcFile->FindObjectAny(hname))->ProjectionX( pf_type+Form("%i_mc",n1+n) ) );
        dataProfiles.push_back( ((TProfile*) dataFile->FindObjectAny(hname))->ProjectionX( pf_type+Form("%i_data",n1+n) ) );
      }
      else {
        TH1D* h_mc = (TH1D*) mcFile->FindObjectAny(hname);
        h_mc->SetName(pf_type+Form("%i_mc",n1+n));
        mcProfiles.push_back(h_mc);
        TH1D* h_data = (TH1D*) dataFile->FindObjectAny(hname);
        h_data->SetName(pf_type+Form("%i_data",n1+n));
        dataProfiles.push_back(h_data);
      }
    }
    else{
      TString hname = hp+Form("_offset_eta_%s%i_", var_type.Data(), n1+n) + pf_type;
      if(hp.EqualTo("p")){
        mcProfiles.push_back( ((TProfile*) mcFile->FindObjectAny(hname))->ProjectionX( pf_type+Form("%i_mc",n1+n) ) );
        dataProfiles.push_back( ((TProfile*) dataFile->FindObjectAny(hname))->ProjectionX( pf_type+Form("%i_data",n1+n) ) );
      }
      else {
        TH1D* h_mc = (TH1D*) mcFile->FindObjectAny(hname);
        h_mc->SetName(pf_type+Form("%i_mc",n1+n));
        mcProfiles.push_back(h_mc);
        TH1D* h_data = (TH1D*) dataFile->FindObjectAny(hname);
        h_data->SetName(pf_type+Form("%i_data",n1+n));
        dataProfiles.push_back(h_data);
      }
    }
    if(hp.EqualTo("h")){
      mcProfiles.back() ->Divide(mcIetas.back());
      dataProfiles.back() ->Divide(dataIetas.back());
    }
  }

  TProfile* NPUtoRho_mc;
  TProfile* NPUtoRho_data;

  if (isIndirect){

    TString hname;
    if (var_type.EqualTo("nPU")) {
      if (rhoCentral) hname = "p_rhoCentral0_nPU";
      else hname = "p_rho_nPU";
    }
    else {
      if (rhoCentral) hname = "p_rhoCentral0_nPV";
      else hname = "p_rho_nPV";
    }

    NPUtoRho_mc = (TProfile*) mcFile->Get(hname);
    NPUtoRho_data = (TProfile*) dataFile->Get(hname);

    if (NPUtoRho_data->GetXaxis()->GetBinWidth(1) == 0.5){
      NPUtoRho_mc->Rebin();
      NPUtoRho_data->Rebin();
    }
    var_type = "indirectRho";
  }

  //cout<<"MC txt: ./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + Form("/%sMC_L1RC_AK%iPF", outName.Data(), Rlabel) + pf_type + ".txt"<<endl;
  ofstream writeMC("./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + Form("/%sMC_L1RC_AK%iPF", outName.Data(), Rlabel) + pf_type + ".txt");
  
  //cout<<"Data txt: ./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + Form("/%sDATA_L1RC_AK%iPF", outName.Data(), Rlabel) + pf_type + ".txt"<<endl;
  ofstream writeData("./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + Form("/%sDATA_L1RC_AK%iPF",outName.Data(), Rlabel) + pf_type + ".txt");

  TString header;
  // changes w.r.t previous versions: 1) TFormula had a bug and we modified accordingly 2) based on Mikko's calculation 1.519 changed to 2.00
  if ( var_type.EqualTo("nPV") || var_type.EqualTo("nPU") )
    //header = "{1\tJetEta\t3\tJetPt\tJetA\t" + var_type + "\t\tmax(0.0001,1-y*([0]+[1]*(z-1)+[2]*pow(z-1,2))/x)\tCorrection L1Offset}";
    //header = "{1\tJetEta\t3\tJetPt\tJetA\t" + var_type + "\t\tmax(0.0001,1-(y/x)*([0]+[1]*(z-1.519)+[2]*pow(z-1.519,2)))\tCorrection L1Offset}";
    header = "{1 JetEta 3 JetPt JetA " + var_type + "  max(0.0001,1-(y/x)*([0]+[1]*(z-2.00)+[2]*pow(z-2.00,2))) Correction L1Offset}";
  else
    //header = "{1\tJetEta\t3\tJetPt\tJetA\tRho\tmax(0.0001,1-y*([0]+[1]*(z-1.519)+[2]*pow(z-1.519,2))/x)\tCorrection L1FastJet}";
    //header = "{1\tJetEta\t3\tJetPt\tJetA\tRho\tmax(0.0001,1-(y/x)*([0]+[1]*(z-1.519)+[2]*pow(z-1.519,2)))\tCorrection L1FastJet}";
    header = "{1 JetEta 3 JetPt JetA Rho  max(0.0001,1-(y/x)*([0]+[1]*(z-2.00)+[2]*pow(z-2.00,2))) Correction L1FastJet}";
  
  writeMC << header << endl;
  writeData << header << endl;
  
  TF1* f_mc = new TF1("f_mc", "1++x++x*x");
  TF1* f_data = new TF1("f_data", "1++x++x*x");
  f_mc->SetLineColor(2);
  f_mc->SetLineWidth(2);
  f_data->SetLineColor(1);
  f_data->SetLineWidth(2);


  for (int i=0; i<ETA_BINS; i++) {
    vector<double> mc_x, data_x, mc_y, data_y, mc_error, data_error, mc_pull, data_pull, data_to_mc; 

    for (int n=0; n<nPoints; n++){

      double mcX = n1+n+0.5;
      double dataX = n1+n+0.5;

      if (isIndirect){
        mcX = NPUtoRho_mc->GetBinContent( NPUtoRho_mc->FindBin(mcX) );
        dataX = NPUtoRho_data->GetBinContent( NPUtoRho_data->FindBin(dataX) );
      }
      mc_x.push_back( mcX );
      data_x.push_back( dataX );

      mc_y.push_back( mcProfiles[n]->GetBinContent(i+1) );
      data_y.push_back( dataProfiles[n]->GetBinContent(i+1) );
      data_to_mc.push_back( mc_y.back()/data_y.back() );

      mc_error.push_back( 0.02 * mc_y.back() );
      data_error.push_back( 0.02 * data_y.back() );
    }
    TGraphErrors* mcGraph = new TGraphErrors(mc_x.size(), &mc_x[0], &mc_y[0], 0, &mc_error[0]);
    TGraphErrors* dataGraph = new TGraphErrors(data_x.size(), &data_x[0], &data_y[0], 0, &data_error[0]);
    TGraphErrors* datamcGraph = new TGraphErrors(mc_x.size(), &mc_x[0], &data_to_mc[0], 0, 0);

    mcGraph->Fit(f_mc, "Q");
    dataGraph->Fit(f_data, "Q");

    double area = M_PI*R*R;

    double x = fabs(etabins[0]) - 0.5*fabs(etabins[i]+etabins[i+1]);
    if (x < R) {
      double theta = 2*acos(x/R);
      double area_seg = 0.5*R*R*theta - x*R*sin(theta/2);
      area -= area_seg;
    }
    //cout << "Area: " << area << endl;

    writeMC << etabins[i] << setw(8) << etabins[i+1] << setw(8) << 9 << setw(6) << 1 << setw(6) << 3500 << setw(6)
            << 0 << setw(6) << 10 << setw(6) << 0 << setw(6) << 200
            << setw(15) << f_mc->GetParameter(0)/area << setw(15) << f_mc->GetParameter(1)/area
            << setw(15) << f_mc->GetParameter(2)/area << endl;
    writeData << etabins[i] << setw(8) << etabins[i+1] << setw(8) << 9 << setw(6) << 1 << setw(6) << 3500 << setw(6)
              << 0 << setw(6) << 10 << setw(6) << 0 << setw(6) << 200
              << setw(15) << f_data->GetParameter(0)/area << setw(15) << f_data->GetParameter(1)/area
              << setw(15) << f_data->GetParameter(2)/area << endl;


    TString xTitle = "";
    if ( var_type.EqualTo("nPV") ) xTitle = "N_{PV}";
    else if ( var_type.EqualTo("nPU") ) xTitle = "#mu";
    else if ( var_type.EqualTo("indirectRho") ) xTitle = "<#rho> (GeV)";

    if (!pull && !DataOverMC){
      TCanvas* c = new TCanvas("c", "c", 600, 600);
      TH1F* h = new TH1F("h", "h", 100, 0, topX);

      h->GetXaxis()->SetTitle(xTitle);
      h->GetYaxis()->SetTitle("Offset p_{T} (GeV)");
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

      if (pf_type.EqualTo("all"))
        text.DrawLatex(0.17, 0.96, Form("AK%i PF %4.3f #leq #eta #leq %4.3f", Rlabel, etabins[i], etabins[i+1]) );
      else
        text.DrawLatex(0.17, 0.96, Form("AK%i PF%s %4.3f #leq #eta #leq %4.3f", Rlabel, pf_type.Data(), etabins[i], etabins[i+1]) );

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
      text.SetTextSize(0.035);
      text.SetTextFont(42);
      text.SetTextColor(1);
      text.DrawLatex( 0.8, 0.96, "#sqrt{s} = 13 TeV" );

      //cout << "Data: " << f_data->GetParameter(0) << "\t" << f_data->GetParameter(1) << "\t" << f_data->GetParameter(2) << endl;
      //cout << "MC: " << f_mc->GetParameter(0) << "\t" << f_mc->GetParameter(1) << "\t" << f_mc->GetParameter(2) << endl;
      //cout << f_data->GetChisquare() / f_data->GetNDF() << "\t" << f_mc->GetChisquare() / f_mc->GetNDF() << endl;

      c->Print("./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + "/" + outName + pf_type + "_pT_" + var_type + Form("_eta%4.3f", etabins[i]) + ".pdf");
      delete h;
      delete c;
    }

    else if (pull){
      TCanvas* c = new TCanvas("c", "c", 600, 600);
      gStyle->SetOptStat(0);

      TH1F* h1 = new TH1F("h1", "h1", 100, 0, topX);
      TH1F* h2 = new TH1F("h2", "h2", 100, 0, topX);

      float b_scale = 0.3, t_scale = 1 - b_scale;

      TPad* top = new TPad("top", "top", 0, b_scale, 1, 1);
      TPad* bottom = new TPad("bottom", "bottom", 0, 0, 1, b_scale);
      c->cd();

      top->SetTopMargin(0.05);
      top->SetBottomMargin(0.05);
      top->Draw();
      bottom->SetBottomMargin(0.35);
      bottom->SetGridy();
      bottom->Draw();
      top->cd();

      h1->GetXaxis()->SetTitle(xTitle);
      h1->GetXaxis()->SetTickLength(0.03/t_scale);
      h1->GetXaxis()->SetLabelSize(0);

      h1->GetYaxis()->SetTitle("Offset p_{T} (GeV)");
      h1->GetYaxis()->SetTitleSize(0.05/t_scale);
      h1->GetYaxis()->SetTitleOffset(0.9);
      h1->GetYaxis()->SetLabelSize(0.04/t_scale);

      h1->GetYaxis()->SetRangeUser(0, topY);

      h1->Draw();
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

      if (pf_type.EqualTo("all"))
        text.DrawLatex(0.17, 0.96, Form("AK%i PF %4.3f #leq #eta #leq %4.3f", Rlabel, etabins[i], etabins[i+1]) );
      else
        text.DrawLatex(0.17, 0.96, Form("AK%i PF%s %4.3f #leq #eta #leq %4.3f", Rlabel, pf_type.Data(), etabins[i], etabins[i+1]) );

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
      text.SetTextSize(0.035);
      text.SetTextFont(42);
      text.SetTextColor(1);
      text.DrawLatex( 0.8, 0.96, "#sqrt{s} = 13 TeV" );

      ////---- define pull------////
      for (int n=0; n<nPoints; n++){
        mc_pull.push_back( ((mc_y.at(n)- (f_mc->GetParameter(0) + f_mc->GetParameter(1) * (mc_x.at(n)) + f_mc->GetParameter(2)* (mc_x.at(n)) * (mc_x.at(n))) )/ mc_error.at(n)) ); 
        data_pull.push_back( ((data_y.at(n)- (f_data->GetParameter(0) + f_data->GetParameter(1) * (data_x.at(n)) + f_data->GetParameter(2) * (data_x.at(n)) * (data_x.at(n))) )/ data_error.at(n)) );
      }
    
      TGraphErrors* mcGraph_pull = new TGraphErrors(mc_x.size(), &mc_x[0], &mc_pull[0], 0, 0 );
      TGraphErrors* dataGraph_pull = new TGraphErrors(data_x.size(), &data_x[0], &data_pull[0], 0, 0 );

      bottom->cd();

      h2->GetXaxis()->SetTitle(xTitle);
      h2->GetXaxis()->SetLabelSize(0.04/b_scale);
      h2->GetXaxis()->SetTickLength(0.03/b_scale);
      h2->GetXaxis()->SetTitleSize(0.05/b_scale);
      h2->GetXaxis()->SetTitleOffset(0.8);

      h2->GetYaxis()->SetTitle("Pull");
      h2->GetYaxis()->SetTitleOffset(0.3);
      h2->GetYaxis()->SetRangeUser(-3, 3);
      //h2->GetYaxis()->SetNdivisions(3, 3, 0);
      h2->GetYaxis()->SetLabelSize(0.04/b_scale);
      //h2->GetYaxis()->CenterTitle(true);
      h2->GetYaxis()->SetTitleSize(0.05/b_scale);

      h2->Draw();
      //dataGraph_pull->SetMarkerStyle(20);
      //dataGraph_pull->SetMarkerColor(1);
      dataGraph_pull->SetLineColor(kBlack);
      dataGraph_pull->SetLineWidth(1);
      dataGraph_pull->SetFillStyle(0);
      dataGraph_pull->Draw("Bsame");
      //mcGraph_pull->SetMarkerStyle(24);
      //mcGraph_pull->SetMarkerColor(2);
      mcGraph_pull->SetLineColor(kRed);
      mcGraph_pull->SetLineWidth(1);
      mcGraph_pull->SetFillStyle(0);
      mcGraph_pull->Draw("Bsame");

      bottom->SetGrid(0,1);

      c->Print("./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + "/pull/" + outName + pf_type + "_pT_" + var_type + Form("_eta%4.3f", etabins[i]) + "_pull.pdf");
      delete h1;
      delete h2;
      delete c;
    }

    else if (!pull && DataOverMC){
      TCanvas* c = new TCanvas("c", "c", 600, 600);
      TH1F* h = new TH1F("h", "h", 100, 0, topX);
      TH1F* h2 = new TH1F("h2", "h2", 100, 0, topX);

      float b_scale = 0.3, t_scale = 1 - b_scale;
      TPad* top = new TPad("top", "top", 0, b_scale, 1, 1);
      TPad* bottom = new TPad("bottom", "bottom", 0, 0, 1, b_scale);
      c->cd();

      top->SetTopMargin(0.05);
      top->SetBottomMargin(0.05);
      top->Draw();
      bottom->SetBottomMargin(0.35);
      bottom->SetGridy();
      bottom->Draw();
      top->cd();
     
      h->GetXaxis()->SetTitle(xTitle);
      h->GetYaxis()->SetTitle("Offset p_{T} (GeV)");
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

      if (pf_type.EqualTo("all"))
        text.DrawLatex(0.17, 0.96, Form("AK%i PF %4.3f #leq #eta #leq %4.3f", Rlabel, etabins[i], etabins[i+1]) );
      else
        text.DrawLatex(0.17, 0.96, Form("AK%i PF%s %4.3f #leq #eta #leq %4.3f", Rlabel, pf_type.Data(), etabins[i], etabins[i+1]) );

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
      text.SetTextSize(0.035);
      text.SetTextFont(42);
      text.SetTextColor(1);
      text.DrawLatex( 0.8, 0.96, "#sqrt{s} = 13 TeV" );

      //cout << "Data: " << f_data->GetParameter(0) << "\t" << f_data->GetParameter(1) << "\t" << f_data->GetParameter(2) << endl;
      //cout << "MC: " << f_mc->GetParameter(0) << "\t" << f_mc->GetParameter(1) << "\t" << f_mc->GetParameter(2) << endl;
      //cout << f_data->GetChisquare() / f_data->GetNDF() << "\t" << f_mc->GetChisquare() / f_mc->GetNDF() << endl;

      bottom->cd();

      h2->GetXaxis()->SetTitle(xTitle);
      h2->GetXaxis()->SetLabelSize(0.035/b_scale);
      h2->GetXaxis()->SetTickLength(0.03/b_scale);
      h2->GetXaxis()->SetTitleSize(0.05/b_scale);
      h2->GetXaxis()->SetTitleOffset(0.8);

      h2->GetYaxis()->SetTitle("Data/MC");
      h2->GetYaxis()->SetTitleOffset(0.4);
      h2->GetYaxis()->SetRangeUser(0.9, 1.1);
      h2->GetYaxis()->SetNdivisions(5, 3, 0);
      h2->GetYaxis()->SetLabelSize(0.04/b_scale);
      //h2->GetYaxis()->CenterTitle(true);
      h2->GetYaxis()->SetTitleSize(0.05/b_scale);

      h2->Draw();
      datamcGraph->SetMarkerStyle(24);
      datamcGraph->SetMarkerColor(1);
      datamcGraph->Draw("Psame");

      bottom->SetGrid(0,1);
      

      c->Print("./text_files/" + var_type + Form("/R%i/",Rlabel) + pf_type + "/" + outName + pf_type + "_pT_" + var_type + Form("_eta%4.3f", etabins[i]) + ".pdf");
      delete h;
      delete c;
      delete h2;
    }

  }
  writeMC.close();
  writeData.close();
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
