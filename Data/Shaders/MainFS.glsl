in vec3 VSNormal;
in vec3 VSLightDir;
in vec4 VSColor;

out vec4 Color;

void main()
{
    vec3 Normal = normalize(VSNormal);
    vec3 Dir = normalize(VSLightDir);
    float lambert = dot(Normal, Dir);

    if (lambert > 0.0f)
        Color = VSColor * lambert;
    else
        Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
