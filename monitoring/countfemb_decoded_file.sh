#!/bin/sh

source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

root -l -q -b -e ".x /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/fembcount.C(\"$1\");"
