#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aNormal;  // 法线信息

uniform mat4 transform;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 FragPos;
out vec3 Normal;  // 法线信息
out vec2 uv;

void main()
{
    FragPos = vec3(transform * vec4(aPos, 1.0));  // 插值之后变为某一个像素的世界坐标
     //根据物体移动改变顶点的法线信息
     //实际情况下，矩阵求逆的代价很高，
    Normal = mat3(transpose(inverse(transform))) * aNormal;  
    gl_Position = projectionMatrix * viewMatrix * transform * vec4(aPos, 1.0);
    uv = aUv;
}
