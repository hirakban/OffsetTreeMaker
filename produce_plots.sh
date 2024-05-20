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

  data="/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_Data_Run2023D-PromptReco_R4.root"
  mc="/uscms/home/hirakban/nobackup/l10ffset/Run3_l1offset/CMSSW_13_0_13/src/test/OffsetTreeMaker/histomaker_outputs/Offset_MC_Run3Summer23BPix_Offset_Data_Run2023D_v2_R4.root"
  outname="Run3-${run_era}_DataMC_R4_"

  n1=18
  n2=56
  topX=58
  topY=58

  var="nPU"  #"nPU"
  ratio="true"
#  label="Run2018${run_era}_${luminosity}fb^{-1} (13.6 TeV)"
  label="${run_era} (13.6 TeV)"

  cmds=( "root -l -b -q 'plot_histos.c (\"$mc\", \"$data\", \"$outname\", \"$label\")'"
         "root -l -b -q 'plot_profile.c (\"$mc\", \"$data\", \"$outname\", $n1, $n2, $topX, $topY, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "all", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "ne",  $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "hfe", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "nh",  $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "hfh", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "chu", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "chm", $ratio, \"$label\")'"
         "root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$outname\", \"$var\", "untrk", $ratio, \"$label\")'"
       )

  for cmd in "${cmds[@]}"
  do
    eval $cmd
  done
