#version 450

layout(location = 0) out vec4 fragColor;

void main() {
    vec2 pos;
    if (gl_VertexIndex == 0)
    {
        pos = vec2(-1.0f, -1.0f);
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        if (gl_VertexIndex == 1)
        {
            pos = vec2(1.0f, -1.0f);
            fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        }
        else
        {
            if (gl_VertexIndex == 2)
            {
                pos = vec2(0.0f, 1.0f);
                fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }
        }
    }
    gl_Position = vec4(pos, 0.0f, 1.0f);
}