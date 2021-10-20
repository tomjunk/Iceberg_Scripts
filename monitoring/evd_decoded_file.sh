#!/bin/sh

source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

#lar -c evd_iceberg_data.fcl $1
lar -c /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/evd2000.fcl $1
