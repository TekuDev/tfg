import string

files = open("../inputs/featuresCoded/fcTrain.txt",'r')
files2 = open("../inputs/featuresCoded/fcDevelopment.txt",'w')
files3 = open("../inputs/featuresCoded/fcParaOptimizar.txt",'w')

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