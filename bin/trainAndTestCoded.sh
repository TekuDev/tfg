#!/bin/bash

#train:
./pruebaWithDiccs featuresTrain dicctionary diccOfClasses ALL < ../inputs/train.txt
python3 script.py 0 featuresTrain dicctionaryTrain diccOfClasses fcTrain 100

#test:
./featuresGenerator featuresTest dicctionary ALL < ../inputs/test.txt
python3 script.py 0 featuresTest dicctionary diccOfClasses fcTest 100