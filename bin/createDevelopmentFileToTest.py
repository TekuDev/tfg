import string
import sys
import random


train = open("tfg/inputs/featuresCoded/fcTrain5-t0-150-2.0.txt",'r')
train2 = open("tfg/inputs/featuresCoded/fcTrain5-t0-150-3.0.txt",'w')
dev1 = open("tfg/inputs/featuresCoded/fcDevTest1.txt",'w')
dev2 = open("tfg/inputs/featuresCoded/fcDevTest2.txt",'w')


line = train.readline()
i = 0
while(line != ''):
	if i < 3000:
		dev1.write(line)
	else:
		train2.write(line)
	i += 1
	line = train.readline()
train.close()
train2.close()

train2 = open("tfg/inputs/featuresCoded/fcTrain5-t0-150-3.0.txt",'r')
train = open("tfg/inputs/featuresCoded/fcTrain5-t0-150-2.0.txt",'w')
i = 0
line = train2.readline()
while(line != ''):
	if i > (238933-3000):
		dev2.write(line)
	else:
		train.write(line)
	i += 1
	line = train2.readline()

train.close()
dev1.close()
dev2.close()
train2.close()