#!/bin/bash

#train:
./featuresGenerator featuresTrain dicctionary diccOfClasses ALL < ../inputs/train.txt
python3 script.py 0 featuresTrain dicctionary diccOfClasses fcTrain

#test:
./featuresGenerator featuresTest default ALL < ../inputs/test.txt
python3 script.py 0 featuresTest dicctionary diccOfClasses fcTest