#! /bin/sh -e
./linguas.sh
echo "now compiling ManifestDBView..." 
cd ./src
g++ `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config sqlite3 --cflags --libs` -c main.cpp manifestDBwindow.cpp
g++ main.o manifestDBwindow.o -o ManifestDBView `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config sqlite3 --cflags --libs`
cd ..
./src/ManifestDBView

