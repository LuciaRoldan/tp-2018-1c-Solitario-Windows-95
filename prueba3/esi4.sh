#!/bin/bash

export LD_LIBRARY_PATH=$PWD/../Shared_Libraries/commons_propias/Debug

cd ../ESI/Debug/

make clean
make all

cd ../../

./ESI/Debug/ESI "ESI/TestFinales/ESI_MultiReemplazo" 4