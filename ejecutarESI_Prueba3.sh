#!/bin/bash

cd ESI/Debug/
make clean
make all
cd ../../

export LD_LIBRARY_PATH=./Shared_Libraries/commons_propias/Debug

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_MultiClave" 1

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_Largo" 2