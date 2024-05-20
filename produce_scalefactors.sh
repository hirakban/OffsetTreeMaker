#!/bin/bash

# Garvita Agarwal 06/19/2020 
# execute as ./produce_scaalefactors.sh <Run> <Luminosity>
# can at a later time, aff l1fastjet_adapted2020.c to this as well


  args=("$@")
  if [ $# -lt 2 ] ; then
    echo "Please provide 'Run Era' and 'Recorded Luminosity': "
  fi

  if [ $# -eq 2 ] ; then

    run_era=${args[0]}
    luminosity=${args[1]}

  fi

  data="/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2022CD-ReReco_R4.root"
  mc="/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_MC_Run3Summer22_Offset_Data_Run2022CD_R4.root"
  outname="Run3-${run_era}_DataMC_R4_"

#  outname="Run3-${run_era}_DatatoData_NHdepth4_R4_"

  R=0.4
  n1=18
  n2=52
  topX=62
  topY=62

  var="nPU"
  ratio="true"
#  label="Run2018${run_era}-${luminosity}fb^{-1}(13 TeV)"
  label="Run3-${run_era} (13.6 TeV)"
#  label="${run_era}"
  subplot=""     # "pull" or "DataOverMC"
  

#1) All
#2) Assoc. Charged Hadron
#3) Lepton
#4) Photon
#5) Neutral Hadron
#6) HF Tower Hadron
#7) HF Tower EM Particle
#8) Charged Hadron Subtraction
#9) Median Charged Hadron Subtraction
#10) Median All

  cmds=( #"root -l -b -q 'offsetpT.c (\"$mc\", \"$data\", \"$outname\", \"$subplot\", $R, 8, $n1, $n2, $topX, $topY)'"
         #"root -l -b -q 'offsetpT.c (\"$mc\", \"$data\", \"$outname\", $R, 9, $n1, $n2, $topX, $topY)'"
         "root -l -b -q 'scalefactor.c (\"$mc\", \"$data\", \"$outname\", $R, 8, $n1, $n2)'"
         #"root -l -b -q 'scalefactor.c (\"$mc\", \"$data\", \"$outname\", $R, 9, $n1, $n2)'"
         #"root -l -b -q 'offsetpT.c (\"$mc\", \"$data\", \"$outname\", $R, 1, $n1, $n2, $topX, $topY, \"$label\")'"
       )

  for cmd in "${cmds[@]}"
  do
    eval $cmd
  done
