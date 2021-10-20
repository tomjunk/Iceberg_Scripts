#!/bin/bash

root -l -b <<EOF | grep "Channel fit" | sed -e "s/Channel fit: //" > $1_chans.txt
.x chanpulsemeans.C("$1");
EOF
