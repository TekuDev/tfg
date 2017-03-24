import sys
import string

def printDic(dic):
	for k,v in dic.items():
		print(k+":"+str(v))

#read the parameters
if len(sys.argv) != 6:
	print("Debes introducir la frequencia de corte (0 para coger todos los features)")
	print("Debes introducir el nombre de los ficheros de input [features], [dicctionary] , [diccOfClasses]")
	print("Debes introducir el nombre del fichero de output [featuresCoded]")
	sys.exit(0)

depth = int(sys.argv[1])
featuresName = sys.argv[2]
dicctionaryName = sys.argv[3]
diccOfClassesName = sys.argv[4]
featuresCodedName = sys.argv[5]

#open the files to work
dicctionary = open("tfg/inputs/dicctionaries/"+dicctionaryName+".txt",'r')
diccOfClasses = open("tfg/inputs/diccOfClasses/"+diccOfClassesName+".txt",'r')
features = open("tfg/features/"+featuresName+".txt",'r')
result = open("tfg/inputs/featuresCoded/"+featuresCodedName+".txt",'w')

#build the Dicctionary
dic = {}
line = diccOfClasses.readline()
while(line != ''):
	feat = line.split(' ')
	dic[feat[1].strip('\n')] = feat[0]
	line = diccOfClasses.readline()

#printDic(dic)

line = dicctionary.readline()
while(line != ''):
	feat = line.split(' ')
	freq = int(feat[2])

	if freq >= depth:
		dic[feat[1].strip('\n')] = feat[0]
	else:
		break
	line = dicctionary.readline()

print("Dicctionary created")
#printDic(dic)

#rewrite the features in result file
nNones = 0
line = features.readline()
while(line != ''):
	feats = line.split(' ')
	clas = feats[0]
	if not clas in dic:
		clas = "NONE"
	
	#write the class
	result.write(dic[clas]+' ')
	feats.pop(0)
	codes = []
	for f in feats:
		tag = f.split('=')
		#if exist this feature in the dictionary write it.
		if f in dic:
			codes.append((int(dic[f]),"1"))
		elif tag[0] in dic:
			codes.append((int(dic[tag[0]]),tag[1]));
	codes.sort()
	for c in codes:
		#write the feature
		result.write(str(c[0])+":"+c[1]+' ')
	result.write('\n')
	
	line = features.readline()

print("Features coded")

dicctionary.close()
features.close()
result.close()
diccOfClasses.close()