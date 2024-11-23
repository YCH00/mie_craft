#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 2) in vec3 aUv;

// 纹理坐标是3维的
out vec3 uv;
// 不用model转换到世界矩阵
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
void main()
{
    // 纹理坐标等于位置坐标/
    uv = aUv;
    // 注：此处的viewMatrix是经过处理的，取消了其平移部分
    vec4 pos = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
    // z为w，透视除法除后z=(z=w/w)=1，深度为最远///
    gl_Position = pos.xyww;
}
