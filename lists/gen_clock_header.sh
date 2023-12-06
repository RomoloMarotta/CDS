#!/bin/bash

gcc compute_clock.c -o compute_clock
#echo -ne "wainting compute clock..."
clocks=$(./compute_clock 5 | grep us | cut -d':' -f2)
#echo "DONE"

out="clock_constant.h"

echo "#ifndef __CLOCKS_PER_US_H__"     > $out
echo "#define __CLOCKS_PER_US_H__"    >> $out
echo ""                               >> $out
echo "#define CLOCKS_PER_US ((unsigned long long)$clocks)"  >> $out
echo "#endif"                         >> $out

echo "" >> $out
echo "#define CLOCK_READ() ({ \\"     >> $out
echo "                        unsigned int lo; \\"  >> $out
echo "                        unsigned int hi; \\"  >> $out
echo "                        __asm__ __volatile__ (\"rdtsc\" : \"=a\" (lo), \"=d\" (hi)); \\"  >> $out
echo "                        (unsigned long long)(((unsigned long long)hi) << 32 | lo); \\"  >> $out
echo "                        })"  >> $out

rm compute_clock
