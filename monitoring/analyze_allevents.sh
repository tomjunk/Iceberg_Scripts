#!/bin/bash

source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

runlow=$(($1 - 1))
runhigh=$(($2 + 1))

sam_cmd="samweb list-files 'run_type=iceberg and run_number > "$runlow" and run_number < "$runhigh"'"
for f in `eval $sam_cmd`; do 
  echo "Running file.. "$f
  ./rawtpcmonitor_file_allevents.sh `samweb locate-file $f`"/"$f
done
