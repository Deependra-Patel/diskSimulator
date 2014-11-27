#!/usr/bin/python3

from random import random

file = open('input.txt', 'w')

tests = 1000 #no. of test cases
D = 5 #D+1 total disks

for i in range (0,tests):
	x = random()
	x = int(x*tests)
	y = random()
	y = int(y*D)
	z = random()
	z = int(z*2)
	file.write(str(x) + "\t" + str(y) + "\t" + str(z) + "\n")
	
