#!/bin/bash

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

cd ESI/Debug/

make clean
make all

cd ../../

./ESI/Debug/ESI "/home/utnso/workspace/tp-2018-1c-Solitario-Windows-95/ESI/Pruebas-ESI/ESI_Largo" 2