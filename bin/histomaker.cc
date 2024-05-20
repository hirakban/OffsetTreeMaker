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
const double etabins[nEta+1] =
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
bool dojetVetoMap = true;					//newline
TString jetVetoMapFileName;                     		//newline
//TString mapName2 = "jetvetomap_all";				//newline
TString mapName2 = "jetvetomap_nobpix";				//newline
bool writeEnergyDeposition = true;

map<TString, TH1*> m_Histos1D;
map<TString, TH2*> m_Histos2D;
map<TString, TProfile*> m_Profiles;
map<TString, TProfile2D*> m_Profiles2D;

const int MAXNPU = 100;
const int MAXNPV = 100;
const int MAXRHO = 100;

int getEtaIndex(float eta);

int main(int argc, char* argv[]) {

  bool isMC = false;
  if (string(argv[1]) == "true") isMC = true;
  float rCone = stof( argv[2] );

  TString dataName = argv[3];

  //Open Files//

  TString inName = isMC ? argv[4] : dataName;
  jetVetoMapFileName = isMC ? argv[5] : argv[4];
  cout << "JetVetoMap used: " << jetVetoMapFileName << endl;

  TString outName = "histomaker_outputs/" + inName( inName.Last('/')+1, inName.Last('.')-inName.Last('/')-1 );
  if (isMC){
    TString add_dataName = dataName(dataName.Last('/')+1, dataName.Last('.')-dataName.Last('/')-1);
    cout << add_dataName << endl;
    outName += "_" + add_dataName ;}
  outName += "_R" + to_string( int(rCone*10) ) + ".root";
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


    //------Modification for hcal depth analysis--------
    if (writeEnergyDeposition){
      hname = Form("nh_p_mikko_eta_nPU%i_ECAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_mikko_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_mikko_eta_nPU%i_HO",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_mikko_eta_nPU%i_rawECAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_mikko_eta_nPU%i_rawHCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_mikko_eta_nPU%i_rawHO",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

      hname = Form("nh_p_fract_depth1_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_fract_depth2_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_fract_depth3_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_fract_depth4_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_fract_depth5_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_fract_depth6_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_p_fract_depth7_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

      hname = Form("nhE_p_fract_depth1_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhE_p_fract_depth2_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhE_p_fract_depth3_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhE_p_fract_depth4_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhE_p_fract_depth5_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhE_p_fract_depth6_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhE_p_fract_depth7_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);


      hname = Form("nh_value_depth1_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_value_depth2_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_value_depth3_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_value_depth4_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_value_depth5_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_value_depth6_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nh_value_depth7_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);


      hname = Form("ne_p_mikko_eta_nPU%i_ECAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("ne_p_mikko_eta_nPU%i_HCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("ne_p_mikko_eta_nPU%i_HO",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("ne_p_mikko_eta_nPU%i_rawECAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("ne_p_mikko_eta_nPU%i_rawHCAL",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("ne_p_mikko_eta_nPU%i_rawHO",i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

      hname = Form("nhplusne_p_mikko_eta_nPU%i_HCAL", i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhplusne_p_mikko_eta_nPU%i_ECAL", i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhplusne_p_mikko_eta_nPU%i_HO", i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

      hname = Form("nhplusne_p_mikko_eta_nPU%i_rawHCAL", i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhplusne_p_mikko_eta_nPU%i_rawECAL", i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);
      hname = Form("nhplusne_p_mikko_eta_nPU%i_rawHO", i_nPU);
      m_Profiles[hname] = new TProfile(hname, hname, nEta, etabins);

      hname = Form("N_EH_nPU%i_", i_nPU);
      m_Histos1D[hname] = new TH1D (hname,hname,nEta,etabins);
      hname = Form("N_HH_nPU%i_", i_nPU);
      m_Histos1D[hname] = new TH1D (hname,hname,nEta,etabins);
      hname = Form("2drawEtorawH_EH_nPU%i", i_nPU);
      m_Histos2D[hname] =  new TH2F (hname,hname,nEta,etabins,100,0.0,20);
      hname = Form("profilerawEtorawH_EH_nPU%i", i_nPU);
      m_Profiles[hname] =  new TProfile (hname,hname,nEta,etabins);
      hname = Form("2drawEtorawH_HH_nPU%i", i_nPU);
      m_Histos2D[hname] =  new TH2F (hname,hname,nEta,etabins,100,0.0,20);
      hname = Form("profilerawEtorawH_HH_nPU%i", i_nPU);
      m_Profiles[hname] =  new TProfile (hname,hname,nEta,etabins);
    }
  }

  if (writeEnergyDeposition){

    hname = "nh_ratio_energy";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_ratio_ECAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_ratio_HCAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_ratio_HO";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_ratio_rawECAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_ratio_rawHCAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_ratio_rawHO";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "nh_rawEtorawH";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,100,0.0,20);

    hname = "profile_nh_ratio_energy";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_ratio_ECAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_ratio_HCAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_ratio_HO";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_ratio_rawECAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_ratio_rawHCAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_ratio_rawHO";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_nh_rawEtorawH";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);


    hname = "ne_ratio_energy";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_ratio_ECAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_ratio_HCAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_ratio_HO";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_ratio_rawECAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_ratio_rawHCAL";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_ratio_rawHO";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,330,0.0,3.3);
    hname = "ne_rawEtorawH";
    m_Histos2D[hname] = new TH2F (hname,hname,nEta,etabins,100,0.0,20);

    hname = "profile_ne_ratio_energy";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_ratio_ECAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_ratio_HCAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_ratio_HO";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_ratio_rawECAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_ratio_rawHCAL";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_ratio_rawHO";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
    hname = "profile_ne_rawEtorawH";
    m_Profiles[hname] = new TProfile (hname,hname,nEta,etabins);
  }
//--------------------------------------------------------------------------------------

  //hname = "nPV_all";
  //m_Histos1D[hname] = new TH1F(hname,hname,MAXNPV,0,MAXNPV);
  //hname = "pv_z";
  //m_Histos1D[hname] = new TH1F(hname,hname,100,-50,50);
  //hname = "2h_nPV_nPU";
  //m_Histos2D[hname] = new TH2F(hname,hname,2*MAXNPU,0,MAXNPU,MAXNPV,0,MAXNPV);
  //hname = "2h_rho_nPU";
  //m_Histos2D[hname] = new TH2F(hname,hname,2*MAXNPU,0,MAXNPU,2*MAXRHO,0,MAXRHO);
  //hname = "2h_rho_nPV";
  //m_Histos2D[hname] = new TH2F(hname,hname,MAXNPV,0,MAXNPV,2*MAXRHO,0,MAXRHO);
  hname = "nPV";
  m_Histos1D[hname] = new TH1F(hname,hname,MAXNPV,0,MAXNPV);
  hname = "nPU";
  m_Histos1D[hname] = new TH1F(hname,hname,2*MAXNPU,0,MAXNPU);
  hname = "rho";
  m_Histos1D[hname] = new TH1F(hname,hname,2*MAXRHO,0,MAXRHO);
  hname = "p_nPV_nPU";
  m_Profiles[hname] = new TProfile(hname,hname,2*MAXNPU,0,MAXNPU);
  hname = "p_rho_nPU";
  m_Profiles[hname] = new TProfile(hname,hname,2*MAXNPU,0,MAXNPU);
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
  int nJets=4;
  float jet_pt[nJets], jet_eta[nJets];
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


//--------Modification for hcal depth analysis---------


  vector<double> *nh_eta = 0;
  vector<double> *nh_energy = 0;
  vector<double> *nh_ECAL = 0;
  vector<double> *nh_HCAL = 0;
  vector<double> *nh_rawECAL = 0;
  vector<double> *nh_rawHCAL = 0;
  vector<double> *nh_HO = 0;
  vector<double> *nh_rawHO = 0;

  vector<double> *ne_eta = 0;
  vector<double> *ne_energy = 0;
  vector<double> *ne_ECAL = 0;
  vector<double> *ne_HCAL = 0;
  vector<double> *ne_rawECAL = 0;
  vector<double> *ne_rawHCAL = 0;
  vector<double> *ne_HO = 0;
  vector<double> *ne_rawHO = 0;

  float nh_sum_HCAL[nEta], nh_sum_ECAL[nEta], nh_sum_HO[nEta];
  float nh_sum_rawHCAL[nEta], nh_sum_rawECAL[nEta], nh_sum_rawHO[nEta];

  float nh_HCAL_depth1_fraction_SUM[nEta], nh_HCAL_depth2_fraction_SUM[nEta], nh_HCAL_depth3_fraction_SUM[nEta], nh_HCAL_depth4_fraction_SUM[nEta],
        nh_HCAL_depth5_fraction_SUM[nEta], nh_HCAL_depth6_fraction_SUM[nEta], nh_HCAL_depth7_fraction_SUM[nEta];

  vector<double> *nh_HCAL_depth1_fraction = 0;
  vector<double> *nh_HCAL_depth2_fraction = 0;
  vector<double> *nh_HCAL_depth3_fraction = 0;
  vector<double> *nh_HCAL_depth4_fraction = 0;
  vector<double> *nh_HCAL_depth5_fraction = 0;
  vector<double> *nh_HCAL_depth6_fraction = 0;
  vector<double> *nh_HCAL_depth7_fraction = 0;

  float ne_sum_HCAL[nEta], ne_sum_ECAL[nEta], ne_sum_HO[nEta];
  float ne_sum_rawHCAL[nEta], ne_sum_rawECAL[nEta], ne_sum_rawHO[nEta];

  if (writeEnergyDeposition){

    tree->SetBranchAddress("nh_eta", &nh_eta);
    tree->SetBranchAddress("nh_energy", &nh_energy);
    tree->SetBranchAddress("nh_ECAL", &nh_ECAL);
    tree->SetBranchAddress("nh_HCAL", &nh_HCAL);
    tree->SetBranchAddress("nh_rawECAL", &nh_rawECAL);
    tree->SetBranchAddress("nh_rawHCAL", &nh_rawHCAL);
    tree->SetBranchAddress("nh_HO", &nh_HO);
    tree->SetBranchAddress("nh_rawHO", &nh_rawHO);

    tree->SetBranchAddress("ne_eta", &ne_eta);
    tree->SetBranchAddress("ne_energy", &ne_energy);
    tree->SetBranchAddress("ne_ECAL", &ne_ECAL);
    tree->SetBranchAddress("ne_HCAL", &ne_HCAL);
    tree->SetBranchAddress("ne_rawECAL", &ne_rawECAL);
    tree->SetBranchAddress("ne_rawHCAL", &ne_rawHCAL);
    tree->SetBranchAddress("ne_HO", &ne_HO);
    tree->SetBranchAddress("ne_rawHO", &ne_rawHO);

    tree->SetBranchAddress("nh_HCAL_depth1_fraction",  &nh_HCAL_depth1_fraction);
    tree->SetBranchAddress("nh_HCAL_depth2_fraction",  &nh_HCAL_depth2_fraction);
    tree->SetBranchAddress("nh_HCAL_depth3_fraction",  &nh_HCAL_depth3_fraction);
    tree->SetBranchAddress("nh_HCAL_depth4_fraction",  &nh_HCAL_depth4_fraction);
    tree->SetBranchAddress("nh_HCAL_depth5_fraction",  &nh_HCAL_depth5_fraction);
    tree->SetBranchAddress("nh_HCAL_depth6_fraction",  &nh_HCAL_depth6_fraction);
    tree->SetBranchAddress("nh_HCAL_depth7_fraction",  &nh_HCAL_depth7_fraction);


    tree->SetBranchAddress("nh_sum_HCAL", nh_sum_HCAL);
    tree->SetBranchAddress("nh_sum_ECAL", nh_sum_ECAL);
    tree->SetBranchAddress("nh_sum_HO", nh_sum_HO);
    tree->SetBranchAddress("nh_sum_rawHCAL", nh_sum_rawHCAL);
    tree->SetBranchAddress("nh_sum_rawECAL", nh_sum_rawECAL);
    tree->SetBranchAddress("nh_sum_rawHO", nh_sum_rawHO);


    tree->SetBranchAddress("nh_HCAL_depth1_fraction_SUM",  nh_HCAL_depth1_fraction_SUM);
    tree->SetBranchAddress("nh_HCAL_depth2_fraction_SUM",  nh_HCAL_depth2_fraction_SUM);
    tree->SetBranchAddress("nh_HCAL_depth3_fraction_SUM",  nh_HCAL_depth3_fraction_SUM);
    tree->SetBranchAddress("nh_HCAL_depth4_fraction_SUM",  nh_HCAL_depth4_fraction_SUM);
    tree->SetBranchAddress("nh_HCAL_depth5_fraction_SUM",  nh_HCAL_depth5_fraction_SUM);
    tree->SetBranchAddress("nh_HCAL_depth6_fraction_SUM",  nh_HCAL_depth6_fraction_SUM);
    tree->SetBranchAddress("nh_HCAL_depth7_fraction_SUM",  nh_HCAL_depth7_fraction_SUM);


    tree->SetBranchAddress("ne_sum_HCAL", ne_sum_HCAL);
    tree->SetBranchAddress("ne_sum_ECAL", ne_sum_ECAL);
    tree->SetBranchAddress("ne_sum_HO", ne_sum_HO);
    tree->SetBranchAddress("ne_sum_rawHCAL", ne_sum_rawHCAL);
    tree->SetBranchAddress("ne_sum_rawECAL", ne_sum_rawECAL);
    tree->SetBranchAddress("ne_sum_rawHO", ne_sum_rawHO);

  }

//--------------------------------------------------------------------------------


  // JetVetoMap implementation

  double faulty_area[nEta] = {0.} ;
  double fraction[nEta];  fill_n(fraction, nEta, 1.);
    int il=0, ir=0 ;

  vector<vector<double>> JetVetoMap ;
  if (dojetVetoMap){
    jetVetoMap( jetVetoMapFileName, mapName2); // mapName6
    JetVetoMap = getVetoMap();

    for(int j=0;  j != int((JetVetoMap[0]).size());  ++j){  // loop over faulty patches ...  
      double delta_phi = JetVetoMap[3][j] - JetVetoMap[2][j] ;
      double delta_eta = 0. ;
      //cout << j << "\t" << JetVetoMap[0][j] << endl;
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
    int entry_num = tree->GetEntry(n);
    if (n % 10000 == 0){ cout << "Processing Event: " << n+1 << ", Total events = "<< entry_num <<endl;
    }
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
    FillProfile("p_nPV_nPU", mu, nPV, weight);
    FillProfile("p_rho_nPU", mu, rho, weight);
    FillProfile("p_rho_nPV", nPV, rho, weight);


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

          
         //--------Modification for hcal depth analysis-----------------
          if (writeEnergyDeposition){
             
            double nh_mikko_offpt_HCAL = nh_sum_HCAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_mikko_eta_nPU%i_HCAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_HCAL, weight);
            double nh_mikko_offpt_ECAL = nh_sum_ECAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_mikko_eta_nPU%i_ECAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_ECAL, weight);
            double nh_mikko_offpt_HO = nh_sum_HO[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_mikko_eta_nPU%i_HO", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_HO, weight);

            double nh_mikko_offpt_rawHCAL = nh_sum_rawHCAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_mikko_eta_nPU%i_rawHCAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_rawHCAL, weight);
            double nh_mikko_offpt_rawECAL = nh_sum_rawECAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_mikko_eta_nPU%i_rawECAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_rawECAL, weight);
            double nh_mikko_offpt_rawHO = nh_sum_rawHO[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_mikko_eta_nPU%i_rawHO", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_rawHO, weight);

            double nh_depth1_offpt_HCAL = nh_HCAL_depth1_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth1_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth1_offpt_HCAL, weight);
            double nh_depth1_offE_HCAL = nh_HCAL_depth1_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth1_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth1_offE_HCAL, weight);

            double nh_depth2_offpt_HCAL = nh_HCAL_depth2_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth2_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth2_offpt_HCAL, weight);
            double nh_depth2_offE_HCAL = nh_HCAL_depth2_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth2_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth2_offE_HCAL, weight);

            double nh_depth3_offpt_HCAL = nh_HCAL_depth3_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth3_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth3_offpt_HCAL, weight);
            double nh_depth3_offE_HCAL = nh_HCAL_depth3_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth3_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth3_offE_HCAL, weight);

            double nh_depth4_offpt_HCAL = nh_HCAL_depth4_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth4_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth4_offpt_HCAL, weight);
            double nh_depth4_offE_HCAL = nh_HCAL_depth4_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth4_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth4_offE_HCAL, weight);

            double nh_depth5_offpt_HCAL = nh_HCAL_depth5_fraction_SUM[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth5_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth5_offpt_HCAL, weight);
            double nh_depth5_offE_HCAL = nh_HCAL_depth5_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth5_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth5_offE_HCAL, weight);

            double nh_depth6_offpt_HCAL = nh_HCAL_depth6_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth6_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth6_offpt_HCAL, weight);
            double nh_depth6_offE_HCAL = nh_HCAL_depth6_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth6_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth6_offE_HCAL, weight);

            double nh_depth7_offpt_HCAL = nh_HCAL_depth7_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("nh_p_fract_depth7_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth7_offpt_HCAL, weight);
            double nh_depth7_offE_HCAL = nh_HCAL_depth7_fraction_SUM[ieta]  * 1 * M_PI*rCone*rCone / 1. / area;
            hname = Form("nhE_p_fract_depth7_eta_nPU%i_HCAL",intmu);
            FillProfile(hname, eta, nh_depth7_offE_HCAL, weight);


            double ne_mikko_offpt_HCAL = ne_sum_HCAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("ne_p_mikko_eta_nPU%i_HCAL", intmu);
            FillProfile(hname, eta, ne_mikko_offpt_HCAL, weight);
            double ne_mikko_offpt_ECAL = ne_sum_ECAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("ne_p_mikko_eta_nPU%i_ECAL", intmu);
            FillProfile(hname, eta, ne_mikko_offpt_ECAL, weight);
            double ne_mikko_offpt_HO = ne_sum_HO[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("ne_p_mikko_eta_nPU%i_HO", intmu);
            FillProfile(hname, eta, ne_mikko_offpt_HO, weight);

            double ne_mikko_offpt_rawHCAL = ne_sum_rawHCAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("ne_p_mikko_eta_nPU%i_rawHCAL", intmu);
            FillProfile(hname, eta, ne_mikko_offpt_rawHCAL, weight);
            double ne_mikko_offpt_rawECAL = ne_sum_rawECAL[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("ne_p_mikko_eta_nPU%i_rawECAL", intmu);
            FillProfile(hname, eta, ne_mikko_offpt_rawECAL, weight);
            double ne_mikko_offpt_rawHO = ne_sum_rawHO[ieta] * 1 * M_PI*rCone*rCone / 1. / cosh(eta) / area;
            hname = Form("ne_p_mikko_eta_nPU%i_rawHO", intmu);
            FillProfile(hname, eta, ne_mikko_offpt_rawHO, weight);

            hname = Form("nhplusne_p_mikko_eta_nPU%i_HCAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_HCAL+ne_mikko_offpt_HCAL, weight);
            hname = Form("nhplusne_p_mikko_eta_nPU%i_ECAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_ECAL+ne_mikko_offpt_ECAL, weight);
            hname = Form("nhplusne_p_mikko_eta_nPU%i_HO", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_HO+ne_mikko_offpt_HO, weight);

            hname = Form("nhplusne_p_mikko_eta_nPU%i_rawHCAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_rawHCAL+ne_mikko_offpt_rawHCAL, weight);
            hname = Form("nhplusne_p_mikko_eta_nPU%i_rawECAL", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_rawECAL+ne_mikko_offpt_rawECAL, weight);
            hname = Form("nhplusne_p_mikko_eta_nPU%i_rawHO", intmu);
            FillProfile(hname, eta, nh_mikko_offpt_rawHO+ne_mikko_offpt_rawHO, weight);
          }     
          //------------------------------------------------------------------------

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
    } // end eta loop
    
    //------------Modification for hcal depth analysis------------------
    if (writeEnergyDeposition){

      for (int inh=0 ; inh<int(nh_energy->size()); inh++){
        int etaIndex = getEtaIndex( (*nh_eta)[inh] );
        if (etaIndex == -1) continue;  
        double eta = etabins[etaIndex] ; 
        //if (fabs(eta)>2.9) { cout << "nh beyond 2.9" << eta << endl;}
        double nh_EovHraw = ((*nh_rawHCAL)[inh]<0.000001)? 1.: double((*nh_rawECAL)[inh])/double((*nh_rawHCAL)[inh]);
        hname = "nh_ratio_energy";
        FillHist2D(hname, eta, double((*nh_energy)[inh])/double((*nh_rawECAL)[inh]+(*nh_rawHCAL)[inh]+(*nh_rawHO)[inh]), weight);
        hname = "nh_ratio_ECAL";
        FillHist2D(hname, eta, double((*nh_ECAL)[inh])/double((*nh_rawECAL)[inh]), weight);
        hname = "nh_ratio_HCAL";
        FillHist2D(hname, eta, double((*nh_HCAL)[inh])/double((*nh_rawHCAL)[inh]), weight);
        hname = "nh_ratio_HO";
        FillHist2D(hname, eta, double((*nh_HO)[inh])/double((*nh_rawHO)[inh]), weight);
        hname = "nh_rawEtorawH";
        FillHist2D(hname, eta, nh_EovHraw, weight);


        hname = "profile_nh_ratio_energy";
        if (double((*nh_rawECAL)[inh]+(*nh_rawHCAL)[inh]+(*nh_rawHO)[inh])!=0){
        FillProfile(hname, eta, double((*nh_energy)[inh])/double((*nh_rawECAL)[inh]+(*nh_rawHCAL)[inh]+(*nh_rawHO)[inh]), weight);}
        hname = "profile_nh_ratio_ECAL";
        if (double((*nh_rawECAL)[inh])!=0){
        FillProfile(hname, eta, double((*nh_ECAL)[inh])/double((*nh_rawECAL)[inh]), weight);}
        hname = "profile_nh_ratio_HCAL";
        if (double((*nh_rawHCAL)[inh])!=0){
        FillProfile(hname, eta, double((*nh_HCAL)[inh])/double((*nh_rawHCAL)[inh]), weight);}
        hname = "profile_nh_ratio_HO";
        if (double((*nh_rawHO)[inh])!=0){
        FillProfile(hname, eta, double((*nh_HO)[inh])/double((*nh_rawHO)[inh]), weight);}
        hname = "profile_nh_rawEtorawH";
        FillProfile(hname, eta, nh_EovHraw, weight);

        hname = Form("nh_value_depth1_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth1_fraction)[inh], weight);
        hname = Form("nh_value_depth2_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth2_fraction)[inh], weight);
        hname = Form("nh_value_depth3_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth3_fraction)[inh], weight);
        hname = Form("nh_value_depth4_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth4_fraction)[inh], weight);
        hname = Form("nh_value_depth5_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth5_fraction)[inh], weight);
        hname = Form("nh_value_depth6_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth6_fraction)[inh], weight);
        hname = Form("nh_value_depth7_eta_nPU%i_HCAL",intmu);
        FillProfile(hname, eta, (*nh_HCAL_depth7_fraction)[inh], weight);

       if ( (*nh_rawECAL)[inh]!= 0) {
         FillHist1D(Form("N_EH_nPU%i_", intmu), eta, weight);
         FillHist2D(Form("2drawEtorawH_EH_nPU%i", intmu), eta, nh_EovHraw, weight);
         FillProfile(Form("profilerawEtorawH_EH_nPU%i", intmu), eta, nh_EovHraw, weight);
       }
       else if ( (*nh_rawECAL)[inh]== 0){
         FillHist1D(Form("N_HH_nPU%i_", intmu), eta, weight);
         FillHist2D(Form("2drawEtorawH_HH_nPU%i", intmu), eta, nh_EovHraw, weight);
         FillProfile(Form("profilerawEtorawH_HH_nPU%i", intmu), eta, nh_EovHraw, weight);
        }
      }

      for (int ine=0 ; ine<int(ne_energy->size()); ine++){
        int etaIndex = getEtaIndex( (*ne_eta)[ine] );
        if (etaIndex == -1) continue;  
        double eta = etabins[etaIndex] ; 
        //if (fabs(eta)>2.9) { cout << "photon beyond 2.9" << eta << endl;}
        double ne_EovHraw = ((*ne_rawHCAL)[ine]<0.000001)? 1.: double((*ne_rawECAL)[ine])/double((*ne_rawHCAL)[ine]);
        hname = "ne_ratio_energy";
        FillHist2D(hname, eta, double((*ne_energy)[ine])/double((*ne_rawECAL)[ine]+(*ne_rawHCAL)[ine]+(*ne_rawHO)[ine]), weight);
        hname = "ne_ratio_ECAL";
        FillHist2D(hname, eta, double((*ne_ECAL)[ine])/double((*ne_rawECAL)[ine]), weight);
        hname = "ne_ratio_HCAL";
        FillHist2D(hname, eta, double((*ne_HCAL)[ine])/double((*ne_rawHCAL)[ine]), weight);
        hname = "ne_ratio_HO";
        FillHist2D(hname, eta, double((*ne_HO)[ine])/double((*ne_rawHO)[ine]), weight);
        hname = "ne_rawEtorawH";
        FillHist2D(hname, eta, ne_EovHraw, weight);

        hname = "profile_ne_ratio_energy";
        if (double((*ne_rawECAL)[ine]+(*ne_rawHCAL)[ine]+(*ne_rawHO)[ine])!=0){
        FillProfile(hname, eta, double((*ne_energy)[ine])/double((*ne_rawECAL)[ine]+(*ne_rawHCAL)[ine]+(*ne_rawHO)[ine]), weight);}
        hname = "profile_ne_ratio_ECAL";
        if (double((*ne_rawECAL)[ine])!=0){
        FillProfile(hname, eta, double((*ne_ECAL)[ine])/double((*ne_rawECAL)[ine]), weight);}
        hname = "profile_ne_ratio_HCAL";
        if (double((*ne_rawHCAL)[ine])!=0){
        FillProfile(hname, eta, double((*ne_HCAL)[ine])/double((*ne_rawHCAL)[ine]), weight);}
        hname = "profile_ne_ratio_HO";
        if (double((*ne_rawHO)[ine])!=0){
        FillProfile(hname, eta, double((*ne_HO)[ine])/double((*ne_rawHO)[ine]), weight);}
        hname = "profile_ne_rawEtorawH";
        FillProfile(hname, eta, ne_EovHraw, weight);
      }
    }
    //---------------------------------------------------------------
    

  } //end event loop

  cout << "Processed all events, writing histograms . . ." << endl;
  //Write Histos//

  TFile* outFile = new TFile(outName,"RECREATE");
  outFile->cd();
  TDirectory* dir = (TDirectory*) outFile;

  for (int i_id=0; i_id<numFlavors; i_id++){
    outFile->mkdir( "offset_nPU/" + ids[i_id] );
    outFile->mkdir( "offset_nPV/" + ids[i_id] );
    outFile->mkdir( "mikko_nPU/"  + ids[i_id] );
  }
  outFile->mkdir( "mikko_nPU/HCAL" );
  outFile->mkdir( "mikko_nPU/ECAL" );
  outFile->mkdir( "mikko_nPU/HO" );

  outFile->mkdir( "mikko_nPU/rawHCAL" );
  outFile->mkdir( "mikko_nPU/rawECAL" );
  outFile->mkdir( "mikko_nPU/rawHO" );
  outFile->mkdir("map/multivseta");

  outFile->mkdir( "ptdensity_/" );
  outFile->mkdir( "offsetMED/" );
  outFile->mkdir( "offsetMEAN/" );
  outFile->mkdir( "mikkoMED/" );
  outFile->mkdir( "mikkoMEAN/" );
  outFile->mkdir( "EH_HH/" );


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
    else if ( hname.Contains("N_") ){
      outFile->cd(outName + ":map/multivseta");
      hid->second->Write();
    }
    else{
      dir->cd(outName+":");
      hid->second->Write();
    }
  }

  for (map<TString, TH2*>::iterator hid = m_Histos2D.begin(); hid != m_Histos2D.end(); hid++){
    hname = hid->first;
    if ( hname.Contains("2drawEtorawH_HH_nPU") || hname.Contains("2drawEtorawH_EH_nPU")){ 
      outFile->cd(outName + ":/EH_HH/");
      hid->second->Write();
    }
    else { 
      dir->cd(outName+":");
      hid->second->Write();
    }
  }

  for (map<TString, TProfile*>::iterator hid = m_Profiles.begin(); hid != m_Profiles.end(); hid++){
    outFile->cd();
    hname = hid->first;

    if ( hname.Contains("p_offset_eta_nPU") ){
      TString id = hname( hname.Last('_')+1, hname.Length() );
      outFile->cd(outName + ":/offset_nPU/" + id);
      hid->second->Write();
    }
    else if ( hname.Contains("p_offset_eta_nPV") ){
      TString id = hname( hname.Last('_')+1, hname.Length() );
      outFile->cd(outName + ":/offset_nPV/" + id);
      hid->second->Write();
    }
    else if ( hname.Contains("p_mikko_eta_nPU") ){
      TString id = hname( hname.Last('_')+1, hname.Length() );
      outFile->cd(outName + ":/mikko_nPU/" + id);
      hid->second->Write();
    }
    else if ( hname.Contains("profilerawEtorawH_HH_nPU") || hname.Contains("profilerawEtorawH_EH_nPU")){
      outFile->cd(outName + ":/EH_HH/");
      hid->second->Write();
    }
    else { 
      dir->cd(outName+":");
      hid->second->Write();
    }
  }

  for (map<TString, TProfile2D*>::iterator hid = m_Profiles2D.begin(); hid != m_Profiles2D.end(); hid++){
    hid->second->Write();
  }

  outFile->Write();
  delete outFile;
  outFile = 0;

  cout << "Histograms saved, finished processing!" << endl;

}

//------------------------------------------------------------------------------------------------------

void FillHist1D(const TString& histName, const Double_t& value, const double& weight) 
{
  map<TString, TH1*>::iterator hid=m_Histos1D.find(histName);
  if (hid!=m_Histos1D.end())
  //  cout << "%FillHist1D -- Could not find histogram with name: " << histName << endl;
//  else
    hid->second->Fill(value, weight);
}

void FillHist2D(const TString& histName, const Double_t& value1, const Double_t& value2, const double& weight) 
{
  map<TString, TH2*>::iterator hid=m_Histos2D.find(histName);
  if (hid!=m_Histos2D.end())
  //  cout << "%FillHist2D -- Could not find histogram with name: " << histName << endl;
//  else
    hid->second->Fill(value1, value2, weight);
}

void FillProfile(const TString& histName, const Double_t& value1, const Double_t& value2, const double& weight) 
{
  map<TString, TProfile*>::iterator hid=m_Profiles.find(histName);
  if (hid!=m_Profiles.end())
  //  cout << "%FillProfile -- Could not find profile with name: " << histName << endl;
//  else
    hid->second->Fill(value1, value2, weight);
}

void FillProfile2D(const TString& histName, const Double_t& value1, const Double_t& value2, const Double_t& value3, const double& weight) 
{
  map<TString, TProfile2D*>::iterator hid=m_Profiles2D.find(histName);
  if (hid!=m_Profiles2D.end())
  //  cout << "%FillProfile2D -- Could not find profile with name: " << histName << endl;
//  else
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

int getEtaIndex(float eta){

  for (int i=0; i<nEta; i++){
    if (etabins[i] <= eta && eta < etabins[i+1]) return i;
  }
  if (eta == etabins[nEta]) return nEta-1;
  else return -1;
}

