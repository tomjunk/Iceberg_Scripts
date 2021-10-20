#!/bin/sh

root -l -b <<EOF | grep Chanfit | sed -e "s/Chanfit: //" > $1_fits.txt
.x allpulserfits.C+("$1")
EOF
