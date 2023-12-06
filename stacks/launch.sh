#!/bin/bash

#$1 time for each test
#$2 us 

MAX_THREADS=$( cat /proc/cpuinfo | grep processor | wc -l)     #maximum number of threads, which is typically equal to the number of (logical) cores.
QUARTER=$(($MAX_THREADS/4))
thl="1 $((QUARTER/2)) $QUARTER $((QUARTER*2)) $((QUARTER*3)) $MAX_THREADS $(($MAX_THREADS * 2))"    #thread counts to be used for scalability charts
#thl="$((QUARTER*2)) $((QUARTER*3)) $MAX_THREADS $(($MAX_THREADS * 2))"    #thread counts to be used for scalability charts

sl="mutex spinl treib tr_bo elimi"

echo -ne  "TH ideal"
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
if [ $2 -ne "0" ]; then
res=$(python3 -c "print('{:.2f}'.format($i*0.02*50/$2))")
else
res=$(python3 -c "print('{:.2f}'.format($i*18.00))")
fi
echo -ne "$res"
for s in $sl; do
  ./${s}_stack $i 100 $1 $2
done
echo ""
done
