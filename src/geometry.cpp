#include "mine/geometry.h"
#include "mine/checkError.h"
#include <cmath>
#include <vector>

Geometry::Geometry()
{

}
Geometry::~Geometry()
{
    //分别清理vao,vbo,ebo的buffer
    if(mVao != 0){
        glDeleteBuffers(1, &mVao);
    }
    if(mPosVbo != 0){
        glDeleteBuffers(1, &mPosVbo);
    }
    if(mColorVbo != 0){
        glDeleteBuffers(1, &mColorVbo);
    }
    if(mUvVbo != 0){
        glDeleteBuffers(1, &mUvVbo);
    }
    if(mEbo != 0){
        glDeleteBuffers(1, &mEbo);
    }
}

Geometry* Geometry::createBox (float size)  //生成一个正方体
{
    Geometry* geometry = new Geometry();

    geometry->mIndicesCount = 36;

    float vertices[] = {
        // -z 面
        0, 0, 0,
        size, 0, 0,
        size,  size, 0,
        0,  size, 0,

        // +z 面
        0, 0, size,
        size, 0, size,
        size,  size, size,
        0,  size, size,

        // -y 面
        0, 0, 0,
        size, 0, 0,
        size, 0, size,
        0, 0, size,

        // +y 面
        0, size, 0,
        size, size, 0,
        size, size, size,
        0, size, size,

        // -x 面
        0, 0, 0,
        0, size, 0,
        0, size, size,
        0, 0, size,

        // +x 面
        size, 0, 0,
        size, size, 0,
        size, size, size,
        size, 0, size
    };


    float uvs[] = {
        // -z 面
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // +z 面
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,

        // -y 面
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        // +y 面
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        // -x 面
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        // +x 面
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    float normals[] = {
        // -z 面
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
        // +z 面
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
        // -y
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
        // +y
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
        // -x
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        // +x
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f
    };

    unsigned int indices[] = {
        // -z 面
        0, 1, 2, 2, 3, 0,

        // +z 面
        4, 5, 6, 6, 7, 4,

        // -y 面
        8, 9, 10, 10, 11, 8,

        // +y 面
        12, 13, 14, 14, 15, 12,

        // -x 面
        16, 17, 18, 18, 19, 16,

        // +x 面
        20, 21, 22, 22, 23, 20
    };


    //2，创建VBO
    GLuint& posVbo = geometry->mPosVbo;
    GLuint& uvVbo = geometry->mUvVbo;
    GLuint& norVbo = geometry->mNorVbo;
    glGenBuffers(1,&posVbo);
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(uvs),uvs,GL_STATIC_DRAW);

    glGenBuffers(1,&norVbo);
    glBindBuffer(GL_ARRAY_BUFFER,norVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(normals),normals,GL_STATIC_DRAW);

    //3，创建EBO
    GLuint& ebo = geometry->mEbo;
    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    //4，创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    //5，绑定vbo、ebo，加入属性描述信息
    // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
    // index：vao属性的位置号；  size：该属性包含的数字数；  type：该属性中每个数字的数据类型；
    // normalized：是否需要归一化；  stride：每个顶点数据的步长；  pointer：该属性在每个顶点数据中的偏移量
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);  //可以省略
    glEnableVertexAttribArray(0);  //激活vao属性的0号位置
    GL_CALL(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0));
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glEnableVertexAttribArray(2); //激活vao属性的2号位置
    GL_CALL(glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(0*sizeof(float))));
    glBindBuffer(GL_ARRAY_BUFFER,norVbo);
    glEnableVertexAttribArray(3); //激活vao属性的3号位置
    GL_CALL(glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(0*sizeof(float))));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);  //不能省略，因为必须在绑定vao之后再绑定ebo

    //6，解绑vao
    glBindVertexArray(0);

    return geometry;

}

