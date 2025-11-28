# LoginApp

A modern C++23 project with an **astonishing** SFML-based login and registration system.

## Features
- **Modular Architecture**: Separated concerns into distinct classes and files.
- **Modern UI/UX**:
  - **Glassmorphism Design**: Semi-transparent cards and blur-like effects.
  - **Dynamic Background**: Animated particle system with floating nodes.
  - **Interactive Elements**: Hover effects, focus states, and smooth transitions.
  - **Typography**: Uses Segoe UI for a clean, modern look.
- **Navigation**: Seamless switching between Login and Register screens.
- **C++23 & SFML 3.0**: Built with the latest standards.

## Structure
- `src/`
  - `main.cpp`: Entry point and state management.
  - `LoginScreen.cpp`: Login UI and logic.
  - `RegisterScreen.cpp`: Registration UI and logic.
  - `ParticleSystem.cpp`: Background animation logic.
  - `UI.cpp`: Reusable UI components (InputField, Button).
  - `login.cpp`: Authentication logic.
- `include/`
  - Headers for all the above components.
- `bin/`: Compiled executable.

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
g++ -std=c++23 -Wall -Wextra -Iinclude src/main.cpp src/login.cpp src/ParticleSystem.cpp src/UI.cpp src/LoginScreen.cpp src/RegisterScreen.cpp -o bin/LoginApp.exe -lsfml-graphics -lsfml-window -lsfml-system
./bin/LoginApp.exe
```

## Controls
- **Tab**: Switch between fields.
- **Mouse**: Click to focus fields or buttons.
- **Backspace**: Delete text.
