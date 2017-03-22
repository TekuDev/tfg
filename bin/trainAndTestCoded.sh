#!/bin/bash

#train:
#./featuresGenerator featuresTrainALL ALL dicctionary diccOfClasses  < ../inputs/train.txt
python3 script.py 5 featuresTrainALL dicctionary diccOfClasses fcTrain5-N
python3 script.py 10 featuresTrainALL dicctionary diccOfClasses fcTrain10-N
python3 script.py 50 featuresTrainALL dicctionary diccOfClasses fcTrain50-N
#python3 script.py 10000 featuresTrainALL dicctionary diccOfClasses fcTrain10000

#test:
#./featuresGenerator featuresTest ALL default < ../inputs/test.txt
python3 script.py 0 featuresTestALL dicctionary diccOfClasses fcTest

python3 script.py 5 featuresDevALL dicctionary diccOfClasses fcDev5-N
python3 script.py 10 featuresDevALL dicctionary diccOfClasses fcDev10-N
python3 script.py 50 featuresDevALL dicctionary diccOfClasses fcDev50-N