#!/bin/bash

cd ESI/Debug/

make clean
make all

export LD_LIBRARY_PATH=./Shared_Libraries/commons_propias/Debug

./ESI "ESI/Prubeas-ESI/ESI_1.txt"