#!/bin/bash

tfg/libsvm-3.21/svm-train -c $2 -t $1 -d 2 tfg/inputs/featuresCoded/fcTrain$3.txt tfg/outputs/models/train$3-$1-$2.model
tfg/libsvm-3.21/svm-predict tfg/inputs/featuresCoded/fcDev$3.txt tfg/outputs/models/train$3-$1-$2.model tfg/outputs/predictions/$3-$1-$2_predict.txt > tfg/outputs/accuracy/$3-$1-$2_acc.txt

cut -d' ' -f1 tfg/inputs/featuresCoded/fcDev5.txt | paste - tfg/outputs/predictions/$3-$1-$2_predict.txt > tfg/outputs/comparativas/c_$3-$1-$2.txt

python3.2 tfg/bin/getResults.py c_$3-$1-$2 > tfg/outputs/results/r_$3-$1-$2.txt