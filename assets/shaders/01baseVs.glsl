#version 460 core
layout(location = 0) in vec3 aPos;
// layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUv;

uniform mat4 transform;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 uv;

void main()
{
    vec4 position = vec4(aPos,1.0);
    position = projectionMatrix * viewMatrix * transform * position;
    gl_Position = position;

    uv = aUv;
}

// ps. 使用这个shader之前需要改变纹理的类型以及vao中uv坐标