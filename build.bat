@echo off
echo Building Terrain Engine...
g++ -std=c++17 -I./include -I./src src/main.cpp src/glad.c -LC:/msys64/mingw64/lib -lglfw3 -lopengl32 -lgdi32 -o terrain_engine.exe
if %errorlevel% equ 0 (
    echo Build successful!
) else (
    echo Build failed with error code: %errorlevel%
)
pause