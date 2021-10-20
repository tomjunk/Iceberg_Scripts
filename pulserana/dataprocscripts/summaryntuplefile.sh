#!/bin/bash

root -l -b <<EOF
.x makesummaryntuple.C("$1");
EOF
