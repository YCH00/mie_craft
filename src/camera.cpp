#include "mine/camera.h"
#include "mine/worldMap.h"
#include <iostream>

extern WorldMap *myWorldMap;
extern bool escape;

Camera::Camera()  //后序可以加入初始设置参数
{

}

Camera::~Camera()
{

}

glm::mat4 Camera::getViewMatrix()
{
    glm::vec3 front = glm::cross(mUp, mRight);
    glm::vec3 center = mPosition + front;

    return glm::lookAt(mPosition, center, mUp);
}

glm::mat4 Camera::getProjectionMatrix()
{
    return glm::identity<glm::mat4>();
}


PerspectiveCamera::PerspectiveCamera()
{

}

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
{
    mFovy = fovy;
    mAspect = aspect;
    mNear = near;
    mFar = far;
}

PerspectiveCamera::~PerspectiveCamera()
{

}

glm::mat4 PerspectiveCamera::getProjectionMatrix()
{
    return glm::perspective(glm::radians(mFovy), mAspect, mNear, mFar);
}

void PerspectiveCamera::scale(float deltaScale)
{
    //透视投影中的缩放是由相机沿着front向量前后移动实现的
    glm::vec3 front = glm::cross(mUp,mRight);
    mPosition += (front * deltaScale);
}

CameraControl::CameraControl()
{

}

CameraControl::~CameraControl()
{

}

void CameraControl::update(void)
{

}

void CameraControl::onMouse(int button, int action, double xpos, double ypos)
{
    //- 鼠标状态：按下（GLFW_PRESS）、松开（GLFW_RELEASE）;

    //1，判断鼠标按键是否按下
    bool pressed = (action == GLFW_PRESS) ? true : false;

    mCurrentX = xpos;
    mCurrentY = ypos;

    //3，根据按下的鼠标按键，更改按键状态
    //- 按下、抬起都会调用该函数，直接实现状态的复原
    switch(button){
        case GLFW_MOUSE_BUTTON_LEFT:
            mLeftMouseDown = pressed;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mRightMouseDown = pressed;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mMiddleMouseDown = pressed;
            break;
        default:
            break;
    }
}

void CameraControl::onCursor(double xpos, double ypos)
{

}

void CameraControl::onKey(int key, int action, int mods)
{
    //- 键盘状态：按下（GLFW_PRESS）、松开（GLFW_RELEASE）、持续按下（GLFW_REPEAT）

    //过滤掉键盘持续按下的状态
    if(action == GLFW_REPEAT){
        return;
    }

    //1，判断键盘按键是否按下
    bool pressed = (action == GLFW_PRESS) ? true : false;

    //2，记录按下的按键
    mKeyMap[key] = pressed;
}

void GameCameraControl::onCursor(double xpos, double ypos)
{
    // 经纬方向上的角度增量
    double deltaX = (xpos - mCurrentX) * mSensitivity;
    double deltaY = (ypos - mCurrentY) * mSensitivity;

    // pitch和yaw各自计算
    pitch(-deltaY);
    yaw(-deltaX);

    //- 每一帧都会调用一次该函数，因此每次都需要更新鼠标当前位置
    mCurrentX = xpos;
    mCurrentY = ypos;
}

void GameCameraControl::pitch(float angle)
{
    mPitch += angle;
    // 保证总的pitch角度在-90~+90之间
    if(mPitch > 89.0f || mPitch < -89.0f){
        mPitch -= angle;
        return;
    }

    //GameCameraControl中pitch时相机位置不会改变，只会“点头”
    glm::mat4 mat = glm::rotate(glm::mat4(1.0f),glm::radians(angle),mCamera->mRight); 

    mCamera->mUp = mat * glm::vec4(mCamera->mUp,0.0f);
}

void GameCameraControl::yaw(float angle)  //根据pitch、yaw角度，计算相机数据参数变化
{
    glm::mat4 mat = glm::rotate(glm::mat4(1.0f),glm::radians(angle),glm::vec3(0.0f,1.0f,0.0f)); //question:如果是沿着up方向旋转，图片会发生旋转（不知道为啥？）

    mCamera->mUp = mat * glm::vec4(mCamera->mUp,0.0f);
    mCamera->mRight = mat * glm::vec4(mCamera->mRight,0.0f);
}

