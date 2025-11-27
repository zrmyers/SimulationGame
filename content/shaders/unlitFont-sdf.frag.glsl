#version 450

layout(location = 0) in vec4 color_in;
layout(location = 1) in vec2 tex_coord_in;

layout(location = 0) out vec4 outColor;

// set 2, sampled textures, storage textures, then storage buffers
layout(set = 2, binding = 0) uniform sampler2D fontTexture;

void main() {
    const float smoothing = (1.0/16.0);
    float distance = texture(fontTexture, tex_coord_in).a;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
    outColor = vec4(color_in.rgb, color_in.a * alpha);
}