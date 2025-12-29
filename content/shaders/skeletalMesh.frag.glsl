#version 450

// Which formula to use for rendering.
const uint MATERIAL_SKIN = 0;
const uint MATERIAL_EYES = 1;
const uint MATERIAL_HAIR = 2;
const uint MATERIAL_APPAREL = 3;

// inputs from vertex shader.
layout(location = 0) in vec2 tex_coord_in;

// outputs color for pixel.
layout(location = 0) out vec4 outColor;

// set 2, sampled textures, storage textures, then storage buffers
layout(set = 2, binding = 0) uniform sampler2D colorTexture;

// set 3 uniform buffers
layout(set = 3, binding = 0) uniform MaterialBuffer {
    uint materialType;
    uint pad0;
    uint pad1;
    uint pad2;
    vec4 color0;
    vec4 color1;
    vec4 color2;
} material;

void main() {

    vec4 multiplyColor = vec4(1.0F, 1.0F, 1.0F, 1.0F);

    if (material.materialType == MATERIAL_SKIN) {

        outColor = mix(material.color0, material.color1, texture(colorTexture, tex_coord_in).r);
    }
    else if (material.materialType == MATERIAL_HAIR) {
        outColor = material.color0;
    }
    else if (material.materialType == MATERIAL_EYES) {
        float rampSelect = texture(colorTexture, tex_coord_in).r;
        vec4 pupilColor = material.color0;
        vec4 irisColor = material.color1;
        vec4 scleraColor = material.color2;

        if (rampSelect < 0.1) {
            outColor = scleraColor;
        }
        else if (rampSelect > 0.9F) {
            outColor = pupilColor;
        }
        else {
            outColor = irisColor;
        }
    }
    else if (material.materialType == MATERIAL_APPAREL) {

        outColor = material.color0;
    }

}