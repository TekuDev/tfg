#!/bin/bash

#train:
#./featuresGenerator featuresTrainALL ALL dicctionary diccOfClasses  < ../inputs/train.txt
python3 script.py 10 featuresTrainALL dicctionary diccOfClasses fcTrain10
python3 script.py 100 featuresTrainALL dicctionary diccOfClasses fcTrain100
python3 script.py 1000 featuresTrainALL dicctionary diccOfClasses fcTrain1000
python3 script.py 10000 featuresTrainALL dicctionary diccOfClasses fcTrain10000

#test:
#./featuresGenerator featuresTest ALL default < ../inputs/test.txt
#python3 script.py 0 featuresTest dicctionary diccOfClasses fcTest0