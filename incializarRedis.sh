#!/bin/bash

#export LD_LIBRARY_PATH=$PWD/Shared_Libraries/commons_propias/Debug

cd ../

# - - -  CLONAR SO-COMMONS - - - 
git clone https:// github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd ../

# - - -  CLONAR PARSI - - - 
git clone https://github.com/sisoputnfrba/parsi.git
cd parsi/
sudo make install
cd ../

# - - - BUILDEAR COMMONS_PROPIAS - - - 
cd tp-2018-1c-Solitario-Windows-95/Shared_Libraries/commons_propias/Debug
sudo make all

