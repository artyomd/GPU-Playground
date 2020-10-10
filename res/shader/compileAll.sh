#!/bin/sh

rm -rf compiled
mkdir compiled

for i in *.glsl; do
  glslc "$i" -o compiled/"${i%.glsl}".spv
done
