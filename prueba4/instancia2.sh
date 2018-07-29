#!/bin/bash

cd ../Instancia/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./instancia "../../prueba4/Configuracion_instancia2.txt" 2