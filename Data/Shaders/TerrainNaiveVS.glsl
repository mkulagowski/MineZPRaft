layout (location=0) in vec3 InPos;
layout (location=1) in vec4 InColor;

out vec4 VSColor;

uniform mat4 worldMat;

void main()
{
    gl_Position = worldMat * vec4(InPos, 1.0);
    VSColor = InColor;
}
