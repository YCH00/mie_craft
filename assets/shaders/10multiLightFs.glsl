#version 330 core
out vec4 FragColor;
// ��������
struct Material {
    samplerCube cubeTexture;
    vec3 specular;
    float shininess;
}; 
// ƽ�й�
struct DirLight  {
    bool valid;
    vec3 direction; // �ӹ�Դ������ȫ�ֵķ���

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
// ���Դ
struct DotLight  {
    bool valid;
    vec3 position;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant; // ����
    float linear;   // һ����
    float quadratic;// ������
};

// �۹��
struct SpotLight {
    bool valid;

    vec3  position; // ��Ҫλ��
    vec3  direction;// ��Ҫ���䷽��
    float cutOff;	// ?���ڹ��У�
    float outerCutOff;// �ã�����У�

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant; // ����
    float linear;   // һ����
    float quadratic;// ������
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
in vec3 uv;// ��������

uniform vec3 viewPos;

// ����ƽ�й�Ӱ�쵱ǰƬ�ε���ɫ
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);// ȡ����Ϊ��Դ�ķ������������ڼ���������;�������ʱ
    // ��������շ���
    float diff = max(dot(normal, lightDir), 0.0);// �õ���Դ�Ե�ǰƬ��ʵ�ʵ�������Ӱ��
    // �������շ���
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �������ȡ��ɫ����
    vec3 ambient = light.ambient * vec3(texture(material.cubeTexture, uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.cubeTexture, uv));;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// ������ԴӰ�쵱ǰƬ�ε���ɫ
vec3 CalcDotLight(DotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������շ���
    float diff = max(dot(normal, lightDir), 0.0);// �õ���Դ�Ե�ǰƬ��ʵ�ʵ�������Ӱ��
    // �������շ���
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��ϵ��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                        light.quadratic * distance * distance);
    // �������ȡ��ɫ����
    vec3 ambient = light.ambient * vec3(texture(material.cubeTexture, uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.cubeTexture, uv));
    vec3 specular = light.specular * spec * material.specular;;
    // ˥����
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// ��������Ծ۹�Ʒ������ɫ
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    // ��Դ�ķ������������ص�ָ���Դ�������ǹ�Դָ�����ص�
    vec3 lightDir = normalize(light.position - fragPos);
    // ���theta��dot�����ص�ָ���Դ������ �� ��Դ�ķ������������������ص�ָ���Դ��ֻ���������䷽���ϵ����ص㣩��
    float theta = dot(lightDir, normalize(-light.direction)); // = dot(-lightDir, normalize(light.direction));// ע�͵��� dot(��Դָ�����ص� �� ��Դ��ǰ�����䷽��ָ�����ص�)
    // ִ���������ռ��㣺����theta��cosֵ��cutoofҲ��cosֵ��cos(0-90)�ݼ�������theta>��������<

    // Ϊ�˱�Եƽ��
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // ������
    float diff = max(dot(normal, lightDir), 0.0);

    // �������
    vec3 reflectDir = reflect(-lightDir, normal);// reflectҪ���һ�������ǹ�Դָ�����ص������
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.cubeTexture, uv));; 
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.cubeTexture, uv));;// �����Ǵ����õ�vec3ǿ��
    vec3 specular = light.specular * spec * material.specular; 

    // ����˥��
    float distance  = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance 
                        + light.quadratic * distance * distance);
    
    // ����Ĺ��������˥��
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // �ܾ۹��Ӱ�죬�����Ի���������Ӱ�죬������������һ���
    diffuse *= intensity;
    specular *= intensity;
    return (ambient + diffuse + specular);
}

void main()
{
    // ����
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0,0.0,0.0);
    // ƽ�й�
    for(int i = 0; i < MAX_DIR_LIGHTS; i++){
        if(dirLights[i].valid){
            result += CalcDirLight(dirLights[i], norm, viewDir);
        }
    }
    // ���Դ
    for(int i = 0; i < MAX_DOT_LIGHTS; i++){
        if(dotLights[i].valid){
            result += CalcDotLight(dotLights[i], norm, FragPos, viewDir);
        }
    }
    
    // �۹��
    for(int i=0; i<MAX_SPOT_LIGHTS; i++){
        if(spotLights[i].valid){
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
        }
    }
    FragColor = vec4(result, 1.0);
}



