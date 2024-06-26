//Garvita Agarwal 6/15/2020, root -l -b -q l1fastjet_adapted2020.c

//MC L1FastJet parametrization evolutions :
    // before 2017 (original version) max(0.0001,1-z*([0]+([1]*x)*(1+[2]*log(y)))/y)            [x=rho, y=pt, z=A]
    // 2017-2018 (Mikko's version)    max(0.0001,1-y*([1]*(z-[0])*(1+[2]*log(x/30.)))/x)        [x=pt, y=A, z=rho]
    // 2018           max(0.0001,1-(z/y)*([0]+[1]*(x-20.0)+[2]*log(y/30.0)+[3]*pow(log(y/30.0),2)+[4]*(x-20.0)*log(y/30.0)+[5]*(x-20.0)*pow(log(y/30.0),2)) 										                        [x=rho, y=pt, z=A]
    // 2020 Simple    max(0.0001,1-(z/y)*[3]*([1]*(x/[2]-[0])*pow(y,0)))                        [x=rho, y=pt, z=A]
    // 2020 Complex   max(0.0001,1-(z/y)*[7]*([0]+[1]*(x/[6]-20.0)+[2]*log(y/30.0)+[3]*pow(log(y/30.0),2)+[4]*(x/[6]-20.0)*log(y/30.0)+[5]*(x/[6]-20.0)*pow(log(y/30.0),2))) 		        				                [x=rho, y=pt, z=A]
   // 2020 SemiSimple max(0.0001,1-(z/y)*[4]*([1]*(x/[3]-[0])*(1.+[2]*log(y/15.))))             [x=rho, y=pt, z=A]			
	
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

using namespace std;
namespace my {
    string to_string( double d ) {
        ostringstream stm ;
        stm << setprecision(11) << d ;
        return stm.str() ;
    }
}

void setStyle();


