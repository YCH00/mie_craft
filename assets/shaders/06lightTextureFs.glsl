#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;// 纹理坐标

uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;// 纹理单元
    //vec3 specular;    // 镜面光照颜色分量依旧是手动设置
    sampler2D specular;//  镜面光照颜色分量从纹理采样
    float shininess;
}; 
uniform Material material;

// 光照强度
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
void main()
{
    // 环境光照分量
    float ambientStrength = 0.1;
    // 从漫反射纹理读取颜色分量
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)); 

    // 漫反射光照分量
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);                 // 得到光源对当前片段实际的漫反射影响
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));// 从漫反射纹理读取颜色分量

    // 镜面光照分量
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos); // 是观察者方向，不是观察者看向的方向
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// 光源对当前片段的镜面光影响
    // vec3 specular = light.specular * (spec * material.specular); // 改变在这里
    // 采样镜面光纹理颜色作为镜面光照颜色分量
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); 

    vec3 result = (ambient + diffuse + specular) ;
    
    FragColor = vec4(result, 1.0);
}
