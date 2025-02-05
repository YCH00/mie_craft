#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// 点光源
struct Light {
    vec3 position;  // 需要位置

    vec3 ambient;   
    vec3 diffuse;
    vec3 specular;

    //光照衰减常数
    float constant; // 常数
    float linear;   // 一次项
    float quadratic;// 二次项
};
uniform Light light;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
void main()
{
    // 环境光光照分量
    // 从漫反射纹理读取颜色分量
    vec3 ambient = light.ambient * material.ambient; 

    // 漫反射光照分量
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos); // 得到光源的方向,与一个点做光源一样
    float diff = max(dot(norm, lightDir), 0.0);         // 得到光源对当前片段实际的漫反射影响
    vec3 diffuse = light.diffuse * diff * material.diffuse;// 从漫反射纹理读取颜色分量

    // 镜面光照分量
    vec3 viewDir = normalize(viewPos - FragPos);        // 是观察者方向，不是观察者看向的方向
    vec3 reflectDir = reflect(-lightDir, norm);         // reflect要求第一个参数是光源指向像素点的向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// 光源对当前片段的镜面光影响
    // 采样镜面光纹理颜色作为镜面光照颜色分量
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // 这句也行
    vec3 specular = light.specular * spec * material.specular; 

    // 计算衰减
    float distance  = length(light.position - FragPos);                // 得到光源到片段长度
    float attenuation = 1.0 / (light.constant + light.linear * distance // 根据公式
                        + light.quadratic * distance * distance);
    
    // 光照分量随距离衰减
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular) ;
    
    FragColor = vec4(result, 1.0);
}
