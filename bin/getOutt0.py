import string
import sys
import random

readFile = sys.argv[1]
writeFile = sys.argv[2]


old = open(readFile,'r')
new = open(writeFile,'w')

line = old.readline()
while(line != ''):
	if line.find("w1t0") == -1:
		new.write(line)
	line = old.readline()

old.close()
new.close()
