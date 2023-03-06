# Welcome to GPU Playground!

This project is just a playground that I created for different experiments with different rendering techniques.
I planned to support various 3D rendering APIs but when trying to support at least 2 APIs (Vulkan and OpenGL)
I've noticed that things were getting more complicated and the focus was on how to develop a good level of abstraction
for graphics APIs, which is not the purpose of this project at all. So, for the sake of simplicity, this project will
now only use Vulkan.

## Building

As this project is a plain CMake project it can be built just like any other CMake project.
```bash
cmake -DCMAKE_BUILD_TYPE=Release -S <path-to-source> -B <path-to-build>
cmake --build <path-to-build> --target playground
```

## Preview
![](previews/preview0.png)

![](previews/preview1.png)

![](previews/preview2.png)

## Lisence

GPU Playground is licensed under the MIT License, see LICENSE for more information.

