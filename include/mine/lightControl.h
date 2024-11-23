#pragma once
#include"mine/core.h"
#include"mine/shader.h"
#define MAX_DOT_LIGHTS 4
#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 1

class LightControl {
public:
    LightControl();
    void addDotLight(const int& seq, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, const float& constant=1.0f, const float& linear=0.09f, const float& quadratic=0.032f);
    void addSpotLight(const int& seq, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, const glm::vec3& direction, const float& cutOff=glm::cos(glm::radians(12.5f)), const float& outerCutOff=glm::cos(glm::radians(17.5f)), const float& constant=1.0f, const float& linear=0.09f, const float& quadratic=0.032f);
    void addDirLight(const int& seq, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction=glm::vec3(0.0f,-1.0f,0.0f));
    void closeLight(const std::string& lightType, const int& seq);
    void openLight(const std::string& lightType, const int& seq);
    void showAllLights(void);
    void setShader(Shader* shader);
private:
    bool mDirLight[MAX_DIR_LIGHTS] = {false};
    bool mSpotLight[MAX_SPOT_LIGHTS] = {false};
    bool mDotLight[MAX_DOT_LIGHTS] = {false};
    Shader* mShader{nullptr};

};