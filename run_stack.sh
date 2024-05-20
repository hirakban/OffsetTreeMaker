#!/bin/bash

# execute as ./run_stack.sh <Run Era> <Rec. Luminosity>

  args=("$@")
  if [ $# -lt 2 ] ; then
    echo "Please provide run era (BCD,EF,FG,H) and Recorded Luminosity"
  fi

  if [ $# -eq 2 ] ; then

    run_era=${args[0]}
    luminosity=${args[1]}

  fi
  mc="histomaker_outputs/Offset_Data_Run2022${run_era}-27Jun2023-v1_R4.root"
  data="histomaker_outputs/Offset_Data_Run2023${run_era}-PromptReco_R4.root"

  var="nPU"
  ratio="true"
  doEnergy="false"
  label="Run 2018${run_era} - ${luminosity} fb^{-1} (13 TeV)"

  cmds=("root -l -b -q 'offsetpT_stack_depth.c(\"$mc\", \"$data\", \"$var\", "all_depths", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack_depth.c (\"$mc\", \"$data\", \"$var\", "depth1", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "depth2", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "depth3", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "depth4", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "depth5", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "depth6", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "depth7", $ratio, $doEnergy, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "all", $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "ne",  $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "hfe", $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "nh",  $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "hfh", $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "chu", $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "chm", $ratio, \"$label\")'"
         #"root -l -b -q 'offsetpT_stack.c (\"$mc\", \"$data\", \"$var\", "untrk", $ratio, \"$label\")'"
       )

  for cmd in "${cmds[@]}"
  do
    eval $cmd
  done
