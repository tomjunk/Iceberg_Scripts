#!/bin/bash

cd /home/nfs/dunecet/artdaq/iceberg/metadatamaker
source offlsetup.sh
cd ./runconfigspreadsheet
runfrom=$1
runfrom=$(($runfrom-1))
runlast=0
for i in `ls /data1/dropbox_staging/iceberg_r000[6-9]*dl1.root`; do 
  run=`echo $(basename $i) | sed -e 's/iceberg.*r000\([0-9]*\)_.*dl1.root/\1/g'`
  if [ $run -gt $runfrom ]; then
    echo $i >> file_list.txt
	runlast=$run
  fi
done
runfrom=$(($runfrom+1))
./spreadsheetdump.sh "$(cat file_list.txt)" | tee "spreadsheet_log_"$runfrom"-"$runlast".csv"
echo "Saved logs from run : "$runfrom" to "`pwd`"/spreadsheet_log_"$runfrom"-"$runlast".csv" 

rm file_list.txt
cd ~/artdaq/iceberg/analysis_scripts
