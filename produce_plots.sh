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
  mc="Offset_MC_UL2016_${run_era}_total_R4.root"
  data="Offset_Data_UL2016_${run_era}_total_R4.root"
  outname="UL16${run_era}_DataMC_R4_"

  n1=10
  n2=42
  topX=44
  topY=44

  var="nPU"
  ratio="true"
  label="Run2016${run_era}-${luminosity}fb^{-1}(13 TeV)"

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
