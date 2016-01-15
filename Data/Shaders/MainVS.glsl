layout (location=0) in vec3 InPos;
layout (location=1) in vec3 InNorm;
layout (location=2) in vec4 InColor;

out vec4 VSNormal;
out vec4 VSLightDir;
out vec4 VSColor;

uniform mat4 worldMat;
uniform mat4 viewMat;
uniform mat4 perspMat;
uniform vec4 playerPos;

void main()
{
    mat4 viewPerspMat = perspMat * viewMat;
    vec4 worldPos = worldMat * vec4(InPos, 1.0);
    gl_Position = viewPerspMat * worldPos;

    VSNormal = viewMat * vec4(InNorm, 0.0);
    VSLightDir = viewMat * (playerPos - worldPos);
    VSColor = InColor;
}
