#!/bin/sh

html_folder=$1
cat $html_folder"/index.html"| head -n 25 >> tmp.txt; cat ~/artdaq/iceberg/analysis_scripts/fft_order.txt >> tmp.txt; cat index.html| tail -n 21 >> tmp.txt
cp tmp.txt $html_folder"/index.html"
rm -r tmp.txt
#mv tmp.txt $html_folder"/index.html"
