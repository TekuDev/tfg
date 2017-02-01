#!/bin/bash

#train:
./featuresGenerator featuresTrain ALL dicctionary diccOfClasses  < ../inputs/train.txt
python3 script.py 0 featuresTrain dicctionary diccOfClasses fcTrain0

#test:
./featuresGenerator featuresTest ALL default < ../inputs/test.txt
python3 script.py 0 featuresTest dicctionary diccOfClasses fcTest0