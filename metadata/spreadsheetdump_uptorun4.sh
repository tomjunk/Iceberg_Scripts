#!/bin/bash
#
#setup artDAQ
#
source /home/nfs/dunecet/artdaq/iceberg/analysis_scripts/offlsetup.sh

# dumps values for upload to a spreadsheet

function spreadsheet_single_file {

CFNAME=`mktemp`
config_dumper -P $1 | sed -e "s/\\\n/\n/g" > ${CFNAME}

TMPMD=`mktemp`
sam_metadata_dumper $1 > ${TMPMD}

CMPFILE=`mktemp`
grep Component $CFNAME | awk '{print $3}' > $CMPFILE

gain=`grep "gain:" $CFNAME | head -1 | awk '{print $2}'`
shapetime=`grep "shape:" $CFNAME | head -1 | awk '{print $2}'`
accouple=`grep "acCouple:" $CFNAME | head -1 | awk '{print $2}'`
baselineHigh=`grep "baselineHigh:" $CFNAME | head -1 | awk '{print $2}'`
leakHigh=`grep "leakHigh:" $CFNAME | head -1 | awk '{print $2}'`
leak10x=`grep "leak10X:" $CFNAME | head -1 | awk '{print $2}'`
pulsemode=`grep "pls_mode:" $CFNAME | head -1 | awk '{print $2}'`
pulsedac=`grep "pls_dac_val:" $CFNAME | head -1 | awk '{print $2}'`
configname=`grep "Config name:" $CFNAME | head -1 | awk '{print $3}'`
febuffer=`grep "buffer:" $CFNAME | grep bypass | head -1 | awk '{print $2}'`

runno=`grep last_event $TMPMD | head -1 | awk '{print $3}' | sed -e "s/,//g"`
firstevent=`grep first_event $TMPMD | head -1 | awk '{print $5}'`
lastevent=`grep last_event $TMPMD | head -1 | awk '{print $5}'`
eventcountline=`grep event_count $TMPMD | head -1 | sed -e "s/,//g" | awk '{print $2}'`
starttimeline=`grep start_time $TMPMD | head -1 | sed -e "s/,//g" | awk '{print $2}' | sed -e "s/\"//g"`
endtimeline=`grep end_time $TMPMD | head -1 | sed -e "s/,//g" | awk '{print $2}' | sed -e "s/\"//g"`

echo ${configname}, ${runno}, ${starttimeline}, ${endtimeline}, ${pulsemode}, ${pulsedac}, ${gain}, ${shapetime}, ${accouple}, ${baselineHigh}, ${leakHigh}, ${leak10x}, ${febuffer}, ${eventcountline}, `basename $1`

rm ${CFNAME}
rm ${TMPMD}
rm ${CMPFILE}

}

find -newer /data1/dropbox/iceberg_r007036_sr01_20200813T155413_1_dl1.root | grep iceberg | grep root | grep -v decode | grep -v json | grep -v anal | while read line
do
#  echo Extracting spreadsheet info for $line
  spreadsheet_single_file $line 
done
