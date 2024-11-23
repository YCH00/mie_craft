#include "mine/application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Application* Application::mInstance = nullptr;

Application* Application::getInstance()
{
    if(mInstance == nullptr)
        mInstance = new Application();

    return mInstance;
}

Application::Application()
{

}

Application::~Application()
{

}

bool Application::init(const unsigned int& width, const unsigned int& height, const char* window_name)
{
    mWidth = width;
    mHeight = height;

    //1，初始化GLFW的基本环境
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); //设置核心模式（非立即渲染模式）

    //2，创建窗体循环
    mWindow = glfwCreateWindow(mWidth,mHeight,window_name,NULL,NULL); 
    if(mWindow == NULL){
        return false;
    }
    glfwMakeContextCurrent(mWindow); //设置当前窗体为opengl绘制的舞台

    //加载opengl函数
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    //设置窗口大小响应
    glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);
    //设置键盘响应
    glfwSetKeyCallback(mWindow,keyCallback);
    //设置鼠标点击事件响应
    glfwSetMouseButtonCallback(mWindow, mouseCallback);
    //设置鼠标移动事件响应
    glfwSetCursorPosCallback(mWindow, cursorCallback);
    //设置鼠标滚轮事件响应
    glfwSetScrollCallback(mWindow, scrollCallback);

    glfwSetWindowUserPointer(mWindow,this); //将this指针暂时存入mWindow



    return true;
}

bool Application::update()
{
    if(glfwWindowShouldClose(mWindow)){
        return false;
    }
    glfwPollEvents();
    glfwSwapBuffers(mWindow);

    return true;
}

void Application::destory()
{
    //4，退出程序前做相关清理
    glfwTerminate();
}

void Application::getCursorPosition(double* xpos, double* ypos)
{
    glfwGetCursorPos(mWindow, xpos, ypos);
}

//一个响应窗体大小变化的函数
void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if(self->mResizeCallback != nullptr){
        self->mResizeCallback(width,height);
    }
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // 其实可以直接在这里定死响应函数，而不需要在main中传进来函数指针之后，调用函数指针实现响应
    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if(self->mKeyBoardCallback != nullptr){
        self->mKeyBoardCallback(key,action,mods);
    }
}

void Application::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if(self->mMouseCallback != nullptr){
        self->mMouseCallback(button,action,mods);
    }
}

void Application::cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if(self->mCursorCallback != nullptr){
        self->mCursorCallback(xpos, ypos);
    }
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* self = (Application*)glfwGetWindowUserPointer(window);
    if(self->mScrollCallback != nullptr){
        self->mScrollCallback(yoffset);
    }
}
