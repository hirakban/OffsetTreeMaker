//Garvita Agarwal April 10, 2019
//OffsetpT_stack.c for HCAL deposit plots
//executable using run_stack.sh

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

enum Depth{depth1=0, depth2, depth3, depth4, depth5, depth6, depth7, all_depths};

enum HCALDeposition{HCAL=0, ECAL, HO, rawHCAL, rawECAL, rawHO, all_HCALDepo};


void offsetpT_stack_depth( TString mcName="SingleNeutrino_MC_R4.root", TString dataName="Legacy_BCD_R4.root", TString bin_var="nPU", Depth depth = all_depths, bool ratio=true, bool do_energy=false,
           TString label="Run 2018B - 0.15 fb^{-1} (13.6 TeV)", TString outfile_name = "") {

  TFile* mcFile = TFile::Open(mcName);
  TFile* dataFile = TFile::Open(dataName);

  TString depths[] = {"depth1", "depth2", "depth3", "depth4", "depth5", "depth6", "depth7", "all_depths"};
  TString HCALDepositions[] ={"HCAL", "ECAL", "HO", "rawHCAL", "rawECAL", "rawHO", "all_HCALDepo"};

  TH1F* h_bin_var = (TH1F*) dataFile->Get(bin_var);
//  int n1 = h_bin_var->GetMean();
  int n1 = 40;        //Change

  vector<TH1D*> v_MC (all_depths);
  vector<TH1D*> v_Data (all_depths);

  TString var = "";
  if(do_energy) var ="E";

  TString hname;
  for (int i=0; i<all_depths; i++) {
    //these plots are already mikko style plots
    hname = Form("nh%s_p_fract_",var.Data()) + depths[i] +Form("_eta_nPU%i_HCAL",n1);
    v_MC[i]   = ((TProfile*) mcFile->FindObjectAny(hname))->ProjectionX(depths[i]+"MC");
    v_Data[i] = ((TProfile*) dataFile->FindObjectAny(hname))->ProjectionX(depths[i]+"Data");

    v_MC[i]->Scale( 1.0 / n1 );
    v_Data[i]->Scale( 1.0 / n1 );
  }

  TH1D* depth1_clone = (TH1D*) v_Data[depth1]->Clone("depth1_clone");
  TH1D* depth2_clone = (TH1D*) v_Data[depth2]->Clone("depth2_clone");
  TH1D* depth3_clone = (TH1D*) v_Data[depth3]->Clone("depth3_clone");
  TH1D* depth4_clone = (TH1D*) v_Data[depth4]->Clone("depth4_clone");
  TH1D* depth5_clone = (TH1D*) v_Data[depth5]->Clone("depth5_clone");
  TH1D* depth6_clone = (TH1D*) v_Data[depth6]->Clone("depth6_clone");
  TH1D* depth7_clone = (TH1D*) v_Data[depth7]->Clone("depth7_clone");

  THStack* mcStack = new THStack();
  THStack* dataStack = new THStack();
  THStack* cloneStack = new THStack();

  for (int i=0; i<all_depths; i++){
    mcStack->Add(v_MC[i]);
    dataStack->Add(v_Data[i]);
  }

  // v_Data is used to draw from 1.0 to 2.95 clone stack is used to draw from -2.95 t0 -1.0
  cloneStack->Add(depth1_clone);
  cloneStack->Add(depth2_clone);
  cloneStack->Add(depth3_clone);
  cloneStack->Add(depth4_clone);
  cloneStack->Add(depth5_clone);
  cloneStack->Add(depth6_clone);
  cloneStack->Add(depth7_clone);

  setStyle();
  TString yTitle= "#LTE_{T,offset}(#pi 0.4^{2})#GT / #LT#mu#GT";

  TCanvas* c = new TCanvas("c", "c", 600, 600);
  gStyle->SetOptStat(0);

  TH1D* h1 = new TH1D("h1", "h1", ETA_BINS, etabins);
  TH1D* h2 = new TH1D("h2", "h2", ETA_BINS, etabins);

  float b_scale = 0.3, t_scale = 1 - b_scale;

  TPad* top = new TPad("top", "top", 0, b_scale, 1, 1);
  TPad* bottom = new TPad("bottom", "bottom", 0, 0, 1, b_scale);

  c->cd();
  if (ratio) {

    top->SetTopMargin(0.05);
    top->SetBottomMargin(0.05);
    top->Draw();
    bottom->SetBottomMargin(0.35);
    bottom->SetGridy();
    bottom->Draw();
    top->cd();
  }

  h1->GetYaxis()->SetTitle(yTitle);

  if (ratio) {
    h1->GetXaxis()->SetTickLength(0.03/t_scale);
    h1->GetXaxis()->SetLabelSize(0);
    h1->GetYaxis()->SetTitleSize(0.05/t_scale);
    h1->GetYaxis()->SetTitleOffset(0.9);
    h1->GetYaxis()->SetLabelSize(0.04/t_scale);
  }
  else {
    h1->GetXaxis()->SetTitle("#eta");
    h1->GetXaxis()->SetTitleSize(0.03/t_scale);
    h1->GetXaxis()->SetTitleOffset(1.1);
    h1->GetXaxis()->SetLabelSize(0.03/t_scale);

    h1->GetYaxis()->SetTitleSize(0.03/t_scale);
    h1->GetYaxis()->SetTitleOffset(1.3);
    h1->GetYaxis()->SetLabelSize(0.03/t_scale);
  }
  h2->GetXaxis()->SetLabelSize(0.04/b_scale);
  h2->GetXaxis()->SetTickLength(0.03/b_scale);
  h2->GetXaxis()->SetTitleSize(0.05/b_scale);
  h2->GetXaxis()->SetTitleOffset(0.8);
  h2->GetXaxis()->SetTitle("#eta");
  h2->GetYaxis()->SetRangeUser(0, 1.6); //chs_Data->GetMaximum()*1.1 );
  h2->GetYaxis()->SetNdivisions(5, 3, 0);
  h2->GetYaxis()->SetLabelSize(0.04/b_scale);
//  h2->GetYaxis()->SetTitle("Data/MC");
  h2->GetYaxis()->SetTitle("2022FG/2022E");      //Change
  h2->GetYaxis()->CenterTitle(true);
  h2->GetYaxis()->SetTitleSize(0.03/b_scale);
  h2->GetYaxis()->SetTitleOffset(0.5);

  h1->Draw();

//  TLegend* leg = new TLegend(.55,.57,.67,.9);
  TLegend* leg = new TLegend(.4,.57,.67,.9);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetTextFont(42);

  TLegend* leg2 = new TLegend(.6,.65,.75,.8);
  leg2->SetBorderSize(0);
  leg2->SetFillColor(0);
  leg2->SetFillStyle(0);
  leg2->SetTextSize(0.06);
  leg2->SetTextFont(42);

  v_MC[depth1] ->SetMarkerStyle(kMultiply);
  v_MC[depth2] ->SetMarkerStyle(kOpenTriangleUp);
  v_MC[depth3] ->SetMarkerStyle(kOpenDiamond);
  v_MC[depth4] ->SetMarkerStyle(kOpenStar);
  v_MC[depth5] ->SetMarkerStyle(kOpenCircle);
  v_MC[depth6] ->SetMarkerStyle(kOpenDoubleDiamond);
  v_MC[depth7] ->SetMarkerStyle(kOpenCross);

  v_Data[depth1] ->SetMarkerStyle(kMultiply);
  v_Data[depth2] ->SetMarkerStyle(kOpenTriangleUp);
  depth2_clone   ->SetMarkerStyle(kOpenTriangleUp);
  v_Data[depth3] ->SetMarkerStyle(kOpenDiamond);
  depth3_clone   ->SetMarkerStyle(kOpenDiamond);
  v_Data[depth4] ->SetMarkerStyle(kOpenStar);
  depth4_clone   ->SetMarkerStyle(kOpenStar);
  v_Data[depth5] ->SetMarkerStyle(kOpenCircle);
  depth5_clone   ->SetMarkerStyle(kOpenCircle);
  v_Data[depth6] ->SetMarkerStyle(kOpenDoubleDiamond);
  depth6_clone   ->SetMarkerStyle(kOpenDoubleDiamond);
  v_Data[depth7] ->SetMarkerStyle(kOpenCross);
  depth7_clone   ->SetMarkerStyle(kOpenCross);
  //for error bars
  v_Data[depth1] ->SetLineColor(kBlack);
  v_Data[depth2] ->SetLineColor(kBlack);
  depth2_clone   ->SetLineColor(kBlack);
  v_Data[depth3] ->SetLineColor(kBlack);
  depth3_clone   ->SetLineColor(kBlack);
  v_Data[depth4] ->SetLineColor(kBlack);
  depth4_clone   ->SetLineColor(kBlack);
  v_Data[depth5] ->SetLineColor(kBlack);
  depth5_clone   ->SetLineColor(kBlack);
  v_Data[depth6] ->SetLineColor(kBlack);
  depth6_clone   ->SetLineColor(kBlack);
  v_Data[depth7] ->SetLineColor(kBlack);
  depth7_clone   ->SetLineColor(kBlack);

  v_MC[depth1] ->SetFillColor(kBlue);
  v_MC[depth2] ->SetFillColor(kViolet+2);
  v_MC[depth3] ->SetFillColor(kGreen);
  v_MC[depth4] ->SetFillColor(kPink+6);
  v_MC[depth5] ->SetFillColor(kRed-9);
  v_MC[depth6] ->SetFillColor(kRed);
  v_MC[depth7] ->SetFillColor(kOrange);

  v_MC[depth1] ->SetLineColor(kBlack);
  v_MC[depth2] ->SetLineColor(kBlack);
  v_MC[depth3] ->SetLineColor(kBlack);
  v_MC[depth4] ->SetLineColor(kBlack);
  v_MC[depth5] ->SetLineColor(kBlack);
  v_MC[depth6] ->SetLineColor(kBlack);
  v_MC[depth7] ->SetLineColor(kBlack);

  //CMS and Lumi Text//

  TLatex text;
  text.SetNDC();

  if (depth == all_depths) {
    v_Data[depth1] ->SetAxisRange(-2.95,2.95);
    v_Data[depth2] ->SetAxisRange(-2.95,2.95);
    v_Data[depth3] ->SetAxisRange(-2.95,2.95);
    v_Data[depth4] ->SetAxisRange(-2.95,2.95);
/*
    v_Data[depth2] ->SetAxisRange(1.0,2.95);
    depth2_clone   ->SetAxisRange(-2.95,-1.0);
    v_Data[depth3] ->SetAxisRange(1.0,2.95);
    depth3_clone   ->SetAxisRange(-2.95,-1.0);
    v_Data[depth4] ->SetAxisRange(1.0,2.95);
    depth4_clone   ->SetAxisRange(-2.95,-1.0);
*/
    v_Data[depth5] ->SetAxisRange(1.0,2.95);
    depth5_clone   ->SetAxisRange(-2.95,-1.0);
    v_Data[depth6] ->SetAxisRange(1.0,2.95);
    depth6_clone   ->SetAxisRange(-2.95,-1.0);
    v_Data[depth7] ->SetAxisRange(1.0,2.95);
    depth7_clone   ->SetAxisRange(-2.95,-1.0);

    h1->GetYaxis()->SetRangeUser( 0, dataStack->GetMaximum()*1.8 ); //0.35  );
    
    h1->GetYaxis()->SetTitle(yTitle);
    h1->GetYaxis()->SetTitleOffset(1.1);

    mcStack->Draw("samehist");
    dataStack->Draw("samepe");
    cloneStack->Draw("samepe");

    leg->SetHeader("#bf{Markers: Run2022FG, Hist: Run2022E}");   //Change
//    leg->SetHeader("#bf{Markers: Data, Histograms: MC}");   //Change
    leg->AddEntry(v_MC[depth1],"Depth 1","PF");
    leg->AddEntry(v_MC[depth2],"Depth 2","PF");
    leg->AddEntry(v_MC[depth3],"Depth 3","PF");
    leg->AddEntry(v_MC[depth4],"Depth 4","PF");
    leg->AddEntry(v_MC[depth5],"Depth 5","PF");
    leg->AddEntry(v_MC[depth6],"Depth 6","PF");
    leg->AddEntry(v_MC[depth7],"Depth 7","PF");

    leg->Draw();

    TString title;
    title ="Neutral Hadrons";
    text.SetTextSize(0.045);
    text.SetTextFont(61);
    text.DrawLatex(0.17, 0.96, title);
    
    text.SetTextSize(0.065);
    text.SetTextFont(61);
    text.DrawLatex(0.22, 0.85, "CMS");

    text.SetTextSize(0.040);
    text.SetTextFont(42);
    text.DrawLatex(0.228, 0.78, Form("#mu = %i",n1));
    text.SetTextSize(0.045);
    

    if (ratio) text.DrawLatex(0.45, 0.96, label);
    else       text.DrawLatex(0.45, 0.96, label);
    gPad->RedrawAxis();

    if (ratio){
      bottom->cd();
      h2->GetYaxis()->SetRangeUser(0,5.0);
      h2->Draw();
      vector<TH1D*> v_ratio(all_depths);
      
      for (int i=0; i<all_depths; i++) {
        v_ratio[i] = (TH1D*) v_Data[i]->Clone("v_ratio[i]");
        v_ratio[i]->Divide(v_MC[i]);
        v_ratio[i]->SetMarkerSize(0.65);
        v_ratio[i]->Draw("sameP");
      }
      v_ratio[depth1] ->SetMarkerColor(kBlue);
      v_ratio[depth2] ->SetMarkerColor(kViolet+2);
      v_ratio[depth3] ->SetMarkerColor(kGreen);
      v_ratio[depth4] ->SetMarkerColor(kPink+6);
      v_ratio[depth5] ->SetMarkerColor(kRed-9);
      v_ratio[depth6] ->SetMarkerColor(kRed);
      v_ratio[depth7] ->SetMarkerColor(kOrange);
      
      v_ratio[depth1] ->SetLineColor(kBlue);
      v_ratio[depth2] ->SetLineColor(kViolet+2);
      v_ratio[depth3] ->SetLineColor(kGreen);
      v_ratio[depth4] ->SetLineColor(kPink+6);
      v_ratio[depth5] ->SetLineColor(kRed-9);
      v_ratio[depth6] ->SetLineColor(kRed);
      v_ratio[depth7] ->SetLineColor(kOrange);
      
      v_ratio[depth1] ->SetAxisRange(-2.95,2.95);
      v_ratio[depth2] ->SetAxisRange(-2.95,2.95);
      v_ratio[depth3] ->SetAxisRange(-2.95,2.95);
      v_ratio[depth4] ->SetAxisRange(-2.95,2.95);
      v_ratio[depth5] ->SetAxisRange(-2.95,2.95);
      v_ratio[depth6] ->SetAxisRange(-2.95,2.95);
      v_ratio[depth7] ->SetAxisRange(-2.95,2.95);
    }
    c->Print("plots/stack_" + depths[depth] + "_" + bin_var + "_" + var + to_string(n1) + "_"+ outfile_name + ".pdf");
    leg->Clear();
    leg2->Clear();
    top->Clear();
    bottom->Clear();
  }
  top->Draw();
  top->cd();
  h1->Draw();

  if (depth != all_depths){

    h1->GetYaxis()->SetRangeUser(0, 0.15);
    h1->GetYaxis()->SetLabelSize(0.06);

    THStack* mcStack2 = new THStack();

    TH1D* hist_MC;
    TH1D* hist_Data;

    hist_MC = v_MC[depth];
    hist_Data = v_Data[depth];
    mcStack2->Add(v_MC[depth]);

    h1->GetYaxis()->SetTitle(yTitle);
    h1->GetYaxis()->SetTitleOffset(1.1);

    hist_MC->Draw("samehist");

    hist_Data->SetMarkerStyle(24);
    hist_Data->SetMarkerColor(kBlack);
    hist_Data->SetAxisRange(-2.95, 2.95);
    hist_Data->Draw("sameP");

    TString title;
    title ="Neutral Hadrons";
    text.SetTextSize(0.045);
    text.SetTextFont(61);
    text.DrawLatex(0.17, 0.96, title);
    
    text.SetTextSize(0.065);
    text.SetTextFont(61);
    text.DrawLatex(0.22, 0.85, "CMS");

    text.SetTextSize(0.040);
    text.SetTextFont(42);
    text.DrawLatex(0.223, 0.78, Form("Depth %i",(depth+1)));
    text.DrawLatex(0.228, 0.71, Form("#mu = %i",n1));
    text.SetTextSize(0.045);

//    leg2->AddEntry(hist_Data,"Data","P");     
//    leg2->AddEntry(hist_MC,"MC","F");
    leg2->AddEntry(hist_Data,"Run2022FG","P");     //Change
    leg2->AddEntry(hist_MC,"Run2022E","F");
    leg2->Draw();

    text.SetTextSize(0.035);
    text.SetTextFont(42);
    if (ratio) text.SetTextSize(0.035);
    text.SetTextFont(42);
    text.DrawLatex(0.55, 0.96, label);
    
    gPad->RedrawAxis();

    if (ratio){
      bottom->cd();
      h2->GetYaxis()->SetRangeUser(0,5.0);
      h2->Draw();
      TH1D* ratio_MC = (TH1D*) hist_MC->Clone("ratio_MC");
      TH1D* ratio_Data = (TH1D*) hist_Data->Clone("ratio_Data");
      ratio_Data->Divide(ratio_MC);  
      ratio_Data->SetAxisRange(-2.9, 2.9);
      ratio_Data->SetMarkerStyle(24);
      ratio_Data->SetMarkerColor(kBlack);
      ratio_Data->Draw("sameP");
    }
  
    c->Print("plots/stack_" + depths[depth] + "_" + bin_var +"_" + var + to_string(n1) + "_"+ outfile_name + ".pdf");
    top->Clear();
    bottom->Clear();
    leg2->Clear();
  }

  //this next block of code gives us the enrgy depositions in HCAL, ECAL and HO for neutral hadrons.

  if (depth == depth1){

    vector<TH1D*> v_MC_nhDepo (all_HCALDepo);
    vector<TH1D*> v_Data_nhDepo (all_HCALDepo);
    vector<TH1D*> ratio_MC_nhDepo (all_HCALDepo);
    vector<TH1D*> ratio_Data_nhDepo (all_HCALDepo);

    // Corrected depositions - hname2
    TString hname2;
    for (int i=0; i<all_HCALDepo; i++) {
      leg->Clear();
      leg2->Clear();
      top->Clear();
      
      top->Draw();
      top->cd(); 
      h1->Draw();

      h1->GetYaxis()->SetRangeUser( 0, 0.1 );
      h1->GetYaxis()->SetLabelSize(0.06);

      h1->GetYaxis()->SetTitle(yTitle);
      h1->GetYaxis()->SetTitleOffset(1.1);

      //THStack* mcStack3 = new THStack();

      hname2 = Form("nh_p_mikko_eta_nPU%i_", n1) + HCALDepositions[i];
      cout <<hname2<<endl;
      v_MC_nhDepo[i]   = ((TProfile*) mcFile->FindObjectAny(hname2))->ProjectionX(HCALDepositions[i]+"MC");
      v_Data_nhDepo[i] = ((TProfile*) dataFile->FindObjectAny(hname2))->ProjectionX(HCALDepositions[i]+"Data");

      v_MC_nhDepo[i]  ->Scale( 1.0 / n1 );
      v_Data_nhDepo[i]->Scale( 1.0 / n1 );
      cout <<v_MC_nhDepo[i]->GetBinContent(19)<<endl;
      cout <<v_Data_nhDepo[i]->GetBinContent(19)<<endl;

      v_MC_nhDepo[i]->Draw("samehist");
      //mcStack3->Add(v_MC_nhDepo[i]);

      v_Data_nhDepo[i]->SetMarkerStyle(24);
      v_Data_nhDepo[i]->SetMarkerColor(kBlack);
      if(i == 2 || i == 5) {v_Data_nhDepo[i]->SetAxisRange(-1.5, 1.5);}
      else v_Data_nhDepo[i]->SetAxisRange(-3., 3.);
      v_Data_nhDepo[i]->Draw("sameP");

      TString title;
      title ="Neutral Hadrons";
      text.SetTextSize(0.045);
      text.SetTextFont(61);
      text.DrawLatex(0.17, 0.96, title);
      
      text.SetTextSize(0.065);
      text.SetTextFont(61);
      text.DrawLatex(0.22, 0.85, "CMS");

      TString depositionType,rawORcorrected;
      if(i == 0 || i == 3) {depositionType = "#bf{HCAL Deposits}"; v_MC_nhDepo[i] ->SetFillColor(kBlue); h1->GetYaxis()->SetRangeUser( 0, v_MC_nhDepo[i]->GetMaximum()*1.5);} //0.4 
      if(i == 1 || i == 4) {depositionType = "#bf{ECAL Deposits}"; v_MC_nhDepo[i] ->SetFillColor(kRed); h1->GetYaxis()->SetRangeUser( 0, v_MC_nhDepo[i]->GetMaximum()*2.5 );} //0.025
      if(i == 2 || i == 5) {depositionType = "#bf{HO Deposits}"; v_MC_nhDepo[i] ->SetFillColor(kGreen); h1->GetYaxis()->SetRangeUser( 0, v_MC_nhDepo[i]->GetMaximum()*1.5 );} //0.012

      text.SetTextSize(0.035);
      text.SetTextFont(42);
      text.DrawLatex(0.6, 0.89, depositionType);

      if(i ==0 || i==1 || i==2) rawORcorrected = "#color[2]{Corrected Deposition}";
      if(i ==3 || i==4 || i==5) rawORcorrected = "#color[2]{Raw Deposition}";
      text.SetTextSize(0.040);
      text.SetTextFont(42);
      text.DrawLatex(0.218, 0.78, rawORcorrected);

      text.DrawLatex(0.228, 0.71, Form("#mu = %i",n1));
      
      text.SetTextSize(0.045);
//      leg2->AddEntry(v_Data_nhDepo[i],"Data","P");
//      leg2->AddEntry(v_MC_nhDepo[i],"MC","F");
      leg2->AddEntry(v_Data_nhDepo[i],"Run2022FG","P");     //Change
      leg2->AddEntry(v_MC_nhDepo[i],"Run2022E","F");
      leg2->Draw();

      text.SetTextSize(0.035);
      text.SetTextFont(42);
      text.DrawLatex(0.55, 0.96, label);
      
      gPad->RedrawAxis();

      if(ratio){
        bottom->cd();
        if(i == 2 || i == 5) {h2->GetYaxis()->SetRangeUser(0.,5.0);}
        else h2->GetYaxis()->SetRangeUser(0,5.0);
        h2->Draw();

        ratio_MC_nhDepo[i]   = (TH1D*) v_MC_nhDepo[i]->Clone("ratio_MC_nhDepo[i]");
        ratio_Data_nhDepo[i] = (TH1D*) v_Data_nhDepo[i]->Clone("ratio_Data_nhDepo[i]");
        ratio_Data_nhDepo[i]->Divide(ratio_MC_nhDepo[i]);

        ratio_Data_nhDepo[i]->SetAxisRange(-3., 3.);
        ratio_Data_nhDepo[i]->SetMarkerStyle(24);
        ratio_Data_nhDepo[i]->SetMarkerColor(kBlack);
        ratio_Data_nhDepo[i]->Draw("sameP");
      }
      c->Print("plots/nh_" + HCALDepositions[i] + "_" + bin_var +"_" + var + to_string(n1) + "_"+ outfile_name + ".pdf");
      bottom->Clear();
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
  tdrStyle->SetTitleYOffset(1.1);

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

  tdrStyle->cd();
}
