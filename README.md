# eXperimental Engine

Extremely simple realtime renderer

## Features

- [x] Physical based rendering(PBR)
- [x] Image based lighting(IBL)
- [x] Screen space ambient occlusion(SSAO)
- [x] Screen space reflection(SSR)
- [x] Deferred pipeline
- [x] Particle system(firework only)
- [x] Post effect

## Dependencies

Third parties kits:

* GLM (Math library)
* GLAD (OpenGL extension)
* GLFW (Window controller)
* Assimp (Resources loader)
* ImGUI (GUI)

CXX standards:

* c++17 (only required for filesystem.cpp)

## Build

> mkdir build && cd build
> cmake ..

## Running

* Set repository folder as working directory
* Add all DLLs(assimp only here) to startup path

In the case of Visual Studio:

* Select project "demo" and choose "Set as Startup Project"
* Go to "Properties" of "demo" then "Debugging" page
* Set "Working Directory" as "$(ProjectDir)../../"
* Set "Environment" as "PATH=$(ProjectDir)../../3rdparty/assimp/bin;%PATH%"
* All above apply if your build folder is under repo folder

## Usage

* Mouse control(view)
* Keyboard WASD(move) Q(up) E(down)
* Keyboard Tab(show/hide ui board)
* Keyboard 1(scene 1) 2(scene 2) 3(null scene)

## Effect

![Alt text](scene1.png?raw=true "Effect")

![Alt text](scene2.png?raw=true "Effect")
