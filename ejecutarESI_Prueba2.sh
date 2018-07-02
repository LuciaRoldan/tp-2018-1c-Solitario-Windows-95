#!/bin/bash

cd ESI/Debug/
make clean
make all
cd ../../

export LD_LIBRARY_PATH=./Shared_Libraries/commons_propias/Debug

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1.txt" 1

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1.txt" 2

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1.txt" 3

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_Largo.txt" 4

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1.txt" 5

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1.txt" 6