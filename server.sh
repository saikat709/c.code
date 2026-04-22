#!/bin/bash

mkdir -p bin

if [ -f bin/server ]; then
    mv bin/server bin/server_old
fi

g++ -std=c++23 -Iinclude -Iinclude/server server/*.cpp lib/*.cpp -o bin/server -lsqlite3

if [ $? -eq 0 ]; then
    echo "Compilation successful! Running application..."
    ./bin/server
else
    echo "Compilation failed."
    exit 1
fi