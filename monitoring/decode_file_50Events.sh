#!/bin/sh

# runs the iceberg raw decoder on one file

source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

lar -c RunIcebergRawDecoder14.fcl -n 50 -T /dev/null $1
