#!/bin/sh

make -j10 decode
make -j10 fittxt
make -j10 fitntuple
make -j10 chansumtxt
make -j10 chansumntuple
