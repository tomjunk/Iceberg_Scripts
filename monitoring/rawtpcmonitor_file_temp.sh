#!/bin/bash

#unpack and run the raw decoder and raw TPC monitor on one file.

fname=`basename $1`
shortername=`echo ${fname} | cut -b -15`
strreplace=`echo ${fname} | sed -e 's/\.root//g'`

if [ -d /data1/monitoring/${shortername} ]; then 
 echo "Already analyzed"
 #echo "Already analyzed, just need to copy index.html"
 #cat /data1/monitoring/iceberg_r000902/index.html | sed -e "s/iceberg_r000[0-9]*_.*dl1/$strreplace/g" > /data1/monitoring/${shortername}/index.html
else 

  source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

  lar -n 10 -c /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/idtmon2000.fcl -T ${fname}_monitor.root $1

  root -b -l <<EOF
.x /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/makeplots.C("${fname}_monitor.root");
.q
EOF
  rm ${fname}_monitor.root

  mv html /data1/monitoring/${shortername}
fi
