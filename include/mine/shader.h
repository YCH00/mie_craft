#pragma once

#include "core.h"

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    ~Shader();

    void begin();  //开始使用当前Shader
    void end();  //结束使用当前Shader

    void setFloat(const std::string& name, float value);  //供外界设置program中1f类型uniform变量的值
    void setVector3(const std::string& name, float x, float y, float z);
    void setVector3(const std::string& name, const float* values);
    void setInt(const std::string& name, int value);
    void setMartrix4x4(const std::string& name, glm::mat4 value);

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) ;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;



private:
    void checkShaderErrors(GLuint target, const std::string type);

private:
    GLuint mProgram{0};

};