#!/bin/sh
 
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_82_00d00 -q e26:prof
 
for filename in iceberg*decode.root; do
    bname=`basename ${filename}`
    ofile=${bname}_flattened.root
    root -b -l <<EOF
.x flatrawdigits.C("${filename}","${ofile}","tpcrawdecoder:daq")
.q
EOF
done
