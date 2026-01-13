#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p bin

# Compilation settings
CXX=g++
CXXFLAGS="-std=c++23 -w -Wall -Wextra"
INCLUDES="-Iinclude -Iinclude/src -Ilib"
LIBS="-lsfml-graphics -lsfml-window -lsfml-system"

# Output file
OUTPUT="bin/c.code"

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
