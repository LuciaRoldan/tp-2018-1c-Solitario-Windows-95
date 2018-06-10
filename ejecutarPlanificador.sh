#!/bin/bash

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

cd Planificador/Debug

make clean
make all

./planificador
