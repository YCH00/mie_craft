#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
// uniform vec3 objectColor;
// uniform vec3 lightColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

// 光源分为3个光照分量
struct Light {
    vec3 position;
    vec3 ambient;  // 光源环境光颜色分量
    vec3 diffuse;  // 光源漫反射颜色分量
    vec3 specular; // 光源镜面反射颜色分量
};
uniform Light light;

void main(){
    // 环境光
    // float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * lightColor;
    //vec3 ambient = lightColor * material.ambient;
    vec3 ambient = light.ambient * material.ambient;            // 环境光照分量

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);                 // 得到光源对当前片段实际的漫反射影响
    // vec3 diffuse = diff * lightColor;
    // vec3 diffuse = lightColor * diff *  material.diffuse;
    vec3 diffuse = light.diffuse * diff *  material.diffuse;    // 漫反射光照分量

    // 镜面光照
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);                // 是观察者方向，不是观察者看向的方向
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// 光源对当前片段的镜面光影响
    //vec3 specular = specularStrength * spec * lightColor;
    //vec3 specular = lightColor * (spec * material.specular);
    vec3 specular = light.specular * (spec * material.specular);// 镜面光光照分量

    //vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = (ambient + diffuse + specular);               // 不用乘以物体颜色，材质已经决定了物体的颜色
    
    FragColor = vec4(result, 1.0);
}

// eg.
// lightingShader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
// lightingShader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
// lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
// lightingShader.setFloat("material.shininess", 32.0f);
// lightingShader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
// lightingShader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
// lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 
