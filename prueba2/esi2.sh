#!/bin/bash

cd ../ESI/Debug/

make clean
make all

cd ../../

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

./ESI/Debug/ESI "ESI/TestFinales/ESI_Compactador1" 2