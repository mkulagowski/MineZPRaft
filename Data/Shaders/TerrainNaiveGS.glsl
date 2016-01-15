layout (points) in;
in vec4 VSColor[];

layout (triangle_strip, max_vertices = 24) out;
out vec3 GSNormal;
out vec3 GSLightDir;
out vec4 GSColor;

uniform mat4 viewMat;
uniform mat4 perspMat;
uniform vec4 playerPos;

void main()
{
    GSColor = VSColor[0];
    mat4 viewPerspMat = perspMat * viewMat;

    // up
    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    GSNormal = vec3(viewMat * vec4(0.0f, 1.0f, 0.0f, 0.0f));
    GSLightDir = vec3(viewMat * (playerPos - gl_in[0].gl_Position));
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, 0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, 0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();
    EndPrimitive();

    // right
    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, 0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    GSNormal = vec3(viewMat * vec4(1.0f, 0.0f, 0.0f, 0.0f));
    GSLightDir = vec3(viewMat * (playerPos - gl_in[0].gl_Position));
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, 0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f,-0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f,-0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();
    EndPrimitive();

    // down
    gl_Position = gl_in[0].gl_Position + vec4( 0.5f,-0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    GSNormal = vec3(viewMat * vec4(0.0f,-1.0f, 0.0f, 0.0f));
    GSLightDir = vec3(viewMat * (playerPos - gl_in[0].gl_Position));
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f,-0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f,-0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f,-0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();
    EndPrimitive();

    // left
    gl_Position = gl_in[0].gl_Position + vec4(-0.5f,-0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    GSNormal = vec3(viewMat * vec4(-1.0f, 0.0f, 0.0f, 0.0f));
    GSLightDir = vec3(viewMat * (playerPos - gl_in[0].gl_Position));
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f,-0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();
    EndPrimitive();

    // front
    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    GSNormal = vec3(viewMat * vec4(0.0f, 0.0f, 1.0f, 0.0f));
    GSLightDir = vec3(viewMat * (playerPos - gl_in[0].gl_Position));
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f,-0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, 0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f,-0.5f, 0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();
    EndPrimitive();

    // back
    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    GSNormal = vec3(viewMat * vec4(0.0f, 0.0f,-1.0f, 0.0f));
    GSLightDir = vec3(viewMat * (playerPos - gl_in[0].gl_Position));
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, 0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f,-0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f,-0.5f,-0.5f, 0.0f);
    gl_Position = viewPerspMat * gl_Position;
    EmitVertex();
}
