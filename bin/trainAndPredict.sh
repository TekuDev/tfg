#!/bin/bash
cortes=(10 100 1000 10000)
kernels=(0 1)
Cs=(0.01 0.1 1 10 100)
for i in ${kernels[@]};
do
	for j in ${Cs[@]};
	do
		for t in ${cortes[@]};
		do
			../libsvm-3.21/svm-train -c ${j} -t ${i} ../inputs/featuresCoded/fcTrain${t}-50.txt ../outputs/models/train${t}-50-${i}-${j}.model
			../libsvm-3.21/svm-predict ../inputs/featuresCoded/fcDevelopment${t}.txt ../outputs/models/train${t}-50-${i}-${j}.model ../outputs/predictions/${t}-50-${i}-${j}_predict.txt > ../outputs/accuracy/${t}-50-${i}-${j}_acc.txt
		done
	done
done