void l1fastjet_adapted2020(TString pf_type="chs"){

  const int ETA_BINS = 82;
  double etabins[ETA_BINS+1] =
  {-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, -3.489, -3.314, -3.139, -2.964, -2.853, -2.65,
   -2.5, -2.322, -2.172, -2.043, -1.93, -1.83, -1.74, -1.653, -1.566, -1.479, -1.392, -1.305, -1.218, -1.131, -1.044, -0.957,
   -0.879, -0.783, -0.696, -0.609, -0.522, -0.435, -0.348, -0.261, -0.174, -0.087, 0,
   0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.879, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479,
   1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013,
   4.191, 4.363, 4.538, 4.716, 4.889, 5.191};

  const int MAXRHO = 100;
  bool nPU_derived = true;
  bool rhoCentral = false;

  string run_name; float luminosity; double R; int par;
  cout << "Run: " << " Lumi: " << "Radius:"<< endl;
  cin >> run_name >> luminosity >> R;
  const int Rlabel = R*10;
  cout <<"\n(1)Simple\n(2)SemiSimple\n(3)Complex\nChoice of Parametisation- ";
  cin >> par;
  string para_choice;
  par = par -1;
  if (par == 0) para_choice ="Simple";
  else if (par == 1) para_choice ="SemiSimple";
  else if (par == 2) para_choice ="Complex";
  cout << para_choice << endl;

//Files for UL16

  TFile* mc_root = TFile::Open(Form("/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_MC_Run3%s_Offset_Data_Run2022EFG_R%i.root",run_name.c_str(),Rlabel ));
  TFile* data_root = TFile::Open( Form("/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2022EFG_R%i.root",Rlabel ) );

//  TFile* mc_root = TFile::Open("/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_MC_Run3Summer23BPix_Offset_Data_Run2023D_v2_R4.root");
//  TFile* data_root = TFile::Open("/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2023D-PromptReco-v1_R4.root" );

//  ifstream scale_file( Form("textfiles_Run3/scalefactor/R%i/chs/Run3-%s_DataMC_R%i_DataMcSF_L1RC_AK%iPF", Rlabel, run_name.c_str(), Rlabel, Rlabel) + pf_type + ".txt" );

  ifstream scale_file( Form("textfiles_Run3/scalefactor/R%i/chs/Run3-%s_DataMC_R%i_DataMcSF_L1RC_AK%iPF", Rlabel, run_name.c_str(), Rlabel, Rlabel) + pf_type + ".txt" );

  string pf_type_MC=" ";
  if(pf_type.Contains("chs")) pf_type_MC="chs";

  ifstream mc_file( "Run3_MC_L1FastJet_files/Summer22EERun3_V1_MC_L1FastJet_AK4PFchs.txt" );

  string scale_line, mc_line, mcheader_line;

  //read first line
  getline(scale_file, scale_line);
  getline(mc_file, mcheader_line);
  TString mc_formula = TString(mcheader_line) ;
  double scale_p0[ETA_BINS], scale_p1[ETA_BINS], scale_p2[ETA_BINS];
  double mc_p0[ETA_BINS], mc_p1[ETA_BINS];
  double mc_p2[ETA_BINS], mc_p3[ETA_BINS];
  double mc_p4[ETA_BINS], mc_p5[ETA_BINS];
  string rho_offset_str;
  double rho_offset, scale_rho_offset;

  for (int i=0; getline(scale_file,scale_line); i++){

    string str;
    int delim_pos;

    while (scale_line.at(0) == ' ')  //eta values have space in front of them
      scale_line.erase(0, 1);

    //loop over strings in data line
    for (int col_num=0; (delim_pos = scale_line.find(' ')) != -1; col_num++){

      str = scale_line.substr(0, delim_pos);
      scale_line.erase(0, delim_pos + 1);

      while (scale_line.at(0) == ' ')  //get rid of white space between columns
        scale_line.erase(0, 1);

      if (col_num == 5) scale_p0[i] = stod(str);
      else if (col_num == 6) scale_p1[i] = stod(str);
    }
    //last column after loop
    scale_p2[i] = stod(scale_line);
    //cout << " here " << scale_p0[i] << "\t" << scale_p1[i] << "\t" << scale_p2[i] << endl;
  }
  scale_file.close();

  cout << "MC Formula: " << mc_formula << endl;

  if (mc_formula.Contains("pow(y,0)") && (par == 0)){
    cout << "L1Simple Parametrisation" << endl;
    int lines;
    mc_file.clear();
    mc_file.seekg(0, mc_file.beg);
    getline(mc_file,mc_line);
    for (int i=0; getline(mc_file,mc_line); i++){

      lines = i;
      string str;
      int delim_pos;
      while (mc_line.at(0) == ' '){  //eta values have space in front of them
        mc_line.erase(0, 1);}
      //loop over strings in mc line
      int it1 = -10;
      for (int col_num=0; (delim_pos = mc_line.find(' ')) != -1; col_num++){

        str = mc_line.substr(0, delim_pos);
        mc_line.erase(0, delim_pos + 1);

        while (!mc_line.empty() && mc_line.at(0) == ' ')  //get rid of white space between columns
          mc_line.erase(0, 1);

        if (col_num == 9) mc_p0[i] = stod(str);
        it1 = col_num;
      }
      if ((delim_pos = mc_line.find(' ')) == -1 && it1 == 9) mc_p1[i] = stod(mc_line.substr(0, mc_line.length()));
    }
  }
  else if (mc_formula.Contains("log(y/15.)") && (par == 1)){
    cout << "L1SemiSimple Parametrisation" << endl;
    int lines;
    mc_file.clear();
    mc_file.seekg(0, mc_file.beg);
    getline(mc_file,mc_line);
    for (int i=0; getline(mc_file,mc_line); i++){

      lines = i;
      string str;
      int delim_pos;
      while (mc_line.at(0) == ' ')  //eta values have space in front of them
        mc_line.erase(0, 1);
      //loop over strings in mc line
      int it1 = -10;
      for (int col_num=0; (delim_pos = mc_line.find(' ')) != -1; col_num++){

        str = mc_line.substr(0, delim_pos);
        mc_line.erase(0, delim_pos + 1);

        while (!mc_line.empty() && mc_line.at(0) == ' ')  //get rid of white space between columns
          mc_line.erase(0, 1);

        if (col_num == 9) mc_p0[i] = stod(str);
        if (col_num == 10) mc_p1[i] = stod(str);
        it1 = col_num;
      }
      if ((delim_pos = mc_line.find(' ')) == -1 && it1 == 10) mc_p2[i] = stod(mc_line.substr(0, mc_line.length()));
    }
  }
  else if (mc_formula.Contains("pow(log(y/30.0),2)") && (par == 2)) {
    cout << "L1Complex Parametrisation" << endl;
    unsigned first_delim_pos = string(mc_formula).find("*(x-");
    rho_offset_str = string(mc_formula).substr(first_delim_pos+4,4);
    rho_offset = stod(rho_offset_str);
    int lines;
    mc_file.clear();
    mc_file.seekg(0, mc_file.beg);
    getline(mc_file,mc_line);
    //cout << mc_line <<endl;
    for (int i=0; getline(mc_file,mc_line); i++){

      lines = i;
      string str;
      int delim_pos;
      while (mc_line.at(0) == ' '){  //eta values have space in front of them
        mc_line.erase(0, 1);}
      //loop over strings in mc line
      delim_pos = mc_line.find(' ');
      int it = -10;
      for (int col_num=0; (delim_pos = mc_line.find(' ')) != -1 ; col_num++){
        str = mc_line.substr(0, delim_pos);
        mc_line.erase(0, delim_pos + 1);

        while (!mc_line.empty() && mc_line.at(0) == ' '){  //get rid of white space between columns
          mc_line.erase(0, 1);}
  
        if (col_num == 9) mc_p0[i] = stod(str);
        else if (col_num == 10) mc_p1[i] = stod(str);
        else if (col_num == 11) mc_p2[i] = stod(str);
        else if (col_num == 12) mc_p3[i] = stod(str);
        else if (col_num == 13) mc_p4[i] = stod(str);
        it = col_num;
      }
      if ((delim_pos = mc_line.find(' ')) == -1 && it == 13) mc_p5[i] = stod(mc_line.substr(0, mc_line.length()));
    }
  }
  TString hname;

  if (nPU_derived) hname = "nPU";
  else hname = "nPV";

  TH1F* h_nPU = (TH1F*) data_root->Get(hname);
  double mean_nPU = h_nPU->GetMean();

  int mean_bin = h_nPU->FindBin(mean_nPU);
  int low_bin, high_bin;
  double total_area = h_nPU->Integral();

  for (low_bin = mean_bin; (h_nPU->Integral(low_bin, mean_bin) / total_area) < 0.34; low_bin--){}
  for (high_bin = mean_bin; (h_nPU->Integral(mean_bin, high_bin) / total_area) < 0.34; high_bin++){}

  double nPU_low = h_nPU->GetBinCenter(low_bin);
  double nPU_high = h_nPU->GetBinCenter(high_bin);

  if (nPU_derived){
      if (rhoCentral) hname = "p_rhoCentral0_nPU";
      else hname = "p_rho_nPU";
  }
  else{
      if (rhoCentral) hname = "p_rhoCentral0_nPV";
      else hname = "p_rho_nPV";
  }

  TProfile* data_rho_nPU = (TProfile*) data_root->Get(hname);
  TProfile* mc_rho_nPU = (TProfile*) mc_root->Get(hname);

  TProfile* datatomc_rhos = new TProfile("TP","TP",2*MAXRHO,0,MAXRHO); 
  for (int i=1; i<data_rho_nPU->GetNbinsX()+1 ;i++){ datatomc_rhos->Fill(mc_rho_nPU->GetBinContent(i),data_rho_nPU->GetBinContent(i),1); }
  double rho_data_mcoff = datatomc_rhos->GetBinContent( datatomc_rhos->FindBin( 20 ) );

  double rho_nominal = data_rho_nPU->GetBinContent( data_rho_nPU->FindBin( mean_nPU ) );
  double rho_low = data_rho_nPU->GetBinContent( data_rho_nPU->FindBin( nPU_low ) );
  double rho_high = data_rho_nPU->GetBinContent( data_rho_nPU->FindBin( nPU_high ) );


  double mc_rho_mean = mc_rho_nPU->GetBinContent( mc_rho_nPU->FindBin( mean_nPU ) );
  double mc_rho_low = mc_rho_nPU->GetBinContent( mc_rho_nPU->FindBin( nPU_low ) );
  double mc_rho_high = mc_rho_nPU->GetBinContent( mc_rho_nPU->FindBin( nPU_high ) );

  double eta[ETA_BINS], sf[ETA_BINS], sf_low[ETA_BINS], sf_high[ETA_BINS];
  double new_p0[ETA_BINS], new_p1[ETA_BINS], new_p2[ETA_BINS] ,new_p3[ETA_BINS], new_p4[ETA_BINS], new_p5[ETA_BINS], new_p6[ETA_BINS], new_p7[ETA_BINS];

  for (int i=0; i<ETA_BINS; i++){

    eta[i] = 0.5*(etabins[i]+etabins[i+1]);

    sf_low[i] = scale_p0[i] + scale_p1[i]*rho_low + scale_p2[i]*rho_low*rho_low;
    sf_high[i] = scale_p0[i] + scale_p1[i]*rho_high + scale_p2[i]*rho_high*rho_high;

    sf[i] = scale_p0[i] + scale_p1[i]*rho_nominal + scale_p2[i]*rho_nominal*rho_nominal;

    //cout <<"sf[i]= "<<sf[i]<<endl;
    
    if ( par == 0 ) {
      //cout << "L1Simple" << endl;
      new_p0[i] = mc_p0[i];
      new_p1[i] = mc_p1[i];
      new_p2[i] = (double(rho_nominal) / double(mc_rho_mean));
      new_p3[i] = sf[i];
    }
    else if ( par == 1 ) {
      //cout << "L1SemiSimple" << endl;
      new_p0[i] = mc_p0[i];
      new_p1[i] = mc_p1[i];
      new_p2[i] = mc_p2[i];
      new_p3[i] = (double(rho_nominal) / double(mc_rho_mean));
      new_p4[i] = sf[i];
      //new_p4[i] = scale_p0[i];
      //new_p5[i] = scale_p1[i];
      //new_p6[i] = scale_p2[i];
    }
    else if ( par == 2 ) {
      //cout << "L1Complex" << endl;
      //scale_rho_offset = rho_offset * (double(rho_nominal) / double(mc_rho_mean));
      new_p0[i] = mc_p0[i] ;
      new_p1[i] = mc_p1[i] ; 
      new_p2[i] = mc_p2[i] ;
      new_p3[i] = mc_p3[i] ;
      new_p4[i] = mc_p4[i] ;
      new_p5[i] = mc_p5[i] ;
      new_p6[i] = (double(rho_nominal) / double(mc_rho_mean));
      new_p7[i] = sf[i]; // in the formula all parameter p0-p6 are getting scaled by p7
      /*std::stringstream stream;
      stream << fixed << setprecision(2) << scale_rho_offset;
      rho_offset_str = stream.str();*/
    }
  }
//  ofstream writeFile( Form("Run3_DATA_L1FastJet_files/Run3-%s_L1%s_L1FastJet_AK%iPF", run_name.c_str(), para_choice.c_str(), Rlabel) + pf_type + "_SF_rho.txt");
  ofstream writeFile( Form("Run3_DATA_L1FastJet_files/Run3-%s_L1%s_L1FastJet_AK%iPF", run_name.c_str(), para_choice.c_str(), Rlabel) + pf_type + ".txt");

  if (par==0){
    mc_file.clear();
    mc_file.seekg(0, mc_file.beg);

    //write first line
    getline(mc_file,mc_line);
    boost::replace_all(mc_line, "(x-[0])", "(x/[2]-[0])");
    boost::replace_all(mc_line,"(z/y)","(z/y)*[3]");
    writeFile << mc_line << endl;
    cout <<"Data Formula: "<< mc_line << endl;

    for (int i=0; getline(mc_file,mc_line); i++){

      string str;
      int delim_pos;

      //loop over strings in mc line
      for (int col_num=0; (delim_pos = mc_line.find(' ')) != -1; col_num++){

        str = mc_line.substr(0, delim_pos);
        mc_line.erase(0, delim_pos + 1);

        while (!mc_line.empty() && mc_line.at(0) == ' ')  //get rid of white space between columns
          mc_line.erase(0, 1);
        if (col_num == 3) str = to_string(10);
        if (col_num == 10) str = my::to_string(new_p0[i]);
        int w = (col_num < 9) ? 10 : 17;
        writeFile << right << setprecision(10) << str << setw(w);

      }
      str = my::to_string(new_p1[i]);
      writeFile << right << setprecision(10) << str << setw(17);
      str = my::to_string(new_p2[i]);
      writeFile << right << setprecision(10) << str << setw(17);
      str = my::to_string(new_p3[i]);
      writeFile << right << setprecision(10) << str << endl;
      //writeFile << mc_line << endl;
    }
    mc_file.close();
  }
  else if (par==1){
    mc_file.clear();
    mc_file.seekg(0, mc_file.beg);

    //write first line
    getline(mc_file,mc_line);
    boost::replace_all(mc_line, "(x-[0])", "(x/[3]-[0])");
    boost::replace_all(mc_line,"(z/y)","(z/y)*[4]");
    //boost::replace_all(mc_line,"(z/y)","(z/y)*([4]+[5]*x+[6]*pow(x,2))");  // For calculating SF on the fly.
    writeFile << mc_line << endl;
    cout <<"Data Formula: "<< mc_line << endl;

    for (int i=0; getline(mc_file,mc_line); i++){

      string str;
      int delim_pos;

      //loop over strings in mc line
      for (int col_num=0; (delim_pos = mc_line.find(' ')) != -1; col_num++){

        str = mc_line.substr(0, delim_pos);
        mc_line.erase(0, delim_pos + 1);

        while (!mc_line.empty() && mc_line.at(0) == ' ')  //get rid of white space between columns
          mc_line.erase(0, 1);
        //if (col_num == 3) str = to_string(11);
        if (col_num == 3) str = to_string(13);
        if (col_num == 10) str = my::to_string(new_p0[i]);
        else if (col_num == 11) str = my::to_string(new_p1[i]);
        int w = (col_num < 9) ? 10 : 17;
        writeFile << right << setprecision(10) << str << setw(w);

      }
      str = my::to_string(new_p2[i]);
      writeFile << right << setprecision(10) << str << setw(17);
      str = my::to_string(new_p3[i]);
      writeFile << right << setprecision(10) << str << setw(17);
      str = my::to_string(new_p4[i]);
      writeFile << right << setprecision(10) << str << endl;
      //str = my::to_string(new_p4[i]);
      //writeFile << right << setprecision(10) << str << setw(17);
      //str = my::to_string(new_p5[i]);
      //writeFile << right << setprecision(10) << str << setw(17);
      //str = my::to_string(new_p6[i]);
      //writeFile << right << setprecision(10) << str << endl;
      //writeFile << mc_line << endl;
    }
    mc_file.close();
  }
  else if (par == 2){
    mc_file.clear();
    mc_file.seekg(0, mc_file.beg);

    //write first line
    getline(mc_file,mc_line);
    boost::replace_all(mc_line, "(x-20.0)", "(x/[6]-20.0)");
    boost::replace_all(mc_line,"(z/y)","(z/y)*[7]");
    writeFile << mc_line << endl;
    cout <<"Data Formula: "<<
 mc_line << endl;

    for (int i=0; getline(mc_file,mc_line); i++){

      string str;
      int delim_pos;

      //loop over strings in mc line
      delim_pos = mc_line.find(' ');
      for (int col_num=0; (delim_pos = mc_line.find(' ')) != -1; col_num++){

        str = mc_line.substr(0, delim_pos);
        mc_line.erase(0, delim_pos + 1);

        while (!mc_line.empty() && mc_line.at(0) == ' ')  //get rid of white space between columns
          mc_line.erase(0, 1);
        if (col_num == 3) str = to_string(14);
        if (col_num == 10) str = my::to_string(new_p0[i]);
        else if (col_num == 11) str = my::to_string(new_p1[i]);
        else if (col_num == 12) str = my::to_string(new_p2[i]);
        else if (col_num == 13) str = my::to_string(new_p3[i]);
        else if (col_num == 14) str = my::to_string(new_p4[i]);
        int w2 = (col_num < 9) ? 10 : 17;
        writeFile << right << setprecision(10) << str << setw(w2);
      }
        str = my::to_string(new_p5[i]);
        writeFile << right << setprecision(10) << str << setw(17);
        str = my::to_string(new_p6[i]);
        writeFile << right << setprecision(10) << str << setw(17);
        str = my::to_string(new_p7[i]);
        writeFile << right << setprecision(10) << str << endl;
        //writeFile << mc_line << endl;
    }
    mc_file.close();
  }
  writeFile.close();

  setStyle();

  TGraphErrors* graph = new TGraphErrors(ETA_BINS, eta, sf, 0, 0);
  TGraphErrors* g_high = new TGraphErrors(ETA_BINS, eta, sf_high, 0, 0);
  TGraphErrors* g_low = new TGraphErrors(ETA_BINS, eta, sf_low, 0, 0);  

  TCanvas* c = new TCanvas("c", "c", 600, 600);
  TH1F* h = new TH1F("h", "h", ETA_BINS, etabins);

  h->GetXaxis()->SetTitle("#eta");
  h->GetYaxis()->SetTitle("Scale Factor");
  h->GetYaxis()->SetTitleOffset(1.2);
  if(pf_type=="all") h->GetYaxis()->SetRangeUser(0.6, 1.3);
  else h->GetYaxis()->SetRangeUser(0.6, 1.6);
  h->Draw();

  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1.25);
  graph->SetMarkerColor(1);
  graph->Draw("psame");

  g_high->SetMarkerStyle(2);
  g_high->SetMarkerSize(1.25);
  g_high->SetMarkerColor(kRed);
  g_high->Draw("psame");

  g_low->SetMarkerStyle(5);
  g_low->SetMarkerSize(1.25);
  g_low->SetMarkerColor(kBlue);
  g_low->Draw("psame");

  TLatex text;
  text.SetNDC();
  text.SetTextSize(0.045);

  if (pf_type.EqualTo("all"))
    text.DrawLatex(0.17, 0.96, Form("AK%i PF", Rlabel));
  else
    text.DrawLatex(0.17, 0.96, Form("AK%i PF%s", Rlabel, pf_type.Data()));

  string var_string;
  if (nPU_derived) var_string = "#mu";
  else var_string = "N_{PV}";

  string rho_string = "";
  if (rhoCentral) rho_string = "central, ";

  float leg = 0.35;

  text.DrawLatex(0.45, leg, Form( "#bf{%s = %4.1f^{ #color[2]{%4.1f}}_{ #color[4]{%4.1f}}}" , var_string.c_str(), mean_nPU, nPU_high, nPU_low ) );
  text.DrawLatex(0.45, leg-0.07, Form( "#bf{#rho_{%sData} = %4.1f^{ #color[2]{%4.1f}}_{ #color[4]{%4.1f}} GeV}" , rho_string.c_str(), rho_nominal, rho_high, rho_low ) );
  text.DrawLatex(0.45, leg-0.14, Form( "#bf{#rho_{%sMC} = %4.1f^{ #color[2]{%4.1f}}_{ #color[4]{%4.1f}} GeV}" , rho_string.c_str(), mc_rho_mean, mc_rho_high, mc_rho_low ) );

  text.SetTextSize(0.03);
  text.SetTextColor(2);
  text.SetTextFont(42);
  text.DrawLatex(0.4, 0.96, Form("Run3-%s (13.6 TeV)", run_name.c_str()) );

//  c->Print(Form("Run3_DATA_L1FastJet_files/Run3-%s_L1", run_name.c_str()) +para_choice+"_L1fastjet_scalefactor_eta_" + Form("AK%i",Rlabel) + "PF" + pf_type + ".pdf" );
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
