#!/bin/bash

cd ../Instancia/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./instancia "../../prueba5/Configuracion_instancia1.txt" 1