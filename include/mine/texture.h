#pragma once
#include"core.h"
#include<vector>

class Texture {
public:
    Texture(){};
    Texture(const std::string& path, unsigned int unit, bool useMipMap=false);
    ~Texture();

    void bind();

    int getWidth() const { return mWidth; };
    int getHeight() const { return mHeight; };

protected:
    GLuint mTexture{0};
    int mWidth{0};
    int mHeight{0};
    unsigned int mUnit{0};  //当前Texture绑定到哪个纹理单元上

};

class TextureCube : public Texture{
public:
    TextureCube(const std::vector<std::string>& pathes, unsigned int unit, bool useMipMap=false);
    ~TextureCube();
    void bind();

protected:
    GLuint mTexture{0};
};