Geometry* Geometry::createBox (float size, glm::vec3 pos)  //生成一个正方体
{
    Geometry* geometry = new Geometry();
    float halfSize = size / 2.0f;

    geometry->mIndicesCount = 36;

    float vertices[] = {
        // -z 面
        -halfSize+pos.x, -halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, -halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x,  halfSize+pos.y, -halfSize+pos.z,
        -halfSize+pos.x,  halfSize+pos.y, -halfSize+pos.z,

        // +z 面
        -halfSize+pos.x, -halfSize+pos.y, halfSize+pos.z,
        halfSize+pos.x, -halfSize+pos.y, halfSize+pos.z,
        halfSize+pos.x,  halfSize+pos.y, halfSize+pos.z,
        -halfSize+pos.x,  halfSize+pos.y, halfSize+pos.z,

        // -y 面
        -halfSize+pos.x, -halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, -halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, -halfSize+pos.y, halfSize+pos.z,
        -halfSize+pos.x, -halfSize+pos.y, halfSize+pos.z,

        // +y 面
        -halfSize+pos.x, halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, halfSize+pos.y, halfSize+pos.z,
        -halfSize+pos.x, halfSize+pos.y, halfSize+pos.z,

        // -x 面
        -halfSize+pos.x, -halfSize+pos.y, -halfSize+pos.z,
        -halfSize+pos.x, halfSize+pos.y, -halfSize+pos.z,
        -halfSize+pos.x, halfSize+pos.y, halfSize+pos.z,
        -halfSize+pos.x, -halfSize+pos.y, halfSize+pos.z,

        // +x 面
        halfSize+pos.x, -halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, halfSize+pos.y, -halfSize+pos.z,
        halfSize+pos.x, halfSize+pos.y, halfSize+pos.z,
        halfSize+pos.x, -halfSize+pos.y, halfSize+pos.z
    };

    float uvs[] = {
        // -z 面
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // +z 面
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,

        // -y 面
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        // +y 面
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        // -x 面
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        // +x 面
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    float normals[] = {
        // -z 面
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
        // +z 面
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
        // -y
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
        // +y
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
        // -x
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        // +x
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f
    };

    unsigned int indices[] = {
        // -z 面
        0, 1, 2, 2, 3, 0,

        // +z 面
        4, 5, 6, 6, 7, 4,

        // -y 面
        8, 9, 10, 10, 11, 8,

        // +y 面
        12, 13, 14, 14, 15, 12,

        // -x 面
        16, 17, 18, 18, 19, 16,

        // +x 面
        20, 21, 22, 22, 23, 20
    };


    //2，创建VBO
    GLuint& posVbo = geometry->mPosVbo;
    GLuint& uvVbo = geometry->mUvVbo;
    GLuint& norVbo = geometry->mNorVbo;
    glGenBuffers(1,&posVbo);
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(uvs),uvs,GL_STATIC_DRAW);

    glGenBuffers(1,&norVbo);
    glBindBuffer(GL_ARRAY_BUFFER,norVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(normals),normals,GL_STATIC_DRAW);

    //3，创建EBO
    GLuint& ebo = geometry->mEbo;
    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    //4，创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    //5，绑定vbo、ebo，加入属性描述信息
    // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
    // index：vao属性的位置号；  size：该属性包含的数字数；  type：该属性中每个数字的数据类型；
    // normalized：是否需要归一化；  stride：每个顶点数据的步长；  pointer：该属性在每个顶点数据中的偏移量
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);  //可以省略
    glEnableVertexAttribArray(0);  //激活vao属性的0号位置
    GL_CALL(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0));
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glEnableVertexAttribArray(2); //激活vao属性的2号位置
    GL_CALL(glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(0*sizeof(float))));
    glBindBuffer(GL_ARRAY_BUFFER,norVbo);
    glEnableVertexAttribArray(3); //激活vao属性的3号位置
    GL_CALL(glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(0*sizeof(float))));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);  //不能省略，因为必须在绑定vao之后再绑定ebo

    //6，解绑vao
    glBindVertexArray(0);

    return geometry;
}

