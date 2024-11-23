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

// ����ǿ��
struct Light {
    // vec3 position; // ʹ��ƽ�й�Ͳ�����Ҫλ����
    vec3 direction; // �ӹ�Դ������ȫ�ֵķ��򣨹�Դָ�����ص��λ�ã�

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
void main()
{
    // ������
    vec3 ambient = light.ambient * material.ambient; 

    // �������
    // ȡ����Ϊ��Դ�ķ������������ڼ���������;�������ʱ
    vec3 lightDir = normalize(-light.direction);
    vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(light.position - FragPos);// һ��������Դ����Ҫ�����������Ƭ�ε���Դ�ķ�������
    float diff = max(dot(norm, lightDir), 0.0);             // �õ���Դ�Ե�ǰƬ��ʵ�ʵ�������Ӱ��
    vec3 diffuse = light.diffuse * diff * material.diffuse; // �������������ȡ��ɫ����

    // ���淴���
    vec3 viewDir = normalize(viewPos - FragPos);            // �ǹ۲��߷��򣬲��ǹ۲��߿���ķ���
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);// ��Դ�Ե�ǰƬ�εľ����Ӱ��
    // vec3 specular = light.specular * (spec * material.specular); // �ı�������
    // ���������������ɫ��Ϊ���������ɫ����
    vec3 specular = light.specular * spec * material.specular; 

    vec3 result = (ambient + diffuse + specular) ;
    
    FragColor = vec4(result, 1.0);
}
