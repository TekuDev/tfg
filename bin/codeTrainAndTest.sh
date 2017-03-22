#!/bin/bash

#train:
./tfg/bin/featuresGenerator featuresTrainALL ALL default  < tfg/inputs/train.txt
python3 tfg/bin/script.py 5 featuresTrainALL dicctionary diccOfClasses fcTrain5-N
python3 tfg/bin/script.py 10 featuresTrainALL dicctionary diccOfClasses fcTrain10-N
python3 tfg/bin/script.py 50 featuresTrainALL dicctionary diccOfClasses fcTrain50-N

#test:
./tfg/bin/featuresGenerator featuresTest ALL default < tfg/inputs/test.txt
python3 tfg/bin/script.py 0 featuresTestALL dicctionary diccOfClasses fcTest

python3 tfg/bin/script.py 5 featuresDevALL dicctionary diccOfClasses fcDev5-N
python3 tfg/bin/script.py 10 featuresDevALL dicctionary diccOfClasses fcDev10-N
python3 tfg/bin/script.py 50 featuresDevALL dicctionary diccOfClasses fcDev50-N

python3 tfg/bin/createDevelopmentFile.py 5
python3 tfg/bin/createDevelopmentFile.py 10
python3 tfg/bin/createDevelopmentFile.py 50