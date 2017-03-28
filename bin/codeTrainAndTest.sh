#!/bin/bash

#train:
./tfg/bin/featuresGenerator featuresTrainALL ALL dicctionary < tfg/inputs/train.txt
python3.2 tfg/bin/script.py 5 featuresTrainALL dicctionary diccOfClasses fcTrain5-N
python3.2 tfg/bin/script.py 10 featuresTrainALL dicctionary diccOfClasses fcTrain10-N
python3.2 tfg/bin/script.py 50 featuresTrainALL dicctionary diccOfClasses fcTrain50-N

#dev:
./tfg/bin/featuresGenerator featuresDevALL ALL default < tfg/inputs/dev.txt
python3.2 tfg/bin/script.py 5 featuresDevALL dicctionary diccOfClasses fcDev5
python3.2 tfg/bin/script.py 10 featuresDevALL dicctionary diccOfClasses fcDev10
python3.2 tfg/bin/script.py 50 featuresDevALL dicctionary diccOfClasses fcDev50

python3.2 tfg/bin/createDevelopmentFile.py 5
python3.2 tfg/bin/createDevelopmentFile.py 10
python3.2 tfg/bin/createDevelopmentFile.py 50

#test:
./tfg/bin/featuresGenerator featuresTestALL ALL default < tfg/inputs/test.txt
python3.2 tfg/bin/script.py 0 featuresTestALL dicctionary diccOfClasses fcTest