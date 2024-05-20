#ifndef __jetVetoMap_C__
#define __jetVetoMap_C__

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <utility>

#include "TFile.h"
#include "TH2D.h"
#include "TMath.h"
#include "TString.h"

using namespace std;

vector<vector<double>> JetVetoMap(4); 

vector<vector<double>> getVetoMap() {
  return JetVetoMap;
}

void jetVetoMap(TString filename="hotjets-UL16.root", TString map_name2 = "h2hot_ul16_plus_hbm2_hbp12_qie11" ) {
  TDirectory *curdir = gDirectory;

  TFile *f0 = new TFile(filename,"READ");
  assert(f0 && !f0->IsZombie());
  curdir->cd();

  TH2D *h2 = (TH2D*)f0->Get(map_name2);
  assert(h2);
//  TH2D *h6 = (TH2D*)f0->Get(map_name6);
//  assert(h6);

  for (int i = 1; i != h2->GetNbinsX()+1; ++i) {
    for (int j = 1; j !=  h2->GetNbinsY()+1; ++j) {
      if (h2->GetBinContent(i,j) >0){
        JetVetoMap[0].push_back(h2->GetXaxis()->GetBinLowEdge(i)); 
        JetVetoMap[1].push_back(h2->GetXaxis()->GetBinLowEdge(i) + h2->GetXaxis()->GetBinWidth(i));
        JetVetoMap[2].push_back(h2->GetYaxis()->GetBinLowEdge(j));
        JetVetoMap[3].push_back(h2->GetYaxis()->GetBinLowEdge(j) + h2->GetYaxis()->GetBinWidth(j));
      }
    }
  }
/*
  for (int i = 1; i != h6->GetNbinsX()+1; ++i) {
    for (int j = 1; j !=  h6->GetNbinsY()+1; ++j) {
      if (h6->GetBinContent(i,j) >0){
        JetVetoMap[0].push_back(h6->GetXaxis()->GetBinLowEdge(i)); 
        JetVetoMap[1].push_back(h6->GetXaxis()->GetBinLowEdge(i) + h6->GetXaxis()->GetBinWidth(i));
        JetVetoMap[2].push_back(h6->GetYaxis()->GetBinLowEdge(j));
        JetVetoMap[3].push_back(h6->GetYaxis()->GetBinLowEdge(j) + h6->GetYaxis()->GetBinWidth(j));
      }
    }
  }
*/

}

#endif //__jetVetoMap_C__