Geometry* Geometry::createCube(float size)
{
    Geometry* geometry = new Geometry();

    geometry->mIndicesCount = 36;

    float vertices[] = {
        // -z 面
        0, 0, 0,
        size, 0, 0,
        size,  size, 0,
        0,  size, 0,

        // +z 面
        0, 0, size,
        size, 0, size,
        size,  size, size,
        0,  size, size,

        // -y 面
        0, 0, 0,
        size, 0, 0,
        size, 0, size,
        0, 0, size,

        // +y 面
        0, size, 0,
        size, size, 0,
        size, size, size,
        0, size, size,

        // -x 面
        0, 0, 0,
        0, size, 0,
        0, size, size,
        0, 0, size,

        // +x 面
        size, 0, 0,
        size, size, 0,
        size, size, size,
        size, 0, size
    };

    float uvs[] = {
        // -z 面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // +z 面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // -y 面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // +y 面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // -x 面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // +x 面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    float normals[] = {
        // -z 面
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
        // +z 面
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
        // -y
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
        // +y
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
        // -x
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        // +x
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f
    };

    unsigned int indices[] = {
        // -z 面
        0, 1, 2, 2, 3, 0,

        // +z 面
        4, 5, 6, 6, 7, 4,

        // -y 面
        8, 9, 10, 10, 11, 8,

        // +y 面
        12, 13, 14, 14, 15, 12,

        // -x 面
        16, 17, 18, 18, 19, 16,

        // +x 面
        20, 21, 22, 22, 23, 20
    };

    //2，创建VBO
    GLuint& posVbo = geometry->mPosVbo;
    GLuint& uvVbo = geometry->mUvVbo;
    GLuint& norVbo = geometry->mNorVbo;
    glGenBuffers(1,&posVbo);
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(uvs),uvs,GL_STATIC_DRAW);

    glGenBuffers(1,&norVbo);
    glBindBuffer(GL_ARRAY_BUFFER,norVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(normals),normals,GL_STATIC_DRAW);

    //3，创建EBO
    GLuint& ebo = geometry->mEbo;
    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    //4，创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER,posVbo);  //可以省略
    glEnableVertexAttribArray(0);  //激活vao属性的0号位置
    GL_CALL(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0));
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glEnableVertexAttribArray(2); //激活vao属性的2号位置
    GL_CALL(glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)(0*sizeof(float))));
    glBindBuffer(GL_ARRAY_BUFFER,norVbo);
    glEnableVertexAttribArray(3); //激活vao属性的3号位置
    GL_CALL(glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(0*sizeof(float))));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);  //不能省略，因为必须在绑定vao之后再绑定ebo

    //6，解绑vao
    glBindVertexArray(0);

    return geometry;
}

Geometry* Geometry::createSphere(float radius)  //生成一个球体
{
    Geometry* geometry = new Geometry();

    std::vector<GLfloat> positions{};
    std::vector<GLfloat> uvs{};
    std::vector<GLuint> indices{};

    // 我们画一个球体的策略是：对于一根纬线（180°），求其跟所有经线的交点，然后对于所有纬线求
    int numLatLines = 60;  //纬线数量
    int numLongLines = 60;  //经线数量

    for(int i=0; i<=numLatLines; i++){
        for(int j=0; j<=numLongLines; j++){
            float phi = glm::pi<float>()*i / numLatLines;  //经线角
            float theta = 2* glm::pi<float>() *j / numLongLines;  //纬线角

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            float u = 1.0f - (float)j / (float)numLongLines;
            float v = 1.0f - (float)i / (float)numLatLines;  //这里根据最终贴图出来的效果可以进行调整

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);

            uvs.push_back(u);
            uvs.push_back(v);
        }
    }
    
    //question:EBO的确定没有想明白
    for(int i=0; i<numLatLines; i++){
        for(int j=0; j<numLongLines; j++){
            int p1 = i * (numLongLines + 1) + j;
            int p2 = p1 + numLongLines + 1;
            int p3 = p1 + 1;
            int p4 = p2 + 1;

            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p3);

            indices.push_back(p3);
            indices.push_back(p2);
            indices.push_back(p4);
        }
    }

    //2，创建VBO
    GLuint& posVbo = geometry->mPosVbo;
    GLuint& uvVbo = geometry->mUvVbo;
    glGenBuffers(1,&posVbo);
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);
    glBufferData(GL_ARRAY_BUFFER,positions.size()*sizeof(GLfloat),positions.data(),GL_STATIC_DRAW);

    glGenBuffers(1,&uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glBufferData(GL_ARRAY_BUFFER,uvs.size()*sizeof(GLfloat),uvs.data(),GL_STATIC_DRAW);

    //3，创建EBO
    GLuint& ebo = geometry->mEbo;
    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLuint),indices.data(),GL_STATIC_DRAW));

    //4，创建VAO
    GLuint& vao = geometry->mVao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    //5，绑定vbo、ebo，加入属性描述信息
    // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
    // index：vao属性的位置号；  size：该属性包含的数字数；  type：该属性中每个数字的数据类型；
    // normalized：是否需要归一化；  stride：每个顶点数据的步长；  pointer：该属性在每个顶点数据中的偏移量
    glBindBuffer(GL_ARRAY_BUFFER,posVbo);  //可以省略
    glEnableVertexAttribArray(0);  //激活vao属性的0号位置
    GL_CALL(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0));
    glBindBuffer(GL_ARRAY_BUFFER,uvVbo);
    glEnableVertexAttribArray(2); //激活vao属性的2号位置
    GL_CALL(glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)(0*sizeof(float))));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);  //不能省略，因为必须在绑定vao之后再绑定ebo

    //6，解绑vao
    glBindVertexArray(0);

    geometry->mIndicesCount = indices.size();

    return geometry;
}
