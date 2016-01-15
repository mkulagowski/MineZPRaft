in vec3 GSNormal;
in vec3 GSLightDir;
in vec4 GSColor;

out vec4 Color;

void main()
{
    vec3 Normal = normalize(GSNormal);
    vec3 Dir = normalize(GSLightDir);
    float lambert = dot(Normal, Dir);

    if (lambert > 0.0f)
        Color = GSColor * lambert;
    else
        Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
