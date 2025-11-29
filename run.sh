#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p bin

# Compilation settings
CXX=g++
CXXFLAGS="-std=c++23 -Wall -Wextra"
INCLUDES="-Iinclude -Iinclude\\src"
LIBS="-lsfml-graphics -lsfml-window -lsfml-system"

# Output file
OUTPUT="bin/C.Code.exe"

# Source files
SOURCES="src/* lib/*"

echo "Compiling..."
$CXX $CXXFLAGS $INCLUDES $SOURCES -o $OUTPUT $LIBS

if [ $? -eq 0 ]; then
    echo "Compilation successful! Running application..."
    ./$OUTPUT
else
    echo "Compilation failed."
    exit 1
fi
