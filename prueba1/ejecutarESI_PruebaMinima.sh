#!/bin/bash

export LD_LIBRARY_PATH=$PWD/../Shared_Libraries/commons_propias/Debug

cd ../ESI/Debug/

make clean
make all

cd ../../

./ESI/Debug/ESI "ESI/TestFinales/ESI_Largo" 1

./ESI/Debug/ESI "ESI/TestFinales/ESI_MilanesaCompleta" 2

./ESI/Debug/ESI "ESI/TestFinales/ESI_MultiClave" 3

./ESI/Debug/ESI "ESI/TestFinales/ESI_Tiramisu" 4

./ESI/Debug/ESI "ESI/TestFinales/ESI_MenuParrilla" 5