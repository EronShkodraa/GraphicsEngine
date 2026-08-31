
---

## How It Was Built

### 1. Window and Context (GLFW)
The window is created using GLFW. I set it to OpenGL 3.3 core profile because it's widely supported.

### 2. OpenGL Loading (GLAD)
GLAD loads all the OpenGL function pointers. I generated the GLAD files using the online generator.

### 3. Terrain Generation
The terrain is a grid of vertices. Heights are calculated using layered value noise — each layer has a higher frequency and lower amplitude. This creates natural-looking mountains.

### 4. Shaders
The vertex shader transforms the terrain and passes world position and normals to the fragment shader. The fragment shader:
- Calculates lighting
- Colors the terrain based on height
- Adds fog in the distance

### 5. Camera
The camera uses Euler angles (yaw and pitch). Mouse movement updates the angles, and WASD moves the camera relative to where you're looking.

### 6. Sound
When you move or turn, the engine plays short `Beep()` sounds through the Windows API. The pitch changes based on speed — it gives a raw retro feel.

---

## Requirements

- Windows (tested on Windows 10)
- MSYS2 with MinGW64 toolchain
- OpenGL 3.3 or higher
- Libraries: GLFW, GLM, GLAD

---

## How to Compile

1. Install [MSYS2](https://www.msys2.org/)
2. Install required packages:
   ```bash
   pacman -S mingw-w64-x86_64-glm mingw-w64-x86_64-glfw