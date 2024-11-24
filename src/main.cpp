#include<iostream>
#include "mine/core.h"
#include<string>
#include"mine/checkError.h"
#include"mine/application.h"
#include"mine/shader.h"
#include"mine/texture.h"
#include"mine/camera.h"
#include"mine/geometry.h"
#include"mine/worldMap.h"
#include"mine/lightControl.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

extern double FPS = 30;

Shader* lightingShader = nullptr;
Shader* skyboxShader = nullptr;

Texture* skyboxTexture = nullptr;
Texture* boxTextures[NULLCUBE] = {nullptr};

glm::mat4 transform(1.0);

PerspectiveCamera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;
extern WorldMap* myWorldMap = nullptr;
extern bool escape = false;

Geometry* skyboxVao = nullptr;
Geometry* boxVao = nullptr;

LightControl* lightControl = nullptr;

//���ڴ�С�ı���Ӧ����
void OnResize(int width, int height)
{
    GL_CALL(glViewport(0,0,width,height));  // �ı��ӿڵĴ�С
    std::cout << "�������´�С��" << width << "�� " << height << std::endl;
}

//������Ϣ��Ӧ����
void OnKey(int key, int action, int mods)
{
    cameraControl->onKey(key,action,mods);
}

//��갴��/̧����Ӧ����
void OnMouse(int button, int action, int mods)
{
    double x, y;
    Application::getInstance()->getCursorPosition(&x,&y);
    cameraControl->onMouse(button,action,x,y);
}

//����ƶ���Ӧ����
void OnCursor(double xpos, double ypos)
{
    cameraControl->onCursor(xpos,ypos);
}

//��������Ӧ����
void OnScroll(double offset)
{
    cameraControl->onScroll(offset);
}

void prepareVaoUsingEbo(void)
{
    skyboxVao = Geometry::createBox(10.0f,glm::vec3(0.0f,0.0f,0.0f));
    boxVao = Geometry::createBox(1.0f);

}

void prepareShader(void)
{
    lightingShader = new Shader("assets/shaders/10multiLightVs.glsl", "assets/shaders/10multiLightFs.glsl");
    skyboxShader = new Shader("assets/shaders/03skyboxVs.glsl", "assets/shaders/03skyboxFs.glsl");

    lightingShader->begin();
    lightControl->setShader(lightingShader);
    lightControl->addDirLight(0,glm::vec3(0.05f, 0.05f, 0.05f),glm::vec3(0.4f, 0.4f, 0.4f),glm::vec3(0.5f, 0.5f, 0.5f),glm::vec3(0.0f,-1.0f, 0.0f));
    lightControl->addDotLight(0,glm::vec3(0.05f, 0.05f, 0.05f),glm::vec3(0.8f, 0.8f, 0.8f),glm::vec3(1.0f, 1.0f, 1.0f),glm::vec3(10.0f,10.0f,10.0f));
    lightControl->addSpotLight(0,glm::vec3(0.1f, 0.1f, 0.1f),glm::vec3(0.8f, 0.8f, 0.8f),glm::vec3(1.0f, 1.0f, 1.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,-1.0f,0.0f));
    lightingShader->setVec3("material.specular",glm::vec3(0.99f, 0.99f, 0.99f));
    lightingShader->setFloat("material.shininess",512.0f);
    lightControl->addDirLight(1,glm::vec3(0.5f,0.5f,0.5f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f));
    lightControl->closeLight("dir",1);
    skyboxShader->begin();
    skyboxShader->setInt("skybox", 0);
}

const std::string boxTextureBasePathes[] = {
    "",
    "assets/textures/cubes/grass/",
    "assets/textures/cubes/stone/",
    "assets/textures/cubes/dirt/",
    "assets/textures/cubes/snow/",
    "assets/textures/cubes/sand/",
    "assets/textures/cubes/wood/",
    ""
};

