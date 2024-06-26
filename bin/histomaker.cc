//Chad Harrington 6/8/2015
//EXECUTE as histomaker isMC rCone dataName mcName
//Example for Data: histmaker false 0.4 Data_tree.root
//              MC: histmaker true  0.4 Data_tree.root MC_tree.root

#include "TFile.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TMath.h"
#include "jetVetoMap.h" //newline

#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <time.h>

using namespace std;

const double pt_cut = 15.;
const int nEta = 82;
const Double_t etabins[nEta+1] =
  {-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, -3.489, -3.314, -3.139, -2.964, -2.853, -2.65,
   -2.5, -2.322, -2.172, -2.043, -1.93, -1.83, -1.74, -1.653, -1.566, -1.479, -1.392, -1.305, -1.218, -1.131, -1.044, -0.957,
   -0.879, -0.783, -0.696, -0.609, -0.522, -0.435, -0.348, -0.261, -0.174, -0.087, 0,
   0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.879, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479,
   1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013,
   4.191, 4.363, 4.538, 4.716, 4.889, 5.191};

void FillHist1D(const TString& histName, const Double_t& value, const double& weight);
void FillHist2D(const TString& histName, const Double_t& value1, const Double_t& value2, const double& weight);
void FillProfile(const TString& histName, const Double_t& value1, const Double_t& value2, const double& weight);
void FillProfile2D(const TString& histName, const Double_t& value1, const Double_t& value2, const Double_t& value3, const double& weight);
void getGeometry(double (&geo)[nEta][nEta], const float& rCone);
double areaS(double R, double x1, double x2);
double dist(double R, double x1, double x2);
bool dojetVetoMap = true;						//newline
//TString jetVetoMapFileName = "hotjets-UL17_v2.root" ;              	//newline	
TString jetVetoMapFileName = "hotjets-UL18.root" ;              	//newline	
//TString mapName2 = "h2hot_ul17_plus_hep17_plus_hbpw89";		//newline
TString mapName2 = "h2hot_ul18_plus_hem1516_and_hbp2m1";		//newline
//TString mapName2 = "h2hot_ul16_plus_hbm2_hbp12_qie11";		//newline
//TString mapName6 = "h2hot_mc";					//newline

map<TString, TH1*> m_Histos1D;
map<TString, TH2*> m_Histos2D;
map<TString, TProfile*> m_Profiles;
map<TString, TProfile2D*> m_Profiles2D;

const int MAXNPU = 100;
const int MAXNPV = 100;
const int MAXRHO = 100;

