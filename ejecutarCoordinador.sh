#!/bin/bash

cd Shared_Libraries/commons_propias/Debug/
make clean
make all

cd ../../../Coordinador/Debug

make clean
make all

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH=~/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons_propias/Debug/

./Coordinador 
