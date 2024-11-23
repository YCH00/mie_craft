#include"mine/lightControl.h"
#include<iostream>

LightControl::LightControl()
{

}

void LightControl::addDotLight(const int& seq, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, const float& constant, const float& linear, const float& quadratic)
{
    if(seq<0 || seq>=MAX_DOT_LIGHTS){
        std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        return;
    }
    if(mDotLight[seq])
        std::cout<< "Warning: this light is existed" << std::endl;
    mDotLight[seq] = true;
    std::string baseName = std::string("dotLights[") + std::to_string(seq) + std::string("].");
    mShader->begin();
    mShader->setBool(baseName+"valid", true);
    mShader->setVec3(baseName+"ambient", ambient);
    mShader->setVec3(baseName+"diffuse", diffuse);
    mShader->setVec3(baseName+"specular", specular);
    mShader->setVec3(baseName+"position", position);
    mShader->setFloat(baseName+"constant", constant);
    mShader->setFloat(baseName+"linear", linear);
    mShader->setFloat(baseName+"quadratic", quadratic);
}

void LightControl::addSpotLight(const int& seq, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, const glm::vec3& direction, const float& cutOff, const float& outerCutOff, const float& constant, const float& linear, const float& quadratic)
{
    if(seq<0 || seq>=MAX_SPOT_LIGHTS){
        std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        return;
    }
    if(mSpotLight[seq])
        std::cout<< "Warning: this light is existed and it will be updated;" << std::endl;
    mSpotLight[seq] = true;
    std::string baseName = std::string("spotLights[") + std::to_string(seq) + std::string("].");
    mShader->begin();
    mShader->setBool(baseName+"valid", true);
    mShader->setVec3(baseName+"ambient", ambient);
    mShader->setVec3(baseName+"diffuse", diffuse);
    mShader->setVec3(baseName+"specular", specular);
    mShader->setVec3(baseName+"position", position);
    mShader->setVec3(baseName+"direction", direction);
    mShader->setFloat(baseName+"cutOff", cutOff);
    mShader->setFloat(baseName+"outerCutOff", outerCutOff);
    mShader->setFloat(baseName+"constant", constant);
    mShader->setFloat(baseName+"linear", linear);
    mShader->setFloat(baseName+"quadratic", quadratic);
}

void LightControl::addDirLight(const int& seq, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction)
{
    if(seq<0 || seq>=MAX_DIR_LIGHTS){
        std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        return;
    }
    if(mDirLight[seq])
        std::cout<< "Warning: this light is existed" << std::endl;
    mDirLight[seq] = true;
    std::string baseName = std::string("dirLights[") + std::to_string(seq) + std::string("].");
    mShader->begin();
    mShader->setBool(baseName+"valid", true);
    mShader->setVec3(baseName+"ambient", ambient);
    mShader->setVec3(baseName+"diffuse", diffuse);
    mShader->setVec3(baseName+"specular", specular);
    mShader->setVec3(baseName+"direction", direction);
}
void LightControl::closeLight(const std::string& lightType, const int& seq)
{
    if(lightType == "dir"){
        if(seq>=0 && seq<=MAX_DIR_LIGHTS){
            if(mDirLight[seq]){
                std::string name = std::string("dirLights[") + std::to_string(seq) + std::string("].valid");
                mShader->begin();
                mShader->setBool(name,false);
            }
            else{
                std::cout<< "Warning: this light is not existed" << std::endl;
            }
        }
        else{
            std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        }
    }
    else if(lightType == "dot"){
        if(seq>=0 && seq<=MAX_DOT_LIGHTS){
            if(mDotLight[seq]){
                std::string name = std::string("dotLights[") + std::to_string(seq) + std::string("].valid");
                mShader->begin();
                mShader->setBool(name,false);
            }
            else{
                std::cout<< "Warning: this light is not existed" << std::endl;
            }
        }
        else{
            std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        }
    }
    else if(lightType == "spot"){
        if(seq>=0 && seq<=MAX_SPOT_LIGHTS){
            if(mSpotLight[seq]){
                std::string name = std::string("spotLights[") + std::to_string(seq) + std::string("].valid");
                mShader->begin();
                mShader->setBool(name,false);
            }
            else{
                std::cout<< "Warning: this light is not existed" << std::endl;
            }
        }
        else{
            std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        }
    }
    else{
        std::cout<< "lightType: \"" << lightType << "\" is not right;" << std::endl;
    }
}

void LightControl::openLight(const std::string& lightType, const int& seq)
{
    if(lightType == "dir"){
        if(seq>=0 && seq<=MAX_DIR_LIGHTS){
            std::string name = std::string("dirLights[") + std::to_string(seq) + std::string("].valid");
            mShader->begin();
            mShader->setBool(name,true);
        }
        else{
            std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        }
    }
    else if(lightType == "dot"){
        if(seq>=0 && seq<=MAX_DOT_LIGHTS){
            std::string name = std::string("dotLights[") + std::to_string(seq) + std::string("].valid");
            mShader->begin();
            mShader->setBool(name,true);
        }
        else{
            std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        }
    }
    else if(lightType == "spot"){
        if(seq>=0 && seq<=MAX_SPOT_LIGHTS){
            std::string name = std::string("spotLights[") + std::to_string(seq) + std::string("].valid");
            mShader->begin();
            mShader->setBool(name,true);
        }
        else{
            std::cout<< "seq \""<< seq << "\" out of range;"<< std::endl;
        }
    }
    else{
        std::cout<< "lightType: \"" << lightType << "\" is not right;" << std::endl;
    }
}




void LightControl::showAllLights(void)
{
    std::cout<< "dirLights: ";
    for(int i=0; i<MAX_DIR_LIGHTS; i++)
        std::cout<< mDirLight[i] << ' ';
    std::cout<< std::endl;

    std::cout<< "dotLights: ";
    for(int i=0; i<MAX_DOT_LIGHTS; i++)
        std::cout<< mDotLight[i] << ' ';
    std::cout<< std::endl;

    std::cout<< "spotLights: ";
    for(int i=0; i<MAX_SPOT_LIGHTS; i++)
        std::cout<< mSpotLight[i] << ' ';
    std::cout<< std::endl;
}

void LightControl::setShader(Shader* shader)
{
    mShader = shader;
    mShader->begin();
    for(int i=0; i<MAX_DIR_LIGHTS; i++){
        std::string name = std::string("dirLights[") + std::to_string(i) + std::string("].valid");
        mShader->setBool(name,false);
    }
    for(int i=0; i<MAX_DOT_LIGHTS; i++){
        std::string name = std::string("dotLights[") + std::to_string(i) + std::string("].valid");
        mShader->setBool(name,false);
    }
    for(int i=0; i<MAX_SPOT_LIGHTS; i++){
        std::string name = std::string("spotLights[") + std::to_string(i) + std::string("].valid");
        mShader->setBool(name,false);
    }
}