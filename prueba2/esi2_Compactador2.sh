#!/bin/bash

cd ../ESI/Debug/

make clean
make all

cd ../../

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

./ESI/Debug/ESI "ESI/TestFinales/ESI_Compactador1" 2
#El documento de Tests nombra tanto el 1 como el 2. Supongo que el que va es el 2.