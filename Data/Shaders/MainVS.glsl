layout (location=0) in vec3 InPos;
layout (location=1) in vec4 InColor;

out vec4 VSColor;

void main()
{
    // Temporarily commented out. When TerrainGenerator will convert chunks to meshes, this will
    // come back.
    /*vec4 viewPos = viewMat * vec4(InPos, 1.0);
    gl_Position = perspMat * viewPos;*/
    gl_Position = vec4(InPos, 1.0);
    VSColor = InColor;
}
