#version 450
#extension GL_GOOGLE_include_directive  : enable
#include "common.glsl"

layout(location = 0) in vec3 position_in;
layout(location = 1) in vec4 color_in;
layout(location = 2) in vec2 tex_coord_in;

layout(location = 0) out vec4 color_out;
layout(location = 1) out vec2 tex_coord_out;

layout(set = 1, binding = 0) uniform UBO {
    mat4 proj_view;
    mat4 model;
} ubo;

void main() {
    gl_Position = ubo.proj_view * ubo.model * vec4(position_in, 1.0F);
    color_out = color_in;
    tex_coord_out = tex_coord_in;
}