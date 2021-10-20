#!/bin/bash

root -l -b <<EOF
.x makentuple.C("$1");
EOF
