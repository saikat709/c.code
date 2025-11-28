# LoginApp

A modern C++23 project with an **astonishing** SFML-based login screen.

## Features
- **Modern UI/UX**:
  - **Glassmorphism Design**: Semi-transparent cards and blur-like effects.
  - **Dynamic Background**: Animated particle system with floating nodes.
  - **Interactive Elements**: Hover effects, focus states, and smooth transitions.
  - **Typography**: Uses Segoe UI for a clean, modern look.
- **C++23 & SFML 3.0**: Built with the latest standards.
- **Networking**: Ready for socket integration.

## Structure
- `src/` - Source files (`main.cpp`, `login.cpp`)
- `include/` - Header files (`login.hpp`)
- `bin/` - Compiled executable

## Build & Run
### Prerequisites
- MSYS2 with MinGW 64-bit
- SFML 3.0 (`mingw-w64-x86_64-sfml`)
- GCC 13+ (C++23 support)

### Running
Simply execute the run script:
```bash
./run.sh
```

Or compile manually:
```bash
g++ -std=c++23 -Wall -Wextra -Iinclude src/main.cpp src/login.cpp -o bin/LoginApp.exe -lsfml-graphics -lsfml-window -lsfml-system
./bin/LoginApp.exe
```

## Controls
- **Tab**: Switch between Username and Password fields.
- **Mouse**: Click to focus fields or click Login.
- **Backspace**: Delete text.