void prepareTexture(void)
{
    // ��պ�����
    std::vector<std::string> skyboxPathes{
        "./assets/textures/skybox/right.jpg",
        "./assets/textures/skybox/left.jpg",
        "./assets/textures/skybox/top.jpg",
        "./assets/textures/skybox/bottom.jpg",
        "./assets/textures/skybox/front.jpg",
        "./assets/textures/skybox/back.jpg"
    };
    skyboxTexture = new TextureCube(skyboxPathes,0);

    // ���ַ�����ʵ�texture
    for(int i=1; i<(int)NULLCUBE; i++){
        if(boxTextureBasePathes[i] != ""){
            std::vector<std::string> boxTexturePathes{
                (boxTextureBasePathes[i] + "side.png"),
                (boxTextureBasePathes[i] + "side.png"),
                (boxTextureBasePathes[i] + "top.png"),
                (boxTextureBasePathes[i] + "bottom.png"),
                (boxTextureBasePathes[i] + "side.png"),
                (boxTextureBasePathes[i] + "side.png")
            };
            boxTextures[i] = new TextureCube(boxTexturePathes,(unsigned int)i);
        }
        std::cout<< "texture loaded correctly: " << boxTextureBasePathes[i] << "; at: " << i<< std::endl;
    }
}

/* ����� */ 
void prepareCamera(void)
{
    camera = new PerspectiveCamera(60.0f,(float)Application::getInstance()->getWidth()/(float)Application::getInstance()->getHeight(),0.1f,100.0f);
    cameraControl = new GameCameraControl();
    cameraControl->setCamera(camera);
    cameraControl->setSensitivity(0.4f);
}


void prepareState(void)
{
    glEnable(GL_DEPTH_TEST);  //������ȼ�⹦�ܣ�ͬʱ����������ʱ����Ҫ������Ȼ��� glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);  //�����ڵ���ʽ�������ڵ�Զ���ģ���ʵ���кܶ���
    // glClearDepth(1.0f);  //����������Ȼ������ݵ�����
}

void prepareWorldMap(void)
{
    myWorldMap = new WorldMap();
    myWorldMap->update(myWorldMap->calculateBlockPos(transWorldposToMappos(camera->getViewPos())));
}

void prepareLightControl()
{
    lightControl = new LightControl;
}

