#!/bin/bash

#train:
./pruebaWC featuresTrain dicctionaryTrain diccOfClasses ALL < ../inputs/train.txt
python3 script.py 0 featuresTrain dicctionaryTrain diccOfClasses fcTrain 100

#test:
./featuresGenerator featuresTest dicctionaryTest ALL < ../inputs/test.txt
python3 script.py 0 featuresTest dicctionaryTest diccOfClasses fcTest 100