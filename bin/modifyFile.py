import string
import sys
import random

readFile = sys.argv[1]
writeFile = sys.argv[2]
ftype = int(sys.argv[3])


old = open(readFile,'r')
new = open(writeFile,'w')

if ftype == 0:
	line = old.readline()
	while(line != ''):
		if line[0] == '0':
			new.write(line)
		elif line[0] != '0':
			new.write('1 ')
			new.write(line[1:])
		line = old.readline()
elif ftype == 1:
	line = old.readline()
	while(line != ''):
		if line[0] != '0':
			new.write(line)
		line = old.readline()
else:
	print("Debe introducir dos paths y la versión que desea ejecutar, 0-> transforma el data set en binario, 1-> solo deja los positivos")
old.close()
new.close()