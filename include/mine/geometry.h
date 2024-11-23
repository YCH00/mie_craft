#pragma once

#include "core.h"

class Geometry {
public:
    Geometry();
    ~Geometry();

    static Geometry* createBox (float size);  //生成一个正方体
    static Geometry* createBox (float size, glm::vec3 pos);  //生成一个正方体
    static Geometry* createCube(float size);  //6个面都用同一种纹理的正方体
    static Geometry* createSphere(float radius);  //生成一个球体

    GLuint getVao() const { return mVao; }
    unsigned int getIndicesCount() const { return mIndicesCount; }
private:
    GLuint mVao{0};
    GLuint mPosVbo{0};
    GLuint mColorVbo {0};
    GLuint mUvVbo{0};
    GLuint mNorVbo{0};
    GLuint mEbo{0};

    unsigned int mIndicesCount { 0 };

};