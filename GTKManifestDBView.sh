#! /bin/sh -e
program="ManifestDBView"
./linguas.sh
echo "now compiling $program..." 
cd ./src
g++ `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config sqlite3 --cflags --libs` -c main.cpp manifestDBwindow.cpp
g++ main.o manifestDBwindow.o -o $program `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config sqlite3 --cflags --libs`
cd ..
./src/$program

