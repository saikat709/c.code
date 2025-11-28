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
SOURCES="src/main.cpp src/login.cpp src/ParticleSystem.cpp src/UI.cpp src/LoginScreen.cpp src/RegisterScreen.cpp src/CodeEditorScreen.cpp"

echo "Compiling..."
$CXX $CXXFLAGS $INCLUDES $SOURCES -o $OUTPUT $LIBS

if [ $? -eq 0 ]; then
    echo "Compilation successful! Running application..."
    ./$OUTPUT
else
    echo "Compilation failed."
    exit 1
fi
