#version 330 core
out vec4 FragColor;
// 材质属性
struct Material {
    samplerCube cubeTexture;
    vec3 specular;
    float shininess;
}; 
// 平行光
struct DirLight  {
    bool valid;
    vec3 direction; // 从光源出发到全局的方向

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
// 点光源
struct DotLight  {
    bool valid;
    vec3 position;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant; // 常数
    float linear;   // 一次项
    float quadratic;// 二次项
};

// 聚光灯
struct SpotLight {
    bool valid;

    vec3  position; // 需要位置
    vec3  direction;// 需要照射方向
    float cutOff;	// ?（内光切）
    float outerCutOff;// γ（外光切）

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant; // 常数
    float linear;   // 一次项
    float quadratic;// 二次项
};


#define MAX_DOT_LIGHTS 4
#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 1

uniform DotLight dotLights[MAX_DOT_LIGHTS];
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform SpotLight spotLights[MAX_DOT_LIGHTS];

uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec3 uv;// 纹理坐标

uniform vec3 viewPos;

// 计算平行光影响当前片段的颜色
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);// 取负变为光源的方向向量，用在计算漫反射和镜面光分量时
    // 漫反射光照分量
    float diff = max(dot(normal, lightDir), 0.0);// 得到光源对当前片段实际的漫反射影响
    // 镜面光光照分量
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 从纹理读取颜色分量
    vec3 ambient = light.ambient * vec3(texture(material.cubeTexture, uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.cubeTexture, uv));;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// 计算点光源影响当前片段的颜色
vec3 CalcDotLight(DotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射光照分量
    float diff = max(dot(normal, lightDir), 0.0);// 得到光源对当前片段实际的漫反射影响
    // 镜面光光照分量
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减系数
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                        light.quadratic * distance * distance);
    // 从纹理读取颜色分量
    vec3 ambient = light.ambient * vec3(texture(material.cubeTexture, uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.cubeTexture, uv));
    vec3 specular = light.specular * spec * material.specular;;
    // 衰减后
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// 计算物体对聚光灯反射的颜色
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    // 光源的方向向量，像素点指向光源，而不是光源指向像素点
    vec3 lightDir = normalize(light.position - fragPos);
    // 算出theta，dot（像素点指向光源的向量 与 光源的方向向量（依旧是像素点指向光源，只不过是照射方向上的像素点））
    float theta = dot(lightDir, normalize(-light.direction)); // = dot(-lightDir, normalize(light.direction));// 注释的是 dot(光源指向像素点 与 光源正前方照射方向指向像素点)
    // 执行正常光照计算：由于theta是cos值，cutoof也是cos值，cos(0-90)递减，所以theta>，而不是<

    // 为了边缘平滑
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 镜面光照
    vec3 reflectDir = reflect(-lightDir, normal);// reflect要求第一个参数是光源指向像素点的向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.cubeTexture, uv));; 
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.cubeTexture, uv));;// 不再是从设置的vec3强度
    vec3 specular = light.specular * spec * material.specular; 

    // 计算衰减
    float distance  = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance 
                        + light.quadratic * distance * distance);
    
    // 反射的光照随距离衰减
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // 受聚光灯影响，将不对环境光做出影响，让它总是能有一点光
    diffuse *= intensity;
    specular *= intensity;
    return (ambient + diffuse + specular);
}

void main()
{
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0,0.0,0.0);
    // 平行光
    for(int i = 0; i < MAX_DIR_LIGHTS; i++){
        if(dirLights[i].valid){
            result += CalcDirLight(dirLights[i], norm, viewDir);
        }
    }
    // 点光源
    for(int i = 0; i < MAX_DOT_LIGHTS; i++){
        if(dotLights[i].valid){
            result += CalcDotLight(dotLights[i], norm, FragPos, viewDir);
        }
    }
    
    // 聚光灯
    for(int i=0; i<MAX_SPOT_LIGHTS; i++){
        if(spotLights[i].valid){
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
        }
    }
    FragColor = vec4(result, 1.0);
}



