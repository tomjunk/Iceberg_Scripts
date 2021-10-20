#!/bin/sh

source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

export TMPDIR=/data1/analysis
outfile=`mktemp --suffix=.root`
# we just want the name, not the file
touch ${outfile}
rm ${outfile}

lar -c RunIcebergRawDecoder14.fcl -T /dev/null $1 -o ${outfile}

#lar -c evd_iceberg_data.fcl ${outfile}
lar -c /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/evd2000.fcl ${outfile}

rm ${outfile}


