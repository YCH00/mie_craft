#include"mine/texture.h"
#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#include"image/stb_image.h"

/*
todo:
    1，没有考虑图片读取失败的情况

*/


Texture::Texture(const std::string& path, unsigned int unit, bool useMipMap)
{
    mUnit = unit;

    int channels;
    stbi_set_flip_vertically_on_load(true);  //反转y轴
    unsigned char* data = stbi_load(path.c_str(),&mWidth,&mHeight,&channels,STBI_rgb_alpha); //读取出rgba的格式

    glGenTextures(1,&mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);  //激活纹理单元
    glBindTexture(GL_TEXTURE_2D,mTexture);  //绑定纹理单元和纹理对象

    if(!useMipMap){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
    }
    else{
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);  // 需要像素 > 图片像素 => linear过滤
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST); // 需要像素 < 图片像素 => nearest过滤

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);  //u方向
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);  //v方向

    std::cout<< "texture " << mUnit << "# is loaded"<< std::endl;
}

Texture::~Texture()
{
    if(mTexture != 0){
        glDeleteTextures(1,&mTexture);
    }
}

void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0 + mUnit);  //切换纹理单元
    glBindTexture(GL_TEXTURE_2D,mTexture);  //绑定纹理单元和纹理对象
}



TextureCube::TextureCube(const std::vector<std::string>& pathes, unsigned int unit, bool useMipMap)
{
    mUnit = unit;

    glGenTextures(1,&mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);  //激活纹理单元
    glBindTexture(GL_TEXTURE_CUBE_MAP,mTexture);  //绑定纹理单元和纹理对象

    if(pathes.size()!=6){
        std::cout<<"Warning: The number of textures is not 6;"<<std::endl;
        // 给的纹理图片没有6张，默认使用第一张代替所有
        int channels;
        unsigned char* data = stbi_load(pathes[0].c_str(),&mWidth,&mHeight,&channels,STBI_rgb_alpha); //读取出rgba的格式
        for(int i=0;i<6;i++){
            if(!useMipMap){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGBA,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
            }
            else{
                // 使用内置工具实现：
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGBA,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            }
        }
        stbi_image_free(data);
    }
    else{
        for(int i=0;i<6;i++){
            int channels;
            unsigned char* data = stbi_load(pathes[i].c_str(),&mWidth,&mHeight,&channels,STBI_rgb_alpha); //读取出rgba的格式
            if (data) {

                    
                if(!useMipMap){
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGBA,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
                }
                else{
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGBA,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
                    glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
                }
                std::cout<< "    i. " << pathes[i] << " loaded successfully" << std::endl;
            }
            else {
                std::cout << "Cubemap texture failed to load at path:" << pathes[i] << std::endl;
            }
            stbi_image_free(data);
        }
        std::cout<<"Textures *6 is loaded correctly" <<std::endl;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout<< "texture " << mUnit << "# is loaded"<< std::endl;
}


void TextureCube::bind()
{
    glActiveTexture(GL_TEXTURE0 + mUnit);  //切换纹理单元
    glBindTexture(GL_TEXTURE_CUBE_MAP,mTexture);  //绑定纹理单元和纹理对象
}
