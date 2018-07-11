#!/bin/bash

export LD_LIBRARY_PATH=./Shared_Libraries/commons_propias/Debug

cd ../ESI/Debug/
make clean
make all
cd ../../

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1" 1

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1" 2

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1" 3

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_Largo" 4

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1" 5

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1" 6