void GameCameraControl::update()
{
    glm::vec3 direction(0.0f);  //xz平面上的移动方向

    glm::vec3 front = glm::cross(mCamera->mUp,mCamera->mRight);
    glm::vec3 right = mCamera->mRight;
    // 将两者归一化，保证权重相同（不过好像两个已经归一化了）
    front = glm::normalize(front);
    right = glm::normalize(right);

    if(mKeyMap[GLFW_KEY_W]){
        direction += glm::vec3(front.x,0.0f,front.z);
    }
    if(mKeyMap[GLFW_KEY_S]){
        direction -= glm::vec3(front.x,0.0f,front.z);
    }
    if(mKeyMap[GLFW_KEY_A]){
        direction -= glm::vec3(right.x,0.0f,right.z);
    }
    if(mKeyMap[GLFW_KEY_D]){
        direction += glm::vec3(right.x,0.0f,right.z);
    }

    if(mKeyMap[GLFW_KEY_SPACE]){
#ifdef CREATER_MOD
        ySpeed = glm::vec3(0.0f,1.0f,0.0f);
#else
        if(onGround)
            ySpeed = glm::vec3(0.0f, 0.7f, 0.0f), onGround = 0;
#endif
        // std::cout<< "space pressed;" << std::endl;
    }
    if(mKeyMap[GLFW_KEY_LEFT_CONTROL]){
#ifdef CREATER_MOD
        ySpeed = -glm::vec3(0.0f,1.0f,0.0f);
#else
#endif
        // std::cout<< "left ctrl pressed;" << std::endl;
    }
    if(mKeyMap[GLFW_KEY_ESCAPE]){
        escape = true;
    }

    //认为玩家是一个1*1*2的正方体，而且这个正方体是不会旋转的，所以进行碰撞检测的时候可以使用正方体的顶点来判断
    /**
     * update
     * 其实发现逻辑很简单：三个方向依次独立判断即可，每次判断都是那8个点，不知道为什么没有出现预想中的一些棘手情况，但……反正实现了
     */
    glm::vec3 newpos = mCamera->mPosition + ySpeed + glm::vec3(0.0f, -EPS, 0.0f);
    //首先实现一个飞行模式的碰撞检测
    if(myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR)
    {
        newpos.y = mCamera->mPosition.y;
        mCamera->mPosition = newpos;
    } 
    if( myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR)
    {
        while (!(myWorldMap->getCubeAt(transWorldposToMappos(mCamera->mPosition + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT - 1, -PLAYER_RADIU / 2))) != AIR || 
                myWorldMap->getCubeAt(transWorldposToMappos(mCamera->mPosition + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT - 1, PLAYER_RADIU / 2))) != AIR || 
                myWorldMap->getCubeAt(transWorldposToMappos(mCamera->mPosition + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT - 1, -PLAYER_RADIU / 2))) != AIR || 
                myWorldMap->getCubeAt(transWorldposToMappos(mCamera->mPosition + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT - 1, PLAYER_RADIU / 2))) != AIR))
        {
            mCamera->mPosition.y -= 1;
        }
        mCamera->mPosition.y = floor(mCamera->mPosition.y - PLAYER_EYE_HEIGHT) + PLAYER_EYE_HEIGHT + EPS / 2;
        // newpos.y = mCamera->mPosition.y;
#ifndef CREATER_MOD//我累个大草啊试了试还能这么写的吗
        onGround = 1;

    }
    else
    {
        mCamera->mPosition = newpos;
        onGround = 0;
#endif
    }

    //将速度归一
    if(glm::length(direction)!=0){
        direction = glm::normalize(direction);
    }

    glm::vec3 plus = direction * mSpeed;
    //x方向
    newpos = mCamera->mPosition + glm::vec3(plus.x, 0, 0);
    //会发现依然是这八个点，不用变
    if(myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR)
    {
        newpos.x = mCamera->mPosition.x;
    }
    mCamera->mPosition = newpos;

    //z方向
    newpos = mCamera->mPosition + glm::vec3(0, 0, plus.z);
    if(myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, PLAYER_HEIGHT - PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(-PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, -PLAYER_RADIU / 2))) != AIR || 
        myWorldMap->getCubeAt(transWorldposToMappos(newpos + glm::vec3(PLAYER_RADIU / 2, -PLAYER_EYE_HEIGHT, PLAYER_RADIU / 2))) != AIR)
    {
        newpos.z = mCamera->mPosition.z;
    }
    mCamera->mPosition = newpos;

#ifndef CREATER_MOD
    if(onGround)
    {
        ySpeed = glm::vec3(0.0f);
    }
    else
    {
        ySpeed -= glm::vec3(0.0f, g, 0.0f);
        ySpeed.y = std::max(ySpeed.y, -ySpeedmax);
    }
#endif
}