#!/bin/bash

#$1 time for each test

MAX_THREADS=$( cat /proc/cpuinfo | grep processor | wc -l)     #maximum number of threads, which is typically equal to the number of (logical) cores.
QUARTER=$(($MAX_THREADS/4))
thl="1 $((QUARTER/2)) $QUARTER $((QUARTER*2)) $((QUARTER*3)) $MAX_THREADS $(($MAX_THREADS * 2))"    #thread counts to be used for scalability charts

sl="mutex spinl treib"

echo -ne  "TH "
for s in $sl; do
    echo -ne  " $s"
done
echo ""

for i in $thl; do
  if [ $i -lt "10" ]; then
    echo -ne  "$i: "
  else
    echo -ne  "$i:"
  fi
for s in $sl; do
  ./${s}_stack $i 100 $1
done
echo ""
done
