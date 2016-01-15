in vec4 VSNormal;
in vec4 VSLightDir;
in vec4 VSColor;

out vec4 Color;

void main()
{
    vec4 Normal = normalize(VSNormal);
    vec4 Dir = normalize(VSLightDir);
    float lambert = dot(Normal, Dir);

    if (lambert > 0.0f)
        Color = VSColor * lambert;
    else
        Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
