#!/bin/python3
import os
import sys

block="_"*16
last_block="_"*15
space="\t\t"
last_space="\t\t"
middle=last_block+"|"

uno="_"*5+"1"+"_"*5
due="_"*5+"2"+"_"*5
tre="_"*5+"5"+"_"*5
qua="_"*5+"10"+"_"*4
cin="_"*5+"20"+"_"*4
sei="_"*5+"30"+"_"*4
set="_"*5+"40"+"_"*4
ott="_"*5+"80"+"_"*4


pes=" "*2+"PESSIMISTIC"+" "*2

print(" "*15+"_"*11*8+"_______")
print(" "*14+"|"+"_"*11*3+"________#THREADS___________"+"_"*11*3+"__|")
print(" "+"_"*13+"|"+uno+"|"+due+"|"+tre+"|"+qua+"|"+cin+"|"+sei+"|"+set+"|"+ott+"|")
#print "|"+space*1+"|"+space*4+last_space+"|"
#print "|"+space*1+"|"+space*4+last_space+"|"
#print "|"+space*1+"|"+space*4+last_space+"|"
#print "|"+last_block*1+"|"+block*4+last_block+"|"

US=int(sys.argv[1])
progs=['ideal', 'pessimistic', 'chained', 'optimistic', 'lockfree']
for p in progs:
  os.system(f'./{p} {US}')
