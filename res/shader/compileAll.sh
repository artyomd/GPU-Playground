#!/bin/sh

rm -rf compiled
mkdir compiled

for i in *.glsl; do
  glslc "$i" -o compiled/"${i%.glsl}".spv
  spirv-opt -O compiled/"${i%.glsl}".spv -o compiled/"${i%.glsl}".spv
done
