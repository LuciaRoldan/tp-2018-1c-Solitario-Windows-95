#!/bin/bash

cd ../Planificador/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./planificador "../../prueba3/config_planificadorSJFSD"
