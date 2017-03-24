import string
import sys

num = sys.argv[1]


allFeats = open("tfg/inputs/featuresCoded/fcTrain"+num+"-N.txt",'r')
train1 = open("tfg/inputs/featuresCoded/fcTrain"+num+".txt",'w')


line = allFeats.readline()
j = 0
#Create train1 file with all positives and 200K negatives examples.
while(line != ''):
	if line[0] == '0' and j < 200000:
		train1.write(line)
		j += 1
	elif line[0] != '0':
		train1.write(line)
	line = allFeats.readline()
allFeats.close()
train1.close()