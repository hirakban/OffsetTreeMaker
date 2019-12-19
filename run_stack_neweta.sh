#!/bin/bash

# execute as ./run_stack.sh

  args=("$@")
  if [ $# -lt 2 ] ; then
    echo "Please provide run era (B,C,D,E,F) and Recorded Luminosity"
  fi

  if [ $# -eq 2 ] ; then

    run_era=${args[0]}
    luminosity=${args[1]}

  fi
  data="Offset_DataUL2017${run_era}_new_R4.root"

  var="nPU"
  ratio="true"
  label="Run 2017${run_era} - ${luminosity} fb^{-1} (13 TeV)"

  cmds=( #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "all", $ratio,"1" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "all", $ratio,"2" , \"$label\")'"
         "root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "all", $ratio,"3" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "all", $ratio,"4" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "all", $ratio,"5" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "ne", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "nh", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "hfh", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "hfe", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "chu", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "chm", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "lep", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "untrk", $ratio,"0" , \"$label\")'"
         #"root -l -b -q 'stack_neweta.c (\"$data\", \"$var\", "hf_dep", $ratio,"0" , \"$label\")'"
      )

  for cmd in "${cmds[@]}"
  do
    eval $cmd
  done