// ����ʮ��׼��
void drawCrosshair(int width, int height) {
    // ��������
    float vertices[] = {
        // λ��          ��ɫ
        -0.014f,  0.00f,  1.0f, 1.0f, 1.0f, // ��˵�
         0.01f,  0.00f,  1.0f, 1.0f, 1.0f, // �Ҷ˵�
         0.00f, -0.012f,  1.0f, 1.0f, 1.0f, // �϶˵�
         0.00f, +0.01f,  1.0f, 1.0f, 1.0f, // �¶˵�
    };

    // �����������
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // ���㻺�����
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // λ������
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ��ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ������ɫ��
    Shader centerShader("assets/shaders/13center.vs", "assets/shaders/13center.fs");

    // ʹ����ɫ������
    centerShader.begin();

    // ����ʮ��׼��
    glDrawArrays(GL_LINES, 0, 4);

    // ����
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

bool left_mouse_down = false;
bool right_mouse_down = false;

//��Ⱦ
void render(void)
{
    //0��������
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));  //���������Ҫ������ȼ�⣬����ͬʱ������Ȼ�������

    //������պ�
    //1���󶨵�ǰprogram
    skyboxShader->begin();
    skyboxTexture->bind();
    skyboxShader->setMartrix4x4("viewMatrix",glm::mat4(glm::mat3(camera->getViewMatrix())));  //���ָо���Ϊ��ʵһЩ������е�������Ը���һ���ƶ������ǻ�����ƶ�����պе����
    skyboxShader->setMartrix4x4("projectionMatrix",camera->getProjectionMatrix());

    //2���󶨵�ǰvao
    GL_CALL(glBindVertexArray(skyboxVao->getVao()));

    //3����������ָ��
    glDrawElements(GL_TRIANGLES,skyboxVao->getIndicesCount(),GL_UNSIGNED_INT,(void*)0);

    // ������������
    drawCrosshair(SCR_WIDTH, SCR_HEIGHT);

    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader->begin();
    lightingShader->setVec3("spotLights[0].position", camera->getViewPos());
    lightingShader->setVec3("spotLights[0].direction", camera->getFront());// direction�ǹ����䷽��
    lightingShader->setMartrix4x4("transform",transform);
    lightingShader->setMartrix4x4("viewMatrix",camera->getViewMatrix());
    lightingShader->setMartrix4x4("projectionMatrix",camera->getProjectionMatrix());
    lightingShader->setVec3("viewPos",camera->getViewPos());
    GL_CALL(glBindVertexArray(boxVao->getVao()));


    // render containers
    glBindVertexArray(boxVao->getVao());

    //���㵱ǰλ�����飬����worldmap
    //ע������yz�Ƿ��ŵ�
    Pos nowpos;
    nowpos.x = camera->getViewPos().x;
    nowpos.y = camera->getViewPos().z;
    nowpos.z = camera->getViewPos().y;
    Pos blockpos = myWorldMap->calculateBlockPos(nowpos);
    myWorldMap->update(blockpos);
    CUBE tmp_cube = AIR;

    for (auto &[blockPos, block] : myWorldMap->wMap){
        if(blockPos.z != 0)
            continue;
        for (int k = 0; k < H; k++) {
        for (int i = 0; i < W; i++) {
        for (int j = 0; j < L; j++) {

            if(block->cubearray[i][j][k] != AIR && block->cubearray[i][j][k] < NULLCUBE){
                bool z1 = ((j == 0     || block->cubearray[i][j - 1][k] == AIR) && camera->getViewPos().z <= blockPos.y * L + j);
                bool z2 = ((j == L - 1 || block->cubearray[i][j + 1][k] == AIR) && camera->getViewPos().z >= blockPos.y * L + j +1);
                bool x1 = ((i == 0     || block->cubearray[i - 1][j][k] == AIR) && camera->getViewPos().x <= blockPos.x * W + i);
                bool x2 = ((i == W - 1 || block->cubearray[i + 1][j][k] == AIR) && camera->getViewPos().x >= blockPos.x * W + i +1);
                bool y1 = ((k != 0     && block->cubearray[i][j][k - 1] == AIR) && camera->getViewPos().y <= blockPos.z * H + k); //������΢����һ�£��ص��ǿ��Բ���Ⱦ��
                bool y2 = ((k == H - 1 || block->cubearray[i][j][k + 1] == AIR) && camera->getViewPos().y >= blockPos.z * H + k +1);
                if(!(z1 || z2 || x1 || x2 || y1 || y2))
                    continue;
                // model = glm::translate(model, glm::vec3(blockPos.x * W + i, blockPos.z * H + k, blockPos.y * L + j));
                // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i + (blockPos.x * W), k + (blockPos.z * H), j + (blockPos.y * L)));
                lightingShader->setMat4("transform", block->model[i][j][k]);

                boxTextures[block->cubearray[i][j][k]]->bind();
                if(tmp_cube != block->cubearray[i][j][k]){
                    tmp_cube = block->cubearray[i][j][k];
                    lightingShader->setInt("material.cubeTexture",(int)(block->cubearray[i][j][k]));
                }
                // std::cout<< (int)(block->cubearray[i][j][k]) <<std::endl;
                //ÿ�������6���沢����Ҫȫ������
                //ԭ�����ж�������Ƿ����������ڵ���û�ڵ��Ļ��ٻ���
                if (z1) // -z  
                    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)(0*sizeof(unsigned int)));
                if (z2)  // +z  
                    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)(6*sizeof(unsigned int)));
                if (x1)  // -x  
                    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)(24*sizeof(unsigned int)));
                if (x2)  // +x  
                    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)(30*sizeof(unsigned int)));
                if (y1)  // -y  
                    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)(12*sizeof(unsigned int)));
                if (y2)  // +y  
                    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)(18*sizeof(unsigned int)));
            }
            
        }}}
    }
    auto [pos1, pos2] = myWorldMap->getPointingCube(camera->getViewPos(), camera->getFront(), 10.0);
    // auto pos_block = myWorldMap->calculateBlockPos(pos1);
    // Pos pos_inblock = {pos1.x - pos_block.x * W, pos1.y - pos_block.y * L, pos1.z - pos_block.z * H};
    // auto [i, j, k] = pos_inblock;
    // myWorldMap->wMap[pos_block]->cubearray[i][j][k];
    if(myWorldMap->getCubeAt(pos1) != AIR){
        lightControl->openLight("dir",1);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pos1.x, pos1.z, pos1.y));
        lightingShader->setMat4("transform", model);
        boxTextures[myWorldMap->getCubeAt(pos1)]->bind();
        lightingShader->setInt("material.cubeTexture",(int)(myWorldMap->getCubeAt(pos1)));  
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,(void*)(0*sizeof(unsigned int)));
        lightControl->closeLight("dir",1);
    }

    // ��������ú�����
    if(cameraControl->getLeftMouseDown()){
        if(!left_mouse_down)
        {
            left_mouse_down = true;
            myWorldMap->getCubeAt(pos1) = AIR;
            // std::cout<< "left mouse down"<<std::endl;
        }
    }
    else{
        // std::cout<< "left mouse up" <<std::endl;
        left_mouse_down = false;
    }
    if(cameraControl->getRightMouseDown()){
        if(!right_mouse_down)
        {
            right_mouse_down = true;
            if(myWorldMap->getCubeAt(pos1) != AIR)
            {
                myWorldMap->getCubeAt(pos2) = STONE;
                //ֱ�ӱ����ж�һ�µ�ǰ�Ƿ�����ײ����ģ��������ǵĻ��Ͳ���
                if(myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
                    myWorldMap->getCubeAt(transWorldposToMappos(camera->getViewPos() + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR)
                {
                    myWorldMap->getCubeAt(pos2) = AIR;
                }
            }
        }
    }
    else{
        right_mouse_down = false;
    }
}

int main()
{
    //1����ʼ��GLFW�Ļ�������
    if(!Application::getInstance() -> init(SCR_WIDTH,SCR_HEIGHT,"learn_opengl")){
        return -1;
    }

    //2����������ѭ��
    Application::getInstance()->setResizeCallback(OnResize);
    Application::getInstance()->setKeyBoardCallback(OnKey);  //������Ӧ����
    Application::getInstance()->setMouseCallback(OnMouse);
    Application::getInstance()->setCursorCallback(OnCursor);
    Application::getInstance()->setScrollCallback(OnScroll);    //����opengl�ӿ��Լ�������ɫ

    // tell GLFW to capture our mouse
    glfwSetInputMode(Application::getInstance()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0,0,800,600);  //�����ӿڣ�ǰ��������Ϊ�����½ǿ�ʼ�����ĺ������꣬����������Ϊwidth��height
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //r,g,b,alpha

    prepareLightControl();
    prepareVaoUsingEbo();
    prepareTexture();
    prepareShader();
    prepareCamera();
    prepareState();
    prepareWorldMap();

     // ֡��ͳ�Ʊ���
    double previousTime = glfwGetTime();
    int frameCount = 0;

    //3��ִ�д���ѭ��
    while(Application::getInstance() -> update()){

        // ��ȡ��ǰʱ��
        double currentTime = glfwGetTime();
        frameCount++;
        // ÿ�����һ��֡��
        if (currentTime - previousTime >= 1.0) {
            FPS = frameCount / (currentTime - previousTime);
            // std::cout << "FPS: " << fps << std::endl;
            // ���´��ڱ���
            std::string title = "OpenGL FPS: " + std::to_string(FPS);
            glfwSetWindowTitle(Application::getInstance()->getWindow(), title.c_str());
            // ���ü�����
            previousTime = currentTime;
            frameCount = 0;
        }

        cameraControl->update();
        //��Ⱦ����
        render();
        if(escape)
            break;
    }

    //4���˳�����ǰ���������
    delete myWorldMap;
    Application::getInstance() -> destory();

    return 0;
}

