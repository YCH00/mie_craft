#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aNormal;  // ������Ϣ

uniform mat4 transform;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 FragPos;
out vec3 Normal;  // ������Ϣ
out vec2 uv;

void main()
{
    FragPos = vec3(transform * vec4(aPos, 1.0));  // ��ֵ֮���Ϊĳһ�����ص���������
     //���������ƶ��ı䶥��ķ�����Ϣ
     //ʵ������£���������Ĵ��ۺܸߣ�
    Normal = mat3(transpose(inverse(transform))) * aNormal;  
    gl_Position = projectionMatrix * viewMatrix * transform * vec4(aPos, 1.0);
    uv = aUv;
}
