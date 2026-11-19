# Advanced Snake Game (C)

A console-based Snake game written in C for Windows using GCC/MinGW.

## Features
- Player mode with W/A/S/D controls
- AI mode using breadth-first search (BFS) to find food
- Easy, Medium, and Hard speed settings
- Score and persistent high score (`highscore.txt`)
- Wall and self-collision detection
- Pause and quit controls

## Requirements
- Windows 10/11
- GCC compiler (MinGW-w64)

## Build
Open a terminal in this folder:

```bash
gcc -std=c11 -Wall -Wextra -O2 main.c -o SnakeGame.exe
```

## Run
```bash
./SnakeGame.exe
```

On Windows PowerShell:
```powershell
.\SnakeGame.exe
```

## Controls
- W: Up
- A: Left
- S: Down
- D: Right
- P: Pause / resume
- Q: Quit current game

The high score is stored in `highscore.txt` in the working directory.
