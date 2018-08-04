#!/bin/bash

cd ../Instancia/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./instancia "../../prueba6/Configuracion_instancia3.txt" 3