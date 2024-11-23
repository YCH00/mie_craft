#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

// ���Դ
struct Light {
    vec3 position;  // ��Ҫλ��

    vec3 ambient;   
    vec3 diffuse;
    vec3 specular;

    //����˥������
    float constant; // ����
    float linear;   // һ����
    float quadratic;// ������
};
uniform Light light;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
void main()
{
    // ��������շ���
    // �������������ȡ��ɫ����
    vec3 ambient = light.ambient * material.ambient; 

    // ��������շ���
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos); // �õ���Դ�ķ���,��һ��������Դһ��
    float diff = max(dot(norm, lightDir), 0.0);         // �õ���Դ�Ե�ǰƬ��ʵ�ʵ�������Ӱ��
    vec3 diffuse = light.diffuse * diff * material.diffuse;// �������������ȡ��ɫ����

    // ������շ���
    vec3 viewDir = normalize(viewPos - FragPos);        // �ǹ۲��߷��򣬲��ǹ۲��߿���ķ���
    vec3 reflectDir = reflect(-lightDir, norm);         // reflectҪ���һ�������ǹ�Դָ�����ص������
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// ��Դ�Ե�ǰƬ�εľ����Ӱ��
    // ���������������ɫ��Ϊ���������ɫ����
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)); // ���Ҳ��
    vec3 specular = light.specular * spec * material.specular; 

    // ����˥��
    float distance  = length(light.position - FragPos);                // �õ���Դ��Ƭ�γ���
    float attenuation = 1.0 / (light.constant + light.linear * distance // ���ݹ�ʽ
                        + light.quadratic * distance * distance);
    
    // ���շ��������˥��
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular) ;
    
    FragColor = vec4(result, 1.0);
}
