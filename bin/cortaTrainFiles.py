import string
import sys

if len(sys.argv) != 3:
	print("Arguments: [Type] [Value]. Type={Percent,Distance} Value={Integer}")
	sys.exit(0)

typeNones = sys.argv[1]
valueNones = int(sys.argv[2])

if typeNones != "Percent" and typeNones != "Distance":
	print("Type has to be 'Percent' or 'Distance'")
	sys.exit(0)

files = open("../inputs/featuresCoded/fcParaOptimizar.txt",'r')
files2 = open("../inputs/featuresCoded/fcTrainGroupType-"+typeNones+"-"+str(valueNones)+".txt",'w')

line = files.readline()
nNones = 0
while(line != ''):
	if line.split(' ')[1].split(':')[0] == "10":
		if(line[0] == '8'):
			dist = int(line.split(' ')[1].split(':')[1])
			
			if typeNones == "Distance" and dist <= valueNones:
				files2.write(line)
			elif typeNones == "Percent" and nNones >= (100-valueNones) and nNones <= 100:
				files2.write(line)
				nNones += 1
			else:
				nNones += 1
		else:
			files2.write(line)

		if nNones >= 100:
			nNones = 0
		
	line = files.readline()

files.close()
files2.close()