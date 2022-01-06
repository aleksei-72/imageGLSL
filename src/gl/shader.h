#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Shader
{
public:
    ~Shader();
    GLuint programm = 0;

    int LoadFromFile(std::string vertexPath, std::string geometryPath, std::string fragmentPath);
    int LoadFromFile(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);

    GLint getUniformPos(std::string uniformName);
private:

    GLuint vertexShader, fragmentShader,geometryShader;
    std::string vertexSource, geometrySource, fragmentSource;
    bool isGeometry = true;
};

#endif // SHADER_H
