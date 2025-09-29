#version 460
#pragma shader_stage(compute)

layout(std140, binding = 0) uniform ParameterUBO { uint deltaTime; } ubo;

layout(std430, binding = 1) readonly buffer PositionsIn { vec2 positionsIn[]; };

layout(std430, binding = 2) readonly buffer VelocityIn { vec2 velocityIn[]; };

layout(std430, binding = 3) buffer PositionsOut { vec2 positionsOut[]; };

layout(std430, binding = 4) buffer VelocityOut { vec2 velocityOut[]; };

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main(){
    uint index = gl_GlobalInvocationID.x;
    vec2  position = positionsIn[index];
    vec2 result = position + velocityIn[index].xy * ubo.deltaTime;
    velocityOut[index] = velocityIn[index];
    if ((result.x <= -1.0) || (result.x >= 1.0)) { velocityOut[index].x = -velocityOut[index].x; }
    if ((result.y <= -1.0) || (result.y >= 1.0)) { velocityOut[index].y = -velocityOut[index].y; }
    positionsOut[index] = result;
}
