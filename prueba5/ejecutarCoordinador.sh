#!/bin/bash

cd ../Coordinador/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./Coordinador "../../prueba5/Configuracion_coordinador.txt"