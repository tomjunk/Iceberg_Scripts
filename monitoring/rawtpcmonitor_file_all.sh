#!/bin/sh

#unpack and run the raw decoder and raw TPC monitor on one file.

source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

fname=`basename $1`

lar -c /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/idtmon2000.fcl -T ${fname}_monitor.root $1

root -b -l <<EOF
.x /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/makeplots.C("${fname}_monitor.root");
.q
EOF
rm ${fname}_monitor.root

shortername=`echo ${fname} | cut -b -15`

mv html /data1/monitoring/${shortername}
