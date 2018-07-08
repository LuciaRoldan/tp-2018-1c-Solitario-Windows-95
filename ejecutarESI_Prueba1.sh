#!/bin/bash

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

cd ESI/Debug/

make clean
make all

./ESI/Debug/ESI "ESI/Prubeas-ESI/ESI_1.txt" 1