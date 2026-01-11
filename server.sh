#!/bin/bash
mkdir -p bin
g++ -std=c++23 -Iinclude -Iinclude/server server/* -o bin/Server.exe -lsqlite3
if [ $? -eq 0 ]; then
    echo "Compilation successful! Running application..."
    ./bin/Server.exe
else
    echo "Compilation failed."
    exit 1
fi