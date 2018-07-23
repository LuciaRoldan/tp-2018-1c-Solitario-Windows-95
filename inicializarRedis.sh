#!/bin/bash

cd ../
# // - - workspace - - //

# - - -  CLONAR SO-COMMONS - - - 
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd ../

# - - -  CLONAR PARSI - - - 
git clone https://github.com/sisoputnfrba/parsi.git
cd parsi/
sudo make install
cd ../

# - - - CREAR CARPETAS DE INSTANCIA - - - 
cd ../
# en home/utnso
mkdir inst1
mkdir inst2
mkdir inst3

# - - - BUILDEAR COMMONS_PROPIAS - - - 
cd workspace/tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons_propias/Debug/
sudo make clean
sudo make all
cd ../..

# - - - BUILDEAR COMMONS - - - 
cd commons/Debug/
sudo make clean
sudo make all
cd ../../..

