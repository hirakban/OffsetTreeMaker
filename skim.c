//Chad Harrington 3/23/2015
//Skim a TTree

#include "TFile.h"
#include "TTree.h"
using namespace std;

void skim(){
  
  int numSkip = 1003;

  TFile* file = TFile::Open("/eos/uscms/store/user/hbandyop/l1offsetUL16_result/MC_Summer19UL2016/Offset_MC_UL2016_total.root");

  TTree* tree = (TTree*) file->Get("T");
  ULong64_t nEntries = tree->GetEntries();

//  ULong64_t event;
//  tree->SetBranchAddress("event", &event);
  Int_t mua;
  tree->SetBranchAddress("mua", &mua);
  tree->SetBranchStatus("*",1);

  TFile *newfile = new TFile("/eos/uscms/store/user/hbandyop/l1offsetUL16_result/MC_Summer19UL2016/Offset_MC_UL2016_postVFP_reduced.root","recreate");
  TTree *newtree = tree->CloneTree(0);

  for (ULong64_t n=0; n<nEntries; n++) {
    tree->GetEntry(n);

//    if (mua % numSkip == 0) newtree->Fill();
  }

  newtree->Print();
  newfile->Write();
  delete file;
  delete newfile;
}
