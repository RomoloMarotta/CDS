#!/bin/python3
import os

block="_"*16
last_block="_"*15
space="\t\t"
last_space="\t\t"
middle=last_block+"|"

uno="_"*5+"1"+"_"*5
due="_"*5+"2"+"_"*5
tre="_"*5+"4"+"_"*5
qua="_"*5+"8"+"_"*5
cin="_"*5+"16"+"_"*4
sei="_"*5+"32"+"_"*4


pes=" "*2+"PESSIMISTIC"+" "*2

print(" "*15+"_"*11*6+"_____")
print(" "*14+"|"+"_"*11*2+"________#THREADS___________"+"_"*11*2+"|")
print(" "+"_"*13+"|"+uno+"|"+due+"|"+tre+"|"+qua+"|"+cin+"|"+sei+"|")
#print "|"+space*1+"|"+space*4+last_space+"|"
#print "|"+space*1+"|"+space*4+last_space+"|"
#print "|"+space*1+"|"+space*4+last_space+"|"
#print "|"+last_block*1+"|"+block*4+last_block+"|"

os.system('./pessimistic')
os.system('./chained')
os.system('./optimistic')
os.system('./lockfree')
