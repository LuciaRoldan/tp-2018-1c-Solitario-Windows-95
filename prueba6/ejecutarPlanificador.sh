#!/bin/bash

cd ../Planificador/Debug

make clean
make all

export LD_LIBRARY_PATH=$PWD/../../Shared_Libraries/commons_propias/Debug

./planificador "../../prueba6/config_planificador"
