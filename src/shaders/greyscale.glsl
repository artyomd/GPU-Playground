#version 460
#pragma shader_stage(compute)

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba8, set = 0, binding = 0) uniform readonly image2D inputImage;
layout(rgba8, set = 0, binding = 1) uniform writeonly image2D outputImage;
layout(std140, binding = 2) uniform ParamsUBO { float fade; } ubo;

void main() {
    ivec2 tex_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 image_dims = imageSize(inputImage);
    if (tex_coords.x >= image_dims.x || tex_coords.y >= image_dims.y) {
        return;
    }
    vec4 color = imageLoad(inputImage, tex_coords);
    float grey = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    vec3 greyscale_color = vec3(grey);
    vec3 outputColor = mix(greyscale_color, color.rgb, ubo.fade);
    imageStore(outputImage, tex_coords, vec4(outputColor.r, outputColor.g, outputColor.b, color.a));
}
