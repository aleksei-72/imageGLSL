#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <src/gl/shader.h>
#include <src/public/public.h>

using namespace std;

int Shader::LoadFromFile(std::string vertexPath, std::string geometryPath, std::string fragmentPath)
{
    return LoadFromFile(vertexPath.c_str(), geometryPath.c_str(), fragmentPath.c_str());
}

int Shader::LoadFromFile(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath)
{

    if(geometryPath == 0 || string(geometryPath) == "0")
    {
        isGeometry = false;
        const GLchar *t = "null";
        geometryPath = t;
    }

    string svertexPath = string(settings.basePath + settings.shaderPath + string(vertexPath)).c_str();
    string sgeometryPath = string(settings.basePath + settings.shaderPath + string(geometryPath)).c_str();
    string sfragmentPath = string(settings.basePath + settings.shaderPath + string(fragmentPath)).c_str();

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // ensures ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(svertexPath);
        fShaderFile.open(sfragmentPath);
        if(isGeometry)
        {
            gShaderFile.open(sgeometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometrySource = gShaderStream.str();
        }

        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexSource = vShaderStream.str();
        fragmentSource = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        logger.error("can't open file" , "files: " + svertexPath+ "', '"+ sgeometryPath+ "', '"+ sfragmentPath+ "'");
        return 1;
    }

    //Компиляция шейдера
    const char* vertexCode = vertexSource.c_str();
    const char* fragmentCode = fragmentSource.c_str();
    const char* geometryCode = geometrySource.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);

    GLint success;//результат компил¤ции
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        logger.error("fail for compile Shader :" + svertexPath, string(infoLog));
        return 1;
    }

    geometryShader = 0;

    if(isGeometry)
    {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryCode, NULL);
        glCompileShader(geometryShader);

        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);//результат компил¤ции
        if (!success)
        {
            glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
            logger.error("fail for compile Shader :" + sgeometryPath, string(infoLog));
            return 1;
        }
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);//результат компил¤ции
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        logger.error("fail for compile Shader :" + sfragmentPath, string(infoLog));
        return 1;
    }

    programm = glCreateProgram();
    glAttachShader(programm, vertexShader);

    if(isGeometry)
        glAttachShader(programm, geometryShader);

    glAttachShader(programm, fragmentShader);
    glLinkProgram(programm);

    glGetProgramiv(programm, GL_LINK_STATUS, &success);//наличие ошибок
    if (!success)
    {
        glGetProgramInfoLog(programm, 512, NULL, infoLog);
        logger.error("fail for compile Shader programm",
                      "files :" + svertexPath + ", " + sgeometryPath + ", " + sfragmentPath + "\n\n" + string(infoLog));
        return 1;
    }
    glDeleteShader(vertexShader);

    if(isGeometry)
        glDeleteShader(geometryShader);

    glDeleteShader(fragmentShader);

    return true;
}

GLint Shader::getUniformPos(std::string uniformName)
{
    return glGetUniformLocation(programm, uniformName.c_str());
}

Shader::~Shader()
{
    if (programm)
    {
        glDetachShader(programm, vertexShader);
        glDetachShader(programm, fragmentShader);

        if(isGeometry)
        {
            glDetachShader(programm, geometryShader);
        }

        glDeleteProgram(programm);
    }
}
