#!/bin/bash

export LD_LIBRARY_PATH=$PWD/../Shared_Libraries/commons_propias/Debug

cd ../Coordinador/Debug

make clean
make all

./Coordinador "../../prueba3/Configuracion_coordinadorEL.txt"