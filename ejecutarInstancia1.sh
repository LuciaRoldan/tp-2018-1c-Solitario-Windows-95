#!/bin/bash

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

cd Instancia/Debug

make clean
make all

./instancia "../Configuracion_instancia.txt" 1
