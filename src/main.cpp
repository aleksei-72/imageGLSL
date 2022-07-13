#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <src/gl/texture.h>
#include <src/public/public.h>
#include <src/gl/shader.h>
#include <src/gl/primitive.h>
#include <src/gl/pixelBufferToTGA.h>
#include <src/timer/timer.h>
#include <glm/vec2.hpp>
#include <vector>
#include <array>

using namespace std;

struct Parameters
{
    bool manyOutputFiles = false;
    std::vector<string> textureNames;
    string outputFileTemplate = "result/result%w%_%h%.tga";
};

Parameters getParameters(int argc, char* argv[])
{
    Parameters param;

    const int ARG_SOURCE_FILES = 1;
    const int ARG_OUTPUT_FILE  = 2;

    int mode = 0;

    for(int i = 1; i < argc; i++)
    {
        string arg = string(argv[i]);

        if (arg[0] == '-')
        {
            if (arg == "-s")
            {
                mode = ARG_SOURCE_FILES;
                continue;
            }

            if (arg == "-o")
            {
                mode = ARG_OUTPUT_FILE;
                continue;
            }

            if (arg == "-m")
            {
                param.manyOutputFiles = true;
                continue;
            }

            logger.warn("undefined argument " + arg);
            continue;
        }

        if (!mode)
        {
            logger.warn("ignore parameter " + arg);
            continue;
        }

        if (mode == ARG_SOURCE_FILES)
        {
            param.textureNames.push_back(arg);
        }

        if (mode == ARG_OUTPUT_FILE)
        {
            param.outputFileTemplate = arg;
        }
    }

    return param;
}

int main(int argc, char* argv[])
{
    Timer t;
    Parameters params = getParameters(argc, argv);

    if (!params.textureNames.size())
    {
        logger.error("no input textures");
    }

    if (glfwInit() == GL_FALSE)
    {
        logger.error("fail for init GLFW");
        return 1;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(640, 480, "", NULL, NULL);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit())
    {
        logger.error("fail for init GLEW");
        return 1;
    }

    glGetError();//fix bug with false error for glewInit();

    if (!GLEW_VERSION_3_3)
    {
        logger.error("OpenGL 3.3 is required");
        return 1;
    }

    Shader shader;
    shader.LoadFromFile("vertex.glsl", "0", "fragment.glsl");

    logger.info("init OpenGL", to_string(t.getElapsedTime()) + " ms");


    int textureCount = params.textureNames.size();

    Texture* texture = new Texture[textureCount];

    for (int i = 0; i < textureCount; i++)
    {
        texture[i] = loadTexture(params.textureNames[i]);
        logger.info("Read file '" + params.textureNames[i] + '\'', to_string(t.getElapsedTime()) + " ms");
    }

    int resolutionsCount = 11,
        resolutionWidths[resolutionsCount] = {16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16};


    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    for (int i = 0; i < resolutionsCount; i++)
    {

        glm::uvec2 resolution = {resolutionWidths[i], resolutionWidths[i] / (texture[0].w / texture[0].h)};

        if ((resolution.x > maxTextureSize || resolution.y > maxTextureSize) ||
            texture[0].w < resolution.x)
            continue;


        unsigned int offscreenBuffer;
        glGenFramebuffers(1, &offscreenBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, offscreenBuffer);

        unsigned int offscreenTexture;
        glGenTextures(1, &offscreenTexture);

        glBindTexture(GL_TEXTURE_2D, offscreenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offscreenTexture, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                logger.error("OpenGl error", "off-screen frameBuffer is not complete");

        SimpleMesh quad = generateQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, offscreenBuffer);

        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.x, resolution.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        glBindFramebuffer(GL_FRAMEBUFFER, offscreenBuffer);


        // render
        glViewport(0, 0, resolution.x, resolution.y);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.programm);

        for (int j = 0; j < textureCount; j++)
        {
            glActiveTexture(GL_TEXTURE0 + j);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_2D, texture[j].texture);

            glUniform1i(shader.getUniformPos("texture" + to_string(j + 1)), j);
        }

        glBindVertexArray(quad.VAO);
        glDrawArrays(GL_TRIANGLES, 0, quad.triangleCount);

        glFinish();

        string outputFileName = params.outputFileTemplate;

        std::array<pair<string, string>, 2> replaces = {
            std::pair<string, string>("{w}", to_string(resolution.x)),
            std::pair<string, string>("{h}", to_string(resolution.y))
        };

        for (pair<string, string> i : replaces)
        {
            int pos = outputFileName.find(i.first);
            if (pos != std::string::npos)
            {
                outputFileName = outputFileName.replace(pos, i.first.length(), i.second);
            }
        }

        outputFileName += ".tga";

        logger.info("Render " + outputFileName, to_string(t.getElapsedTime()) + " ms");

        PixelBuffer buffer = getPixelBuffer(resolution.x, resolution.y);
        pixelBufferToTGA(buffer, outputFileName);

        buffer.clear();

        logger.info("Write file '" + outputFileName + '\'', to_string(t.getElapsedTime()) + " ms");

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);

        glDeleteFramebuffers(1, &offscreenBuffer);
        glDeleteTextures(1, &offscreenTexture);
        glDeleteRenderbuffers(1, &rbo);

        if (!params.manyOutputFiles)
        {
            break;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
