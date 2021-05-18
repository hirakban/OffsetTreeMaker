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
  mc="Offset_MC_UL2016_${run_era}_total_R4.root"
  data="Offset_Data_UL2016_${run_era}_total_R4.root"
  outname="UL16_Run${run_era}_DataMC_R4_"

  R=0.4
  n1=10
  n2=60
  topX=64
  topY=20

  var="nPU"
  ratio="true"
  label="Run2016${run_era}MC (13 TeV)"
#  label="Run2016${run_era}-${luminosity}fb^{-1}(13 TeV)"
#  pull_subplot="false"
#  datamc_subplot="false"


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

  cmds=( "root -l -b -q 'offsetpT.c (\"$mc\", \"$data\", \"$outname\", \"\", \"\", $R, 8, $n1, $n2, $topX, $topY)'"
         #"root -l -b -q 'offsetpT.c (\"$mc\", \"$data\", \"$outname\", $R, 9, $n1, $n2, $topX, $topY)'"
         "root -l -b -q 'scalefactor.c (\"$mc\", \"$data\", \"$outname\", $R, 8, $n1, $n2)'"
         #"root -l -b -q 'scalefactor.c (\"$mc\", \"$data\", \"$outname\", $R, 9, $n1, $n2)'"
         #"root -l -b -q 'offsetpT.c (\"$mc\", \"$data\", \"$outname\", $R, 1, $n1, $n2, $topX, $topY, \"$label\")'"
       )

  for cmd in "${cmds[@]}"
  do
    eval $cmd
  done
