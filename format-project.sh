#!/bin/sh

for file_name in ./src/**/*.{cpp,h,hpp}; do
    if [ -f "$file_name" ]; then
        printf '%s\n' "$file_name"
        clang-format -i -style=Google $file_name
    fi
done
for file_name in ./res/shader/**/*.glsl; do
    if [ -f "$file_name" ]; then
        printf '%s\n' "$file_name"
        clang-format -i -style=Google $file_name
    fi
done
