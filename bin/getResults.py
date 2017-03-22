def printTable(table):
	print("      0\t\t|\t1\t|\t2\t|\t3\t|\t4\t|\t5\t|\t")
	print("-------------------------------------------------------------------------------------------------")
	i = 0
	for v in table:
		row = str(i)+" |  "
		for val in v:
			row = row + str(val) + "\t|\t"
		print(row)
		i+=1
	print("-------------------------------------------------------------------------------------------------")

def sumRow(row, table):
	suma = 0
	for i in range(0,6):
		suma += table[row][i]
	return suma

def sumColum(col, table):
	suma = 0
	for i in range(0,6):
		suma += table[i][col]
	return suma

def addTotals(table):
	for i in range(0,6):
		table[i][6] = sumRow(i,table)
		table[6][i] = sumColum(i,table)

def sumDiagonal(table):
	suma = 0
	for i in range(1,6):
		suma += table[i][i]
	return suma

def sumRows(table):
	suma = 0
	for i in range(1,6):
		for j in range(0,6):
			suma += table[i][j]
	return suma

def sumColums(table):
	suma = 0
	for i in range(0,6):
		for j in range(1,6):
			suma += table[i][j]
	return suma


files = ("1000-100k-0-0.01", "1000-100k-0-0.1", "1000-150k-0-0.01"
	, "100-100k-0-0.1", "100-100k-1-10", "100-150k-0-0.01"
	, "10-100k-0-0.01", "10-100k-0-0.1", "10-100k-1-0.01"
	, "10-100k-1-0.1", "10-100k-1-100", "10-100k-1-10"
	, "10-100k-1-1", "10-150k-0-0.01", "10-150k-1-100", "10-150k-1-10")


for f in files:
	print(f+"\n")
	file = open("../outputs/comparativa-"+f+".txt",'r')
	table = [[0 for x in range(7)] for y in range(7)]

	line = file.readline()
	while(line != ""):
		esp = line.split('\t')[0]
		pred = line.split('\t')[1]
		#print(esp+" "+pred)
		table[int(pred)][int(esp)] += 1
		line = file.readline()
	file.close()
	addTotals(table)
	printTable(table)

	#print(sumColum(2,table))
	#print(sumRow(5,table))

	for i in range(0,6):
		try:
			print("Class: "+str(i))
			print("")
			p = table[i][i]/table[i][6]*100
			print("Precision = "+str(p)+" ("+str(table[i][i])+"/"+str(table[i][6])+")")
			r = table[i][i]/table[6][i]*100
			print("Recall = "+str(r)+" ("+str(table[i][i])+"/"+str(table[6][i])+")")
			f1 = (2*p*r)/(p+r)
			print("F1 = "+str(f1)+" ("+str(2*p*r)+"/"+str(p+r)+")")
		except Exception as e:
			print("Esta clase nunca se ha predecido")
		
		print("---------------------")
	print("Global estadistics:")
	pg = sumDiagonal(table)/sumRows(table)*100
	print("Precision = "+str(pg)+" ("+str(sumDiagonal(table))+"/"+str(sumRows(table))+")")
	rg = sumDiagonal(table)/sumColums(table)*100
	print("Recall = "+str(rg)+" ("+str(sumDiagonal(table))+"/"+str(sumColums(table))+")")
	f1g = (2*pg*rg)/(pg+rg)
	print("F1 = "+str(f1g)+" ("+str(2*pg*rg)+"/"+str(pg+rg)+")")

	print("---------------------")
	print("resum "+f+" "+str(pg)+" "+str(rg)+" "+str(f1g))
	print("---------------------")
