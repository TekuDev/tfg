#!/bin/bash
featTypes=(ALL OL OS)
perNones=(5 3 2 1)
for i in ${featTypes[@]};
do
	for j in ${perNones[@]};
	do
		#test:
		./featuresGenerator featuresTest${i}${j} dicctionaryTest${i}${j} ${i} < ../inputs/test.txt
		python3 script.py 10 featuresTest${i}${j} dicctionaryTest${i}${j} diccOfClassesTest featuresCodedTest${i}${j} ${j}

		#train:
		./featuresGenerator featuresX${i}${j} dicctionaryX${i}${j} ${i} < ../inputs/input.txt
		python3 script.py 10 featuresX${i}${j} dicctionaryX${i}${j} diccOfClassesTest featuresCodedX${i}${j} ${j}
		../libsvm-3.21/svm-train ../inputs/featuresCoded/featuresCodedX${i}${j}.txt ../outputs/models/X${i}${j}.model
		../libsvm-3.21/svm-predict ../inputs/featuresCoded/featuresCodedTest${i}${j}.txt ../outputs/models/X${i}${j}.model ../outputs/predictions/X${i}${j}_predict.txt > ../outputs/accuracy/X${i}${j}_out.txt
	done
done