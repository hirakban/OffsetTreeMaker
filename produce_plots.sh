#!/bin/bash

# execute as ./produce_plots.sh <Run> <Luminosity>

  args=("$@")
  if [ $# -lt 2 ] ; then
    echo "Please provide 'Run Era' and 'Recorded Luminosity': "
  fi

  if [ $# -eq 2 ] ; then

    run_era=${args[0]}
    luminosity=${args[1]}

  fi

  data="histomaker_outputs/new-MinbiasXS/Offset_Data_Run2024C-PromptReco-v1-puppi-Alexis-new-new-MinbiasXS_R4.root"
  mc="histomaker_outputs/new-MinbiasXS/Offset_MC_Run3Summer24DRPremix-puppi-Alexis_Offset_Data_Run2024C-PromptReco-v1-puppi-Alexis-new-new-MinbiasXS_R4.root"

#  mc="histomaker_outputs/Offset_Data_Run2024I-PromptReco-v1v2-withbpixveto-correctHLT_R4.root"

#  mc="/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_15_0_0/src/test/OffsetTreeMaker/histomaker_outputs/Offset_MC_Run3Summer24DRPremix-withbpixveto-jetveto2024BCDE_FG_FPix_Offset_Data_Run2024I-HcalPFCuts_HB_5x_R4.root"

#  mc="histomaker_outputs/Offset_MC_Run3Summer24DRPremix-withbpixveto-jetveto2024BCDE_FG_FPix_Offset_Data_Run2024I-PromptReco-v1v2-withbpixveto-correctHLT_R4.root"

#  mc="histomaker_outputs/Offset_MC_Run3Winter24Reco-withbpixveto_R4.root"

#  outname="Run3-${run_era}-Winter25_DataMC_R4_"
  outname="Run3-${run_era}-Summer24_DataMC_R4_"
#  outname="Run3-${run_era}_DatatoData_R4_"
#  outname="Run3-${run_era}_Winter25-vs-Summer24-MCtoMC_R4_"

  n1=32
  n2=62
  topX=64
  topY=74

  var="nPU"  #"nPU"
  ratio="true"
#  label="Run2017${run_era}_Summer20MC(13 TeV)"
#  label="Run2018${run_era}_${luminosity}fb^{-1} (13.6 TeV)"
  label="${run_era} (13.6 TeV)"
#  label="${run_era}"

  cmds=( "root -l -b -q 'plot_histos.c(\"$mc\", \"$data\", \"$outname\", \"$label\")'"
         "root -l -b -q 'plot_profile.c(\"$mc\", \"$data\", \"$outname\", $n1, $n2, $topX, $topY, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "all", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "ne",  $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "hfe", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "nh",  $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "hfh", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "chu", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "chm", $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c(\"$mc\", \"$data\", \"$outname\", \"$var\", "untrk", $ratio, \"$label\")'"

         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth1")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth2")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth3")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth4")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth5")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth6")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "depth7")'"
         #"root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$outname\", \"$var\", $ratio, \"$label\", "all_depths")'"
       )

  for cmd in "${cmds[@]}"
  do
    eval $cmd
  done
