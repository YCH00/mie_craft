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
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

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


//窗口大小改变响应函数
void OnResize(int width, int height)
{
    GL_CALL(glViewport(0,0,width,height));  // 改变视口的大小
    std::cout << "窗口最新大小：" << width << "， " << height << std::endl;
}

//键盘消息响应函数
void OnKey(int key, int action, int mods)
{
    cameraControl->onKey(key,action,mods);
}

//鼠标按下/抬起响应函数
void OnMouse(int button, int action, int mods)
{
    double x, y;
    Application::getInstance()->getCursorPosition(&x,&y);
    cameraControl->onMouse(button,action,x,y);
}

//鼠标移动响应函数
void OnCursor(double xpos, double ypos)
{
    cameraControl->onCursor(xpos,ypos);
}

//鼠标滚轮响应函数
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
    "assets/textures/cubes/glowstone/",
    ""
};

void prepareTexture(void)
{
    // 天空盒纹理
    std::vector<std::string> skyboxPathes{
        "./assets/textures/skybox/right.jpg",
        "./assets/textures/skybox/left.jpg",
        "./assets/textures/skybox/top.jpg",
        "./assets/textures/skybox/bottom.jpg",
        "./assets/textures/skybox/front.jpg",
        "./assets/textures/skybox/back.jpg"
    };
    skyboxTexture = new TextureCube(skyboxPathes,0);

    // 各种方块材质的texture
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

/* 摄像机 */ 
const float elev_y = 60.0f;
void prepareCamera(void)
{
    camera = new PerspectiveCamera(60.0f,(float)Application::getInstance()->getWidth()/(float)Application::getInstance()->getHeight(),0.1f,100.0f);
    cameraControl = new GameCameraControl();
    cameraControl->setCamera(camera);
    cameraControl->setSensitivity(0.4f);
}


void prepareState(void)
{
    glEnable(GL_DEPTH_TEST);  //开启深度检测功能，同时在清理画布的时候需要清理深度缓存 glClear(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);  //设置遮挡方式，近处遮挡远处的，其实还有很多种
    // glClearDepth(1.0f);  //设置清理深度缓存数据的数据
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

// 绘制十字准心
void drawCrosshair(int width, int height) {
    // 顶点数据
    float vertices[] = {
        // 位置          颜色
        -0.014f,  0.00f,  1.0f, 1.0f, 1.0f, // 左端点
         0.01f,  0.00f,  1.0f, 1.0f, 1.0f, // 右端点
         0.00f, -0.012f,  1.0f, 1.0f, 1.0f, // 上端点
         0.00f, +0.01f,  1.0f, 1.0f, 1.0f, // 下端点
    };

    // 顶点数组对象
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 顶点缓冲对象
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 编译着色器
    Shader centerShader("assets/shaders/13center.vs", "assets/shaders/13center.fs");

    // 使用着色器程序
    centerShader.begin();

    // 绘制十字准心
    glDrawArrays(GL_LINES, 0, 4);

    // 清理
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

bool left_mouse_down = false;
bool right_mouse_down = false;

//渲染
void render(void)
{
    //0，清理画布
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));  //如果我们想要加入深度检测，必须同时清理深度缓存数据

    //绘制天空盒
    //1，绑定当前program
    skyboxShader->begin();
    skyboxTexture->bind();
    skyboxShader->setMartrix4x4("viewMatrix",glm::mat4(glm::mat3(camera->getViewMatrix())));  //这种感觉更为真实一些，天空中的物体可以跟我一起移动，但是会出现移动出天空盒的情况
    skyboxShader->setMartrix4x4("projectionMatrix",camera->getProjectionMatrix());

    //2，绑定当前vao
    GL_CALL(glBindVertexArray(skyboxVao->getVao()));

    //3，发出绘制指令
    glDrawElements(GL_TRIANGLES,skyboxVao->getIndicesCount(),GL_UNSIGNED_INT,(void*)0);

    // 绘制其他方块
    drawCrosshair(SCR_WIDTH, SCR_HEIGHT);

    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader->begin();
    lightingShader->setVec3("spotLights[0].position", camera->getViewPos());
    lightingShader->setVec3("spotLights[0].direction", camera->getFront());// direction是光照射方向
    lightingShader->setMartrix4x4("transform",transform);
    lightingShader->setMartrix4x4("viewMatrix",camera->getViewMatrix());
    lightingShader->setMartrix4x4("projectionMatrix",camera->getProjectionMatrix());
    lightingShader->setVec3("viewPos",camera->getViewPos());
    lightingShader->setBool("is_lightcube",false);
    GL_CALL(glBindVertexArray(boxVao->getVao()));


    // render containers
    glBindVertexArray(boxVao->getVao());

    //计算当前位置区块，传给worldmap
    //注意这里yz是反着的
    Pos nowpos;
    nowpos.x = camera->getViewPos().x;
    nowpos.y = camera->getViewPos().z;
    nowpos.z = camera->getViewPos().y;
    Pos blockpos = myWorldMap->calculateBlockPos(nowpos);
    myWorldMap->update(blockpos);
    CUBE tmp_cube = AIR;
    
    const double elev = std::atan(std::hypot(SCR_HEIGHT, SCR_WIDTH) / SCR_HEIGHT * std::tan(glm::radians(elev_y / 2)));

    int lightcube_num=0;

    for (auto &[blockPos, block] : myWorldMap->wMap){
        if(blockPos.z != 0)
            continue;
        
        for (int k = 0; k < H; k++) {
        for (int i = 0; i < W; i++) {
        for (int j = 0; j < L; j++) {
            if(block->cubearray[i][j][k] != AIR && block->cubearray[i][j][k] >= GLOWSTONE && block->cubearray[i][j][k] < NULLCUBE){
                lightControl->addDotLight(lightcube_num,glm::vec3(0.2f, 0.2f, 0.2f),glm::vec3(0.5f, 0.5f, 0.5f),glm::vec3(1.0f, 1.0f, 1.0f),glm::vec3(i + (blockPos.x * W), k + (blockPos.z * H), j + (blockPos.y * L)));  
                lightcube_num++;
            }
        }}}

        for (int k = 0; k < H; k++) {
        for (int i = 0; i < W; i++) {
        for (int j = 0; j < L; j++) {

            if(block->cubearray[i][j][k] != AIR && block->cubearray[i][j][k] < NULLCUBE){
                bool z1 = ((j == 0     || block->cubearray[i][j - 1][k] == 0) && camera->getViewPos().z <= blockPos.y * L + j);
                bool z2 = ((j == L - 1 || block->cubearray[i][j + 1][k] == 0) && camera->getViewPos().z >= blockPos.y * L + j +1);
                bool x1 = ((i == 0     || block->cubearray[i - 1][j][k] == 0) && camera->getViewPos().x <= blockPos.x * W + i);
                bool x2 = ((i == W - 1 || block->cubearray[i + 1][j][k] == 0) && camera->getViewPos().x >= blockPos.x * W + i +1);
                bool y1 = ((k == 0     || block->cubearray[i][j][k - 1] == 0) && camera->getViewPos().y <= blockPos.z * H + k);
                bool y2 = ((k == H - 1 || block->cubearray[i][j][k + 1] == 0) && camera->getViewPos().y >= blockPos.z * H + k +1);
                if(!(z1 || z2 || x1 || x2 || y1 || y2))
                    continue;
                // model = glm::translate(model, glm::vec3(blockPos.x * W + i, blockPos.z * H + k, blockPos.y * L + j));
                //如果在视野之外就不绘制
                //具体来说，近距离全部绘制，远距离再判断
                const double BDIS = 10;
                const double ELEVEPS = 0.1;
                glm::vec3 cubePos(i + (blockPos.x * W) + 0.5, k + (blockPos.z * H) + 0.5, j + (blockPos.y * L) + 0.5);
                if(glm::distance(camera->getViewPos(), cubePos) > BDIS)
                {
                    glm::vec3 t = cubePos - camera->getViewPos();
                    if(std::acos(glm::dot(t, camera->getFront()) / glm::length(t) / glm::length(camera->getFront())) > elev + ELEVEPS)
                    {
                        continue;
                    }
                }
                // model = glm::translate(glm::mat4(1.0f), glm::vec3(i + (blockPos.x * W), k + (blockPos.z * H), j + (blockPos.y * L)));
                lightingShader->setMat4("transform", block->model[i][j][k]);

                if(block->cubearray[i][j][k] < GLOWSTONE)
                    lightingShader->setBool("is_lightcube",false);
                else
                    lightingShader->setBool("is_lightcube",true);      
                    
                boxTextures[block->cubearray[i][j][k]]->bind();
                if(tmp_cube != block->cubearray[i][j][k]){
                    tmp_cube = block->cubearray[i][j][k];
                    lightingShader->setInt("material.cubeTexture",(int)(block->cubearray[i][j][k]));
                }
                // std::cout<< (int)(block->cubearray[i][j][k]) <<std::endl;
                //每个方块的6个面并不需要全部绘制
                //原理是判断这个面是否被其他方块遮挡，没遮挡的话再绘制
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
        
        for(int i=0;i<lightcube_num;i++)
            lightControl->closeLight("dot",i);
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

    // 处理方块放置和消除
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
                myWorldMap->getCubeAt(pos2) = cameraControl->getOnhandCube();
                //直接暴力判断一下当前是否发生碰撞（穿模），如果是的话就不放
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
    //1，初始化GLFW的基本环境
    if(!Application::getInstance() -> init(SCR_WIDTH,SCR_HEIGHT,"learn_opengl")){
        return -1;
    }

    //2，创建窗体循环
    Application::getInstance()->setResizeCallback(OnResize);
    Application::getInstance()->setKeyBoardCallback(OnKey);  //设置响应函数
    Application::getInstance()->setMouseCallback(OnMouse);
    Application::getInstance()->setCursorCallback(OnCursor);
    Application::getInstance()->setScrollCallback(OnScroll);    //设置opengl视口以及清理颜色

    // tell GLFW to capture our mouse
    glfwSetInputMode(Application::getInstance()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);  //设置视口，前两个参数为从左下角开始计数的横纵坐标，后两个参数为width和height
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //r,g,b,alpha

    prepareLightControl();
    prepareVaoUsingEbo();
    prepareTexture();
    prepareShader();
    prepareCamera();
    prepareState();
    prepareWorldMap();

     // 帧率统计变量
    double previousTime = glfwGetTime();
    int frameCount = 0;

    //3，执行窗体循环
    while(Application::getInstance() -> update()){

        // 获取当前时间
        double currentTime = glfwGetTime();
        frameCount++;
        // 每秒更新一次帧率
        if (currentTime - previousTime >= 1.0) {
            double fps = frameCount / (currentTime - previousTime);
            // std::cout << "FPS: " << fps << std::endl;
            // 更新窗口标题
            std::string title = "OpenGL FPS: " + std::to_string(fps);
            glfwSetWindowTitle(Application::getInstance()->getWindow(), title.c_str());
            // 重置计数器
            previousTime = currentTime;
            frameCount = 0;
        }

        cameraControl->update();
        //渲染操作
        render();
        if(escape)
            break;
    }

    //4，退出程序前做相关清理
    delete myWorldMap;
    Application::getInstance() -> destory();

    return 0;
}

