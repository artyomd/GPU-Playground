for i in *.glsl
do
     glslc "$i" -o compiled/"${i%.glsl}".spv
done
