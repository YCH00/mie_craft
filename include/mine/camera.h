#pragma once

#include"core.h"
#include "worldMap.h"

// #define CREATER_MOD

const double PLAYER_HEIGHT = 1.9;
const double PLAYER_RADIU = 0.4;
const double PLAYER_EYE_HEIGHT = 1.5;
const double EPS = 0.05;

class Camera {
public:
    Camera();
    ~Camera();

    glm::mat4 getViewMatrix();  //返回视图变换矩阵
    virtual glm::mat4 getProjectionMatrix();  //返回投影矩阵
    virtual void scale(float deltaScale) {}  //缩放功能的实现
    glm::vec3 getViewPos() { return mPosition; }
    glm::vec3 getFront() { return glm::cross(mUp,mRight); }

public:
    glm::vec3 mPosition {0.0f,20.0f,0.0f};  //摄像机的当前位置
    glm::vec3 mUp {0.0f,1.0f,0.0f};  //摄像机头朝向的方向  //可以代替穹顶方向传入glm::lookat()函数
    glm::vec3 mRight {1.0f,0.0f,0.0f};  //摄像机正右侧的方向
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera();
    PerspectiveCamera(float fovy, float aspect, float near, float far);
    ~PerspectiveCamera();

    glm::mat4 getProjectionMatrix() override;  //返回投影矩阵（覆写）
    void scale(float deltaScale) override;  //缩放功能的实现
private:
    float mFovy = 0.0f;  //y方向市张角（角度）
    float mAspect = 0.0f;  //近平面的横/纵比
    float mNear = 0.0f;
    float mFar = 0.0f;  // 可视空间参数
};


class CameraControl {
public:
    CameraControl();
    ~CameraControl();
    virtual void update(void);  //需要每一帧更新的行为
    virtual void onMouse(int button, int action, double xpos, double ypos);
    virtual void onCursor(double xpos, double ypos);
    virtual void onKey(int key, int action, int mods);
    virtual void onScroll(float offset) {}  //滚轮事件响应函数  offset: +1,-1

    void setCamera(Camera* camera) { mCamera = camera; }
    void setSensitivity(float s) { mSensitivity = s; }

    bool getLeftMouseDown() const { return mLeftMouseDown; }
    bool getRightMouseDown() const { return mRightMouseDown; }
    bool getMiddleMouseDown() const { return mMiddleMouseDown; }
    CUBE getOnhandCube() const { return onhand; }

protected:
    bool mLeftMouseDown = false;
    bool mRightMouseDown = false;
    bool mMiddleMouseDown = false;  //分别表示鼠标左键、右键、中键是否被按下
    double mCurrentX = 0.0f, mCurrentY = 0.0f;  //当前鼠标的位置坐标
    std::map<int,bool> mKeyMap;  //记录每一个键盘按键是否被按下
    float mSensitivity = 0.1f;  //灵敏度，鼠标移动距离=>旋转角 的比值
    Camera* mCamera = nullptr;  //控制的摄像机
    CUBE onhand = STONE; //玩家手中的方块

    bool onGround = 0;
    const float g = 22; //重力加速度
    float ySpeed = 0.0f;//玩家y方向的速度
    const float ySpeedmax = 25;
};

class GameCameraControl : public CameraControl {
public:
    GameCameraControl() {}
    ~GameCameraControl() {}

    void onCursor(double xpos, double ypos) override;
    void update() override;

    void setSpeed(float v) { mSpeed = v; }

private:
    void pitch(float angle);
    void yaw(float angle);  //根据pitch、yaw角度，计算相机数据参数变化

private:
    float mPitch {0.0f};  //记录pitch的角度，保证总的pitch角度在-90~+90之间
    float mSpeed {3.0f};  //相机运动的速度
};



