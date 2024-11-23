#version 330 core
out vec4 FragColor;

// 纹理坐标是3维的
in vec3 uv;// 纹理坐标

uniform samplerCube box;

void main(){ 
    FragColor = texture(box, uv);
}
