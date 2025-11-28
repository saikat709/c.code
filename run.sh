#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p bin

# Compilation settings
CXX=g++
CXXFLAGS="-std=c++23 -Wall -Wextra"
INCLUDES="-Iinclude"
LIBS="-lsfml-graphics -lsfml-window -lsfml-system"

# Output file
OUTPUT="bin/LoginApp.exe"

# Source files
SOURCES="src/main.cpp src/login.cpp"

echo "Compiling..."
$CXX $CXXFLAGS $INCLUDES $SOURCES -o $OUTPUT $LIBS

if [ $? -eq 0 ]; then
    echo "Compilation successful! Running application..."
    ./$OUTPUT
else
    echo "Compilation failed."
    exit 1
fi
