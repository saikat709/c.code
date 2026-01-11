# C.CODE

## Install
1.  **Install MSYS2**: Download and install from [msys2.org](https://www.msys2.org/).
2.  **Install Dependencies**: Open MSYS2 UCRT64 terminal and run:
    ```bash
    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sfml mingw-w64-x86_64-nlohmann-json mingw-w64-x86_64-sqlite3
    ```

## For linux
    ```bash
    sudo apt update
    sudo apt install libsfml-dev
    sudo apt install nlohmann-json3-dev
    sudo apt install libsqlite3-dev
    ``

## Setup
1.  **Clone/Open**: Open this folder in VS Code.
2.  **Build**: Open the integrated terminal (Git Bash or MSYS2) and run:
    ```bash
    ./run.sh    # For Client
    ./server.sh # For Server
    ```


## Run
- **Client**: `./bin/C.Code.exe`
- **Server**: `./bin/server.exe`