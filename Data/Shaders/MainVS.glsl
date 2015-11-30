layout (location=0) in vec3 InPos;
layout (location=1) in vec4 InColor;

out vec4 VSColor;

uniform mat4 viewMat;
uniform mat4 perspMat;

void main()
{
    vec4 viewPos = viewMat * vec4(InPos, 1.0);
    gl_Position = perspMat * viewPos;
    VSColor = InColor;
}
