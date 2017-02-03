import string

files = open("../inputs/featuresCoded/fcTrain10000.txt",'r')
files2 = open("../inputs/featuresCoded/fcDevelopment10000.txt",'w')
files3 = open("../inputs/featuresCoded/fcParaOptimizar10000.txt",'w')

file = files.readline()
i = 0
while(i < 2000000 and file != ''):
	files2.write(file)
	i += 1
	file = files.readline()

while(file != ''):
	files3.write(file)
	file = files.readline()

files.close()
files2.close()
files3.close()