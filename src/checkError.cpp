//错误检查函数
//opengl中的错误发生往往是隐藏的，因此需要使用glGetError()函数来发现错误
#include<iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include<string>
#include<assert.h> //断言

//错误检查函数
void checkError(void)
{
    //检查错误
    GLenum errorCode = glGetError();
    std::string error = "";
    if(errorCode != GL_NO_ERROR){
        switch(errorCode)
        {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            default:
                error = "UNKNOW";
                break;
        }
        std::cout << error << std::endl;
        //断言，assert会根据传入的bool值，True程序顺利执行，False程序终止运行
        assert(false);
    }
}