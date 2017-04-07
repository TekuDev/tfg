import string
import sys
import random

readFile = sys.argv[1]
writeFile = sys.argv[2]
percent = int(sys.argv[3])


allFeats = open("tfg/inputs/featuresCoded/"+readFile+".txt",'r')
train1 = open("tfg/inputs/featuresCoded/"+writeFile+".txt",'w')


line = allFeats.readline()
#Create train1 file with all positives and 200K negatives examples.
while(line != ''):
	if line[0] == '0' and random.randint(1,100) < percent:
		train1.write(line)
	elif line[0] != '0':
		train1.write(line)
	line = allFeats.readline()
allFeats.close()
train1.close()