int main(int argc, char* argv[]) {

  bool isMC = false;
  if (string(argv[1]) == "true") isMC = true;
  float rCone = stof( argv[2] );

  TString dataName = argv[3];

  //Open Files//

  TString inName = isMC ? argv[4] : dataName;
  TString outName = inName( inName.Last('/')+1, inName.Last('.')-inName.Last('/')-1 );
//  outName += "_R" + to_string( int(rCone*10) ) + ".root";
  TString newName = dataName( dataName.Last('/')+1, dataName.Last('.')-dataName.Last('/')-1 );
  outName += "_" + newName + "_R" + to_string( int(rCone*10) ) + ".root";
  cout << "output:" << "\t" << outName << endl;
  cout << "inName:" << "\t" << inName << endl;

  TFile* inFile = TFile::Open(inName);

  TTree* tree = (TTree*) inFile->Get("T");
  Long64_t nEntries = tree->GetEntries();
  cout << nEntries << " Events" << endl;

  //Declare Histos//

  enum Flavor{ chm = 0, chu, nh, ne, hfh, hfe, lep, untrk, numFlavors};
  TString ids[] = {"chm", "chu", "nh", "ne", "hfh", "hfe", "lep", "untrk"};
  TString hname;

  for (int i_id=0; i_id<numFlavors; i_id++){
    for (int i_nPU=0; i_nPU<MAXNPU; i_nPU++){
      hname = Form("p_offset_eta_nPU%i_", i_nPU) + ids[i_id];
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("p_mikko_eta_nPU%i_",i_nPU) + ids[i_id];
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

      hname = Form("h_offset_eta_nPU%i_", i_nPU) + ids[i_id];
      m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);
      hname = Form("h_mikko_eta_nPU%i_",i_nPU) + ids[i_id];
      m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);

    }
    for (int i_nPV=0; i_nPV<MAXNPV; i_nPV++){
      hname = Form("p_offset_eta_nPV%i_", i_nPV) + ids[i_id];
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
    }

    hname = "ptdensity_bb_" + ids[i_id];
    m_Histos1D[hname] = new TH1F(hname,hname,100,0,5);
    hname = "ptdensity_ec1_" + ids[i_id];
    m_Histos1D[hname] = new TH1F(hname,hname,100,0,5);
    hname = "ptdensity_ec2_" + ids[i_id];
    m_Histos1D[hname] = new TH1F(hname,hname,100,0,5);
    hname = "ptdensity_hf_" + ids[i_id];
    m_Histos1D[hname] = new TH1F(hname,hname,100,0,5);
  }
  for (int i_nPU=0; i_nPU<MAXNPU; i_nPU++){
    hname = Form("h_ieta_nPU%i", i_nPU); 
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);

    hname = Form("p_offsetMEDchs_eta_nPU%i", i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
    hname = Form("p_mikkoMEDchs_eta_nPU%i",i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

    hname = Form("h_offsetMEDchs_eta_nPU%i", i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);
    hname = Form("h_mikkoMEDchs_eta_nPU%i",i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);

    hname = Form("p_offsetMEANchs_eta_nPU%i", i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
    hname = Form("p_mikkoMEANchs_eta_nPU%i",i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

    hname = Form("h_offsetMEANchs_eta_nPU%i", i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);
    hname = Form("h_mikkoMEANchs_eta_nPU%i",i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);

    hname = Form("p_offsetMED_eta_nPU%i", i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
    hname = Form("p_mikkoMED_eta_nPU%i",i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

    hname = Form("h_offsetMED_eta_nPU%i", i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);
    hname = Form("h_mikkoMED_eta_nPU%i",i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);

    hname = Form("p_offsetMEAN_eta_nPU%i", i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
    hname = Form("p_mikkoMEAN_eta_nPU%i",i_nPU);
    m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

    hname = Form("h_offsetMEAN_eta_nPU%i", i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);
    hname = Form("h_mikkoMEAN_eta_nPU%i",i_nPU);
    m_Histos1D[hname] = new TH1F(hname, hname, nEta, etabins);

    hname = Form("h2_offsetMEDchs_eta_nPU%i",i_nPU);
    m_Histos2D[hname] = new TH2F(hname, hname, nEta, etabins, 100, 0., 20.);

    hname = Form("h2_offsetMEANchs_eta_nPU%i",i_nPU);
    m_Histos2D[hname] = new TH2F(hname, hname, nEta, etabins, 100, 0., 20.);
  }

  hname = "nPV_all";
  m_Histos1D[hname] = new TH1F(hname,hname,MAXNPV,0,MAXNPV);
  hname = "nPV";
  m_Histos1D[hname] = new TH1F(hname,hname,MAXNPV,0,MAXNPV);
  hname = "pv_z";
  m_Histos1D[hname] = new TH1F(hname,hname,MAXNPV,-MAXNPV*0.5,MAXNPV*0.5);
  hname = "nPU";
  m_Histos1D[hname] = new TH1F(hname,hname,2*MAXNPU,0,MAXNPU);
  hname = "rho";
  m_Histos1D[hname] = new TH1F(hname,hname,2*MAXRHO,0,MAXRHO);

  //hname = "2h_nPV_nPU";
  //m_Histos2D[hname] = new TH2F(hname,hname,2*MAXNPU,0,MAXNPU,MAXNPV,0,MAXNPV);
  hname = "p_nPV_nPU";
  m_Profiles[hname] = new TProfile(hname,hname,2*MAXNPU,0,MAXNPU);

  //hname = "2h_rho_nPU";
  //m_Histos2D[hname] = new TH2F(hname,hname,2*MAXNPU,0,MAXNPU,2*MAXRHO,0,MAXRHO);
  hname = "p_rho_nPU";
  m_Profiles[hname] = new TProfile(hname,hname,2*MAXNPU,0,MAXNPU);

  //hname = "2h_rho_nPV";
  //m_Histos2D[hname] = new TH2F(hname,hname,MAXNPV,0,MAXNPV,2*MAXRHO,0,MAXRHO);
  hname = "p_rho_nPV";
  m_Profiles[hname] = new TProfile(hname,hname,MAXNPV,0,MAXNPV);

  //Get Areas//

  double geo[nEta][nEta];
  getGeometry(geo, rCone);

  //Weighting//

  TH1F* h_weights = 0;
  if (isMC) {

    cout << " Reweighting with respect to:" << "\t" << dataName << endl;
    TFile* dataFile = TFile::Open(dataName);
    TTree* dTree = (TTree*) dataFile->Get("T");

    h_weights = new TH1F("h_weights","h_weights",2*MAXNPU,0,MAXNPU);
    dTree->Draw("mu>>h_weights");

    TH1F* h_muMC = new TH1F("h_muMC","h_muMC",2*MAXNPU,0,MAXNPU);
    tree->Draw("mu>>h_muMC");

    h_weights->Divide(h_muMC);
    h_weights->Scale( 1/ h_weights->GetMaximum() );
  }

  //Set Branches//
/*
  ULong64_t event;
  int run, lumi, bx;

  if (!isMC){
    //parsePileUpJSON2();
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("run", &run);
    tree->SetBranchAddress("lumi", &lumi);
    tree->SetBranchAddress("bx", &bx);
  }

  float eRMS[nEta], et[nEta];
  float rhoC0, rhoCC;
  int nPVall;
  float pv_ndof[nPVall], pv_z[nPVall], pv_rho[nPVall];
  float ht;
  int nJets=4;
  float jet_eta[nJets], jet_phi[nJets], jet_pt[nJets], jet_area[nJets];
  float jet_ch[nJets], jet_nh[nJets], jet_ne[nJets], jet_hfh[nJets], jet_hfe[nJets], jet_lep[nJets];
*/

  UChar_t f[numFlavors][nEta];
  float energy[nEta], etMEAN[nEta], etMED[nEta], etMEANchs[nEta], etMEDchs[nEta];
  int nJets, nPVall;
  float jet_pt[nJets], jet_eta[nJets], pv_z[MAXNPV];
  float mu, rho;
  int nPV;

  tree->SetBranchAddress("energy", energy);
  tree->SetBranchAddress("jet_pt", jet_pt);
  tree->SetBranchAddress("jet_eta", jet_eta);
  tree->SetBranchAddress("nJets", &nJets);
  tree->SetBranchAddress("etMEAN", etMEAN);
  tree->SetBranchAddress("etMEANchs", etMEANchs);
  tree->SetBranchAddress("etMED", etMED);
  tree->SetBranchAddress("etMEDchs", etMEDchs);
  tree->SetBranchAddress("fchm", f[chm]);
  tree->SetBranchAddress("fchu", f[chu]);
  tree->SetBranchAddress("fnh", f[nh]);
  tree->SetBranchAddress("fne", f[ne]);
  tree->SetBranchAddress("fhfh", f[hfh]);
  tree->SetBranchAddress("fhfe", f[hfe]);
  tree->SetBranchAddress("flep", f[lep]);
  tree->SetBranchAddress("funtrk", f[untrk]);
  tree->SetBranchAddress("mu", &mu);
  tree->SetBranchAddress("rho", &rho);
  tree->SetBranchAddress("nPV", &nPV);
  tree->SetBranchAddress("nPVall", &nPVall);
  tree->SetBranchAddress("pv_z", pv_z);

  // JetVetoMap implementation

  double faulty_area[nEta] = {0.} ;
  double fraction[nEta];  fill_n(fraction, nEta, 1.);
    int il=0, ir=0 ;

  vector<vector<double>> JetVetoMap ;
  if (dojetVetoMap){
    jetVetoMap( jetVetoMapFileName, mapName2); //mapName6
    JetVetoMap = getVetoMap();

    for(int j=0;  j != int((JetVetoMap[0]).size());  ++j){  // loop over faulty patches ...  
      double delta_phi = JetVetoMap[3][j] - JetVetoMap[2][j] ;
      double delta_eta = 0. ;

      for(int i = 0; i != nEta; ++i){  // loop over eta strips ...
        if (etabins[i] <= JetVetoMap[0][j] && etabins[i+1] >= JetVetoMap[0][j] ) il  = i ;
        if (etabins[i] <= JetVetoMap[1][j] && etabins[i+1] >= JetVetoMap[1][j] ) ir  = i ;
      }
      delta_eta = min(JetVetoMap[1][j] - JetVetoMap[0][j],  etabins[il+1] - JetVetoMap[0][j] ) ;
      faulty_area[il] += delta_eta * delta_phi ;
      while ( ir > il ){
        il++ ;
        delta_eta = min(JetVetoMap[1][j] - etabins[il], etabins[il+1] - etabins[il]) ;
        faulty_area[il] += delta_eta * delta_phi ;
      }
    }
  }    
  for(int i = 0; i != nEta; ++i){ 
    fraction[i] = 1. - faulty_area[i] / (2. * M_PI *( etabins[i+1] - etabins[i] ));
    //cout << "fraction[" << i << "]= " << fraction[i] << "\t" << "Etabins=" << etabins[i] << endl;
  }

  //Loop Over Entries//

  for (Long64_t n=0; n<nEntries; n++) {
    if (n % 100000 == 0) cout << "Processing Event " << n+1 << endl;
    tree->GetEntry(n);
    //if (jet_pt[0] > pt_cut) continue ; //to only select events with pT,corr < pt_cut
    //cout << "Processing Event " << n+1 << endl;
    for (int ieta=0; ieta<nEta; ieta++){ 
      //cout << "previous energy[" << ieta  <<"] = "<< energy[ieta] << endl; 
      energy[ieta] = energy[ieta]/ fraction[ieta] ;
      //cout << "new energy[" << ieta  <<"] = "<< energy[ieta] << endl;
    } 

    float weight = isMC ? h_weights->GetBinContent( h_weights->FindBin(mu) ) : 1.;

    FillHist1D("nPU", mu, weight);
    FillHist1D("nPV", nPV, weight);
    FillHist1D("rho", rho, weight);
    FillHist1D("nPV_all", nPVall, weight);
    FillProfile("p_nPV_nPU", mu, nPV, weight);
    FillProfile("p_rho_nPU", mu, rho, weight);
    FillProfile("p_rho_nPV", nPV, rho, weight);

    for (int i=0; i != nPVall; ++i){ 
      FillHist1D("pv_z", pv_z[i], 1.0);
    }

    int intmu = mu + 0.5;

    for (int ieta=0; ieta<nEta; ieta++){
      double eta = 0.5*(etabins[ieta] + etabins[ieta+1]);

      /*bool jet_overlap = false ;
      for (int i=0; i != nJets; ++i){ 
        if( fabs(eta - jet_eta[i]) < 0.2 && jet_pt[i] > pt_cut)  jet_overlap = true ;
      }
      //cout<< "JetOverlap: "<< jet_overlap <<" eta: " << eta << endl;
      if( jet_overlap) continue;*/
      hname = Form("h_ieta_nPU%i", intmu); 
      FillHist1D(hname, eta, 1.);

      double offsetptMEDchs = 0. , offsetptMEANchs = 0., offsetptMED = 0. , offsetptMEAN = 0.;

      for(int jeta = ieta-10; jeta <= ieta+10; jeta++){
        if( jeta<0 || jeta+1 > nEta) continue;
        double etaC = 0.5*(etabins[jeta] + etabins[jeta+1]);
        offsetptMEDchs += etMEDchs[jeta] * geo[ieta][jeta] * cosh(etaC) * 12.;
        offsetptMEANchs += etMEANchs[jeta] * geo[ieta][jeta] * cosh(etaC) * 12.;
        offsetptMED += etMED[jeta] * geo[ieta][jeta] * cosh(etaC) * 12.;
        offsetptMEAN += etMEAN[jeta] * geo[ieta][jeta] * cosh(etaC) * 12.;
      }
      hname = Form("p_offsetMEDchs_eta_nPU%i", intmu);
      FillProfile(hname, eta, offsetptMEDchs, weight);
      hname = Form("h_offsetMEDchs_eta_nPU%i", intmu);
      FillHist1D(hname, eta, offsetptMEDchs);

      hname = Form("p_offsetMEANchs_eta_nPU%i", intmu);
      FillProfile(hname, eta, offsetptMEANchs, weight);
      hname = Form("h_offsetMEANchs_eta_nPU%i", intmu);
      FillHist1D(hname, eta, offsetptMEANchs);

      hname = Form("p_offsetMED_eta_nPU%i", intmu);
      FillProfile(hname, eta, offsetptMED, weight);
      hname = Form("h_offsetMED_eta_nPU%i", intmu);
      FillHist1D(hname, eta, offsetptMED);

      hname = Form("p_offsetMEAN_eta_nPU%i", intmu);
      FillProfile(hname, eta, offsetptMEAN, weight);
      hname = Form("h_offsetMEAN_eta_nPU%i", intmu);
      FillHist1D(hname, eta, offsetptMEAN);

      if (intmu == 30){ // Here choose the most representative value of mu
        float offsetpt_norm = offsetptMEDchs/mu ;
        if (offsetpt_norm >= 20.) offsetpt_norm = 19.99 ;
        hname = Form("h2_offsetMEDchs_eta_nPU%i", intmu); 
        FillHist2D(hname, eta, offsetpt_norm, 1.);

        offsetpt_norm = offsetptMEANchs/mu ;
        if (offsetpt_norm >= 20.) offsetpt_norm = 19.99 ;
        hname = Form("h2_offsetMEANchs_eta_nPU%i", intmu); 
        FillHist2D(hname, eta, offsetpt_norm, 1.);
      }  

      if (mu > 5){
      double area = M_PI * (etabins[ieta+1] - etabins[ieta]) / 6. ;
      double mikko_offptMEDchs = etMEDchs[ieta] * M_PI*rCone*rCone / area;
      hname = Form("p_mikkoMEDchs_eta_nPU%i", intmu) ;
      FillProfile(hname, eta, mikko_offptMEDchs, weight);
      hname = Form("h_mikkoMEDchs_eta_nPU%i", intmu) ;
      FillHist1D(hname, eta, mikko_offptMEDchs);

      double mikko_offptMEANchs = etMEANchs[ieta] * M_PI*rCone*rCone / area;
      hname = Form("p_mikkoMEANchs_eta_nPU%i", intmu) ;
      FillProfile(hname, eta, mikko_offptMEANchs, weight);
      hname = Form("h_mikkoMEANchs_eta_nPU%i", intmu) ;
      FillHist1D(hname, eta, mikko_offptMEANchs);

      double mikko_offptMED = etMED[ieta] * M_PI*rCone*rCone / area;
      hname = Form("p_mikkoMED_eta_nPU%i", intmu) ;
      FillProfile(hname, eta, mikko_offptMED, weight);
      hname = Form("h_mikkoMED_eta_nPU%i", intmu) ;
      FillHist1D(hname, eta, mikko_offptMED);

      double mikko_offptMEAN = etMEAN[ieta] * M_PI*rCone*rCone / area;
      hname = Form("p_mikkoMEAN_eta_nPU%i", intmu) ;
      FillProfile(hname, eta, mikko_offptMEAN, weight);
      hname = Form("h_mikkoMEAN_eta_nPU%i", intmu) ;
      FillHist1D(hname, eta, mikko_offptMEAN);
      }

      for (int i_id=0; i_id<numFlavors; i_id++){
        double offpt = 0;

        for(int jeta = ieta-10; jeta <= ieta+10; jeta++){
          if( jeta<0 || jeta+1 > nEta) continue;

          offpt += energy[jeta] * geo[ieta][jeta] * f[i_id][jeta] / 255.;
        }
        hname = Form("p_offset_eta_nPU%i_", intmu) + ids[i_id];
        FillProfile(hname, eta, offpt, weight);
        hname = Form("p_offset_eta_nPV%i_", nPV) + ids[i_id];
        FillProfile(hname, eta, offpt, weight);

        hname = Form("h_offset_eta_nPU%i_", intmu) + ids[i_id];
        FillHist1D(hname, eta, offpt);
        //hname = Form("h_offset_eta_nPV%i_", nPV) + ids[i_id];
        //FillHist1D(hname, eta, offpt);


        if (mu > 5) {
          double area = 2 * M_PI * (etabins[ieta+1] - etabins[ieta]);
          double mikko_offpt = energy[ieta] * f[i_id][ieta] * M_PI*rCone*rCone / 255. / cosh(eta) / area;
          hname = Form("p_mikko_eta_nPU%i_", intmu) + ids[i_id];
          FillProfile(hname, eta, mikko_offpt, weight); 
          hname = Form("h_mikko_eta_nPU%i_", intmu) + ids[i_id];
          FillHist1D(hname, eta, mikko_offpt); 
          double ptdensity_permu_in_eta_stripe = mikko_offpt / (M_PI*rCone*rCone) / mu ;
          ptdensity_permu_in_eta_stripe = (ptdensity_permu_in_eta_stripe>5 ? 4.99 : ptdensity_permu_in_eta_stripe) ;
          if(fabs(eta)<1.3){
            FillHist1D("ptdensity_bb_"+ids[i_id], ptdensity_permu_in_eta_stripe, weight);
          } else if(abs(eta)<2.5) {
           FillHist1D("ptdensity_ec1_"+ids[i_id], ptdensity_permu_in_eta_stripe, weight);
          } else if(abs(eta)<3.0) {
            FillHist1D("ptdensity_ec2_"+ids[i_id], ptdensity_permu_in_eta_stripe, weight);
          } else {
            FillHist1D("ptdensity_hf_"+ids[i_id], ptdensity_permu_in_eta_stripe, weight);
          }
        }
      }
    }
  } //end event loop

  //Write Histos//

  TFile* outFile = new TFile(outName,"RECREATE");
  outFile->cd();
  TDirectory* dir = (TDirectory*) outFile;

  for (int i_id=0; i_id<numFlavors; i_id++){
    outFile->mkdir( "offset_nPU/" + ids[i_id] );
    outFile->mkdir( "offset_nPV/" + ids[i_id] );
    outFile->mkdir( "mikko_nPU/"  + ids[i_id] );
  }
  outFile->mkdir( "ptdensity_/" );
  outFile->mkdir( "offsetMED/" );
  outFile->mkdir( "offsetMEAN/" );
  outFile->mkdir( "mikkoMED/" );
  outFile->mkdir( "mikkoMEAN/" );


  for (map<TString, TH1*>::iterator hid = m_Histos1D.begin(); hid != m_Histos1D.end(); hid++){
    hname = hid->first;
    if ( hname.Contains( "ptdensity_" ) ){
      outFile->cd(outName + ":/ptdensity_/" );
      hid->second->Write();
    }
    else if ( hname.Contains( "offsetMED" ) ){
      outFile->cd(outName + ":/offsetMED/" );
      hid->second->Write();
    }
    else if ( hname.Contains( "offsetMEAN" ) ){
      outFile->cd(outName + ":/offsetMEAN/" );
      hid->second->Write();
    }
    else if ( hname.Contains( "mikkoMED" ) ){
      outFile->cd(outName + ":/mikkoMED/" );
      hid->second->Write();
    }
    else if ( hname.Contains( "mikkoMEAN" ) ){
      outFile->cd(outName + ":/mikkoMEAN/" );
      hid->second->Write();
    }
    else{
      dir->cd(outName+":");
      hid->second->Write();
    }
  }
  for (map<TString, TH2*>::iterator hid = m_Histos2D.begin(); hid != m_Histos2D.end(); hid++)
    hid->second->Write();

  for (map<TString, TProfile*>::iterator hid = m_Profiles.begin(); hid != m_Profiles.end(); hid++){
    outFile->cd();
    hname = hid->first;

    if ( hname.Contains("p_offset_eta_nPU") ){
      TString id = hname( hname.Last('_')+1, hname.Length() );
      outFile->cd(outName + ":/offset_nPU/" + id);
    }
    else if ( hname.Contains("p_offset_eta_nPV") ){
      TString id = hname( hname.Last('_')+1, hname.Length() );
      outFile->cd(outName + ":/offset_nPV/" + id);
    }
    else if ( hname.Contains("p_mikko_eta_nPU") ){
      TString id = hname( hname.Last('_')+1, hname.Length() );
      outFile->cd(outName + ":/mikko_nPU/" + id);
    }
    hid->second->Write();
  }

  for (map<TString, TProfile2D*>::iterator hid = m_Profiles2D.begin(); hid != m_Profiles2D.end(); hid++)
    hid->second->Write();

  outFile->Write();
  delete outFile;
  outFile = 0;
}

void FillHist1D(const TString& histName, const Double_t& value, const double& weight) 
{
  map<TString, TH1*>::iterator hid=m_Histos1D.find(histName);
  if (hid==m_Histos1D.end())
    cout << "%FillHist1D -- Could not find histogram with name: " << histName << endl;
  else
    hid->second->Fill(value, weight);
}

void FillHist2D(const TString& histName, const Double_t& value1, const Double_t& value2, const double& weight) 
{
  map<TString, TH2*>::iterator hid=m_Histos2D.find(histName);
  if (hid==m_Histos2D.end())
    cout << "%FillHist2D -- Could not find histogram with name: " << histName << endl;
  else
    hid->second->Fill(value1, value2, weight);
}

void FillProfile(const TString& histName, const Double_t& value1, const Double_t& value2, const double& weight) 
{
  map<TString, TProfile*>::iterator hid=m_Profiles.find(histName);
  if (hid==m_Profiles.end())
    cout << "%FillProfile -- Could not find profile with name: " << histName << endl;
  else
    hid->second->Fill(value1, value2, weight);
}

void FillProfile2D(const TString& histName, const Double_t& value1, const Double_t& value2, const Double_t& value3, const double& weight) 
{
  map<TString, TProfile2D*>::iterator hid=m_Profiles2D.find(histName);
  if (hid==m_Profiles2D.end())
    cout << "%FillProfile2D -- Could not find profile with name: " << histName << endl;
  else
    hid->second->Fill(value1, value2, value3, weight);
}

void getGeometry(double (&geo)[nEta][nEta], const float& rCone){

  for (int ieta=0; ieta<nEta; ieta++){
    double eta = 0.5*(etabins[ieta] + etabins[ieta+1]);

    for(int jeta = ieta-10; jeta <= ieta+10; jeta++){

      if( jeta<0 || jeta+1 > nEta) continue;

      double etaL = etabins[jeta];                    // left  edge of the eta strip
      double etaR = etabins[jeta+1];                  // right edge of the eta strip
      double etaC = 0.5*(etaL+etaR);                  // center of the eta strip
      double A = areaS(rCone, etaL-eta, etaR-eta);    // area of the eta strip inside the cone
      //if (A <= 0.00000001) continue;                //  (A <= 0.)  gives precision issues

      // the next lines make sure that we do vector addition in phi direction;
      // We would be doing scalar addition with coef = 1.

      double dphi = dist(rCone, etaL-eta, etaR-eta);
      double coef = dphi > 0.000001 ? TMath::Sin(dphi) / dphi : 1.;

      geo[ieta][jeta] = A * coef / (2*TMath::Pi()*(etaR-etaL)) / cosh(etaC);
    }
  }
}

double areaS(double R, double x1, double x2){
//
// Area inside a shape delinated by a circle of radius R
// centered at (0,0) and two parallel lines at x=x1 and x=x2
//

   if( R<=0. || x1==x2 ) return 0.;
   if(x1*x2>0 && fabs(x1) > R && fabs(x2) > R) return 0. ;

   double d1 = fabs(x1) ;
   double d2 = fabs(x2) ;
   if (d1>d2){
     double d = d1;
     d1 = d2;
     d2 = d ;
   }

   // area of segment at distance d1 from the center
   double theta1 = 2.* TMath::ACos(d1/R) ;
   double A1 = (0.5*R*R*(theta1-TMath::Sin(theta1))) ;

   // area of segment at distance d2 from the center
   double A2 = 0. ;
   if(d2<=R){
     double theta2 = 2.* TMath::ACos(d2/R) ;
     A2 = (0.5*R*R*(theta2-TMath::Sin(theta2))) ;
   }

   if(x1*x2>=0){ // both lines on the same side from the center
     return A1 - A2 ;
   } else { // the lines on the opposite side from the center
    return TMath::Pi()*R*R - A1 - A2 ;
   }
}

double dist(double R, double x1, double x2){
//
// Take a circle of radius R centered at (0,0).
// This function calculates distance between a
// midpoint of x=x1 and x=x2 and the point on circle rim
// along vertical line.
//

   if( R<=0.) return 0.;
   if(x1*x2>0 && fabs(x1) >= R && fabs(x2) >= R) return 0. ; // both lines outside the circle
                                                             // and on the same side of origin.
  if(fabs(x1)>R) x1 = TMath::Sign(R, x1);
  if(fabs(x2)>R) x2 = TMath::Sign(R, x2);

   double x = 0.5*( x1 + x2) ;

   return TMath::Sqrt(R*R-x*x);
}
