#!/bin/bash

#train:
./featuresGenerator featuresTrain dicctionary diccOfClasses ALL < ../inputs/train.txt
python3 script.py 0 featuresTrain dicctionary diccOfClasses fcTrain 100

#test:
./featuresGenerator featuresTest default ALL < ../inputs/test.txt
python3 script.py 0 featuresTest dicctionary diccOfClasses fcTest 100