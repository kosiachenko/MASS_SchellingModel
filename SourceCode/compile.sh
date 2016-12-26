#!/bin/sh

export MASS_DIR=../..
g++ -Wall Resident.cpp -I$MASS_DIR/source -shared -fPIC -o Resident
g++ -Wall House.cpp -I$MASS_DIR/source -shared -fPIC -o House
g++ -Wall main.cpp Timer.cpp -I$MASS_DIR/source -L$MASS_DIR/ubuntu -lmass -I$MASS_DIR/ubuntu/libssh2-1.4.2/include -L$MASS_DIR/ubuntu -lssh2 -o main
