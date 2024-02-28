@echo off

set SOURCE_DIR=C:\Users\birud\OneDrive - iitgn.ac.in\Chip8
set OUTPUT_DIR=C:\Users\birud\OneDrive - iitgn.ac.in\Chip8
set SDL_INCLUDE=-I C:\msys64\mingw64\include\SDL2
set SDL_LIB=-L C:/msys64/mingw64/lib/cmake/SDL2 -lmingw32 -lSDL2main -lSDL2
set CFLAGS=-Wall

if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

set /p FILE_NAME=Enter the name of the file (without extension): 

set INPUT_FILE_C=%SOURCE_DIR%\%FILE_NAME%.c
set OUTPUT_FILE_C=%OUTPUT_DIR%\%FILE_NAME%.exe

set INPUT_FILE_CPP=%SOURCE_DIR%\%FILE_NAME%.cpp
set OUTPUT_FILE_CPP=%OUTPUT_DIR%\%FILE_NAME%.exe

if exist "%INPUT_FILE_C%" (
    echo Compiling %INPUT_FILE_C%...
    gcc %CFLAGS% %SDL_INCLUDE% "%INPUT_FILE_C%" %SDL_LIB% -o "%OUTPUT_FILE_C%"
    echo Compilation completed.
) else if exist "%INPUT_FILE_CPP%" (
    echo Compiling %INPUT_FILE_CPP%...
    g++ %CFLAGS% %SDL_INCLUDE% "%INPUT_FILE_CPP%" %SDL_LIB% -o "%OUTPUT_FILE_CPP%"
    echo Compilation completed.
) else (
    echo File not found: %FILE_NAME%.c or %FILE_NAME%.cpp
    exit /b 1
)
