#!/bin/bash

for file in iceberg*decode.root; do

root -l -b <<EOF
.x makesummaryntuple.C("${file}_chans.txt");
EOF

done
