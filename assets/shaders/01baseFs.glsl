#version 460 core
out vec4 FragColor;

uniform sampler2D Sampler;
// uniform int width;
// uniform int height;

in vec2 uv;

// shader中，颜色的范围均为0-1，可作为混合比例

void main()
{
    /* 自己选择使用哪一个level的MipMap（需要注销掉texture中对filter的设置）：
    vec2 location = uv * vec2(width,height);
    vec2 dx = dFdx(location);
    vec2 dy = dFdy(location);
    float maxDelta = sqrt(max(dot(dx,dx),dot(dy,dy)));
    float L = log2(maxDelta);
    int level = max(int(L+0.5),0);
    FragColor = textureLod(Sampler, uv, level);
    */
    FragColor = texture(Sampler,uv);
}