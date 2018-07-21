#!/bin/bash

export LD_LIBRARY_PATH=$PWD/../Shared_Libraries/commons_propias/Debug

cd ../Instancia/Debug

make clean
make all

./instancia "../../prueba1/Configuracion_instancia2.txt" 2