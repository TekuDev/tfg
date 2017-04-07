#!/bin/bash
cortes=(5 10 50)
kernels=(0 1)
Cs=(0.01 0.1 1 10 100)
nNones=(150)
for i in ${kernels[@]};
do
	for j in ${Cs[@]};
	do
		for t in ${cortes[@]};
		do
			for h in ${nNones[@]};
			do
				qsub -q medium tfg/bin/sendTest.sh ${i} ${j} ${t}-${h}
			done
		done
	done
done