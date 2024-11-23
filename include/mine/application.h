#pragma once
#include<iostream>

class GLFWwindow; //因为没有加GLFW的头文件（会导致头文件重复包含），所以声明一下 

// #define app Application::getInstance()


using ResizeCallback = void(*) (int width, int height); 
using KeyBoardCallback = void(*) (int key, int action, int mods);
using MouseCallback = void(*) (int button, int action, int mods);
using CursorCallback = void(*) (double xpos, double ypos);
using ScrollCallback = void(*) (double offset);  //根据main函数中的响应函数来的类型
//using用于指定别名，等价于typedef void(*) (int width, int height) ResizeCallback;

//单例类，全局唯一实例
class Application{
public:
    ~Application();

    //用于访问实例的静态函数
    static Application* getInstance();

    //三个成员函数，分别掌管窗口对象的 初始化，帧更新，销毁
    bool init(const unsigned int& width = 800, const unsigned int& height = 600, const char* window_name = "LearnOpenGL");
    bool update();
    void destory();

    unsigned int getWidth() const {return mWidth;};
    unsigned int getHeight() const {return mHeight;};
    GLFWwindow* getWindow() const { return mWindow; };
    void getCursorPosition(double* xpos, double* ypos);

    void setResizeCallback(ResizeCallback callback) { mResizeCallback = callback; }
    void setKeyBoardCallback(KeyBoardCallback callback) { mKeyBoardCallback = callback; }
    void setMouseCallback(MouseCallback callback) { mMouseCallback = callback; }
    void setCursorCallback(CursorCallback callback) { mCursorCallback = callback; }
    void setScrollCallback(ScrollCallback callback) { mScrollCallback = callback; }

    void test(){ //后续可以不要
        std::cout << "App test" << std::endl;
    }

private:
    //C++类内函数指针
    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    //全局唯一的静态变量实例
    static Application* mInstance;

    unsigned int mWidth {0};
    unsigned int mHeight {0};  // 当前窗口的长宽值
    GLFWwindow* mWindow {nullptr};  // 当前窗口指针

    ResizeCallback mResizeCallback {nullptr};
    KeyBoardCallback mKeyBoardCallback {nullptr};
    MouseCallback mMouseCallback {nullptr};
    CursorCallback mCursorCallback {nullptr};  // 接收main中传输来的响应函数指针
    ScrollCallback mScrollCallback {nullptr};

    Application();

};

