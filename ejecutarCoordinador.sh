#!/bin/bash

export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

<<<<<<< Updated upstream
./Coordinador/Debug/Coordinador
=======
cd Coordinador/Debug
>>>>>>> Stashed changes

make clean
make all

./coordinador_copia 
