#!/bin/bash
cortes=(10 100 1000 10000)
kernels=(l l2)
Cs=(0.01 0.1 1 10 100)
for i in ${kernels[@]};
do
	for j in ${Cs[@]};
	do
		for t in ${cortes[@]};
		do
			../libsvm-3.21/svm-train ../inputs/featuresCoded/fcTrain${t}-50.txt ../outputs/models/Train${t}-50.model
			../libsvm-3.21/svm-predict ../inputs/featuresCoded/featuresCodedTest${i}${j}.txt ../outputs/models/X${i}${j}.model ../outputs/predictions/X${i}${j}_predict.txt > ../outputs/accuracy/X${i}${j}_out.txt
		done
	done
done