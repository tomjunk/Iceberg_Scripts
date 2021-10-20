#!/bin/sh

#unpack and run the raw decoder and raw TPC monitor on one file.

#source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

fpath=`echo $1 | sed -e 's/enstore://g'`
fname=`basename $fpath`

lar -c /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/idtmon2000.fcl -T ${fname}_monitor.root $fpath

root -b -l <<EOF
.x /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/makeplots.C("${fname}_monitor.root");
.q
EOF
rm ${fname}_monitor.root

shortername=`echo ${fname} | cut -b -15`

mv html /data1/monitoring/all_events_${shortername}
