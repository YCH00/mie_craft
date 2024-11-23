#include"mine/shader.h"
#include "mine/checkError.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    //1，完成vs，fs源代码，装载字符串
    // 装载shader代码的string
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    // vs、fs文件读取的文件流
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    std::ifstream gShaderFile;

    // ifstream遇到问题的时候抛出异常
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // if geometry shader path is present, also load a geometry shader
        if(geometryPath != nullptr)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR: Shader File Error: " << e.what() << std::endl;
        std::cout<< "vs_path: " <<vertexPath << std::endl;
        std::cout<< "fs_path: " <<fragmentPath << std::endl;
        if(geometryPath!=nullptr)
            std::cout<<"geo_path:"<<geometryPath<<std::endl;
    }

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    //2，创建Shader程序
    GLuint vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    //3，为Shader程序输入shader代码
    // GLuint glShaderSource(GLuint shader, GLsize count, const GLchar *const* string, const GLint *length);
    // shader：glCreateShader返回的shader程序句柄ID；  count：string字符串数组的长度
    // string：输入代码的字符串数组；  length：存储每一个字符串长度的数组（字符串有尾0可以输入NULL）
    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glShaderSource(fragment, 1, &fragmentShaderSource, NULL);

    //4，Shader代码编译
    glCompileShader(vertex);
    checkShaderErrors(vertex,"COMPILE");

    glCompileShader(fragment);
    checkShaderErrors(fragment,"COMPILE");

    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if(geometryPath != nullptr)
    {
        const char * gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkShaderErrors(geometry, "COMPILE");
    }

    //5，创建一个Program壳子
    mProgram = glCreateProgram();

    //6，将vs和fs编译好的结果放入program壳子里
    glAttachShader(mProgram,vertex);
    glAttachShader(mProgram,fragment);
    if(geometryPath != nullptr)
        glAttachShader(mProgram, geometry);

    //7，执行program的链接操作，形成最终可执行shader程序
    glLinkProgram(mProgram);
    checkShaderErrors(mProgram,"LINK");

    //8，清理
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if(geometryPath != nullptr)
        glDeleteShader(geometry);
    
}

Shader::~Shader()
{

}

void Shader::begin()  //开始使用当前Shader
{
    glUseProgram(mProgram);
}

void Shader::end()  //结束使用当前Shader
{
    glUseProgram(0);
}

void Shader::checkShaderErrors(GLuint target, const std::string type)
{
    int success = 0;
    char infoLog[1024]; //检验结果的数据准备

    if(type == "COMPILE"){
        glGetShaderiv(target, GL_COMPILE_STATUS, &success); //检查编译结果
        if(!success){
            glGetShaderInfoLog(target, 1024, NULL, infoLog);
            std::cout << "Error: SHADER COMPILE ERROR\n" << infoLog << std::endl;
        }
    }
    else if(type == "LINK"){
        glGetProgramiv(target, GL_LINK_STATUS, &success);  //检验链接错误
        if(!success){
            glGetProgramInfoLog(target, 1024, NULL, infoLog);
            std::cout << "Error: SHADER LINK ERROR\n" << infoLog << std::endl;
        }
    }
    else{
        std::cout << "Error: Check shader errors Type is wrong" << std::endl;
    }
}

//**若vs和fs中的uniform变量重名，两者相当于一个（只有一个备份，只用更新一次即可）
void Shader::setFloat(const std::string& name, float value)
{
    //1，通过名称获得uniform变量的location
    GLint location = GL_CALL(glGetUniformLocation(mProgram,name.c_str()));

    //2，通过location更改uniform变量的值
    GL_CALL(glUniform1f(location,value));
}

void Shader::setVector3(const std::string& name, float x, float y, float z)
{
    GLint location = GL_CALL(glGetUniformLocation(mProgram,name.c_str()));
    GL_CALL(glUniform3f(location,x,y,z));
}

void Shader::setVector3(const std::string& name, const float* values)
{
    GLint location = GL_CALL(glGetUniformLocation(mProgram,name.c_str()));
    
    // glUniform3fv函数的第二个参数表示当前更新的uniform变量如果是数组，数组里包含几个向量vec3
    GL_CALL(glUniform3fv(location,1,values));
}

void Shader::setInt(const std::string& name, int value)
{
    GLint location = GL_CALL(glGetUniformLocation(mProgram,name.c_str()));
    GL_CALL(glUniform1i(location,value));
}

void Shader::setMartrix4x4(const std::string& name, glm::mat4 value)
{
    GLint location = GL_CALL(glGetUniformLocation(mProgram,name.c_str()));
    // opengl中存储矩阵是使用 列优先存储，如果原始数据是使用行优先存储，则需要进行转置；
    glUniformMatrix4fv(location,1,false,glm::value_ptr(value));  //GLboolean transpose：表示是否需要转置
}

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(mProgram, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(mProgram, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(mProgram, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(mProgram, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(mProgram, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(mProgram, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) 
{ 
    glUniform4f(glGetUniformLocation(mProgram, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}