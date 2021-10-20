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
shapetime=`grep "peak_time:" $CFNAME | head -1 | awk '{print $2}'`
accouple=`grep "ac_couple:" $CFNAME | head -1 | awk '{print $2}'`
baselineHigh=`grep "baseline:" $CFNAME | head -1 | awk '{print $2}'`
leakHigh=`grep "leak:" $CFNAME | head -1 | awk '{print $2}'`
leak10x=`grep "leak_10x:" $CFNAME | head -1 | awk '{print $2}'`
pulsemode=`grep "enable_pulser:" $CFNAME | head -1 | awk '{print $2}'`
pulsedac=`grep "pulse_dac:" $CFNAME | head -1 | awk '{print $2}'`
configname=`grep "Config name:" $CFNAME | head -1 | awk '{print $3}'`
febuffer=`grep "buffer:" $CFNAME | head -1 | awk '{print $2}'`

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

find /data1/dropbox/ -newer /data1/dropbox/iceberg_r008173_sr01_20210112T015442_1_dl1.root | grep iceberg | grep root | grep -v decode | grep -v json | grep -v anal | while read line
do
#  echo Extracting spreadsheet info for $line
  spreadsheet_single_file $line 
done
