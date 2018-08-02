#!/bin/bash

cd ../Coordinador/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./Coordinador "../../prueba3/Configuracion_coordinador.txt"