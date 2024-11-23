#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// 光照强度
struct Light {
    // vec3 position; // 使用平行光就不再需要位置了
    vec3 direction; // 从光源出发到全局的方向（光源指向像素点的位置）

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
void main()
{
    // 环境光
    vec3 ambient = light.ambient * material.ambient; 

    // 漫反射光
    // 取负变为光源的方向向量，用在计算漫反射和镜面光分量时
    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - FragPos);// 一个点做光源才需要这样相减计算片段到光源的方向向量
    float diff = max(dot(norm, lightDir), 0.0);             // 得到光源对当前片段实际的漫反射影响
    vec3 diffuse = light.diffuse * diff * material.diffuse; // 从漫反射纹理读取颜色分量

    // 镜面反射光
    vec3 viewDir = normalize(viewPos - FragPos);            // 是观察者方向，不是观察者看向的方向
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// 光源对当前片段的镜面光影响
    // vec3 specular = light.specular * (spec * material.specular); // 改变在这里
    // 采样镜面光纹理颜色作为镜面光照颜色分量
    vec3 specular = light.specular * spec * material.specular; 

    vec3 result = (ambient + diffuse + specular) ;
    
    FragColor = vec4(result, 1.0);
}
