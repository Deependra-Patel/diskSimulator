#!/usr/bin/python3

from random import random

file = open('input.txt', 'w')

ilimit = 1000
jlimit = 11

for i in range (0,ilimit):
	x = random()
	x = int(x*ilimit)
	y = random()
	y = int(y*jlimit)
	z = random()
	z = int(z*2)
	file.write(str(x) + "\t" + str(y) + "\t" + str(z) + "\n")
	
