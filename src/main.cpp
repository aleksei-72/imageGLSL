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

using namespace std;

int main(int argc, char* argv[])
{
    Timer t;

    int textureCount = 2;
    string textureFileNames[textureCount] = {"res/file1.jpg", "res/file2.jpg"};


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

    Texture* texture = new Texture[textureCount];

    for (int i = 0; i < textureCount; i++)
    {
        texture[i] = loadTexture(textureFileNames[i]);
        logger.info("Read file '" + textureFileNames[i] + '\'', to_string(t.getElapsedTime()) + " ms");
    }

    int resolutionsCount = 11,
        resolutionWidths[resolutionsCount] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};


    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    for (int i = 0; i < resolutionsCount; i++)
    {

        glm::uvec2 resolution = {resolutionWidths[i], resolutionWidths[i] / (texture[0].w / texture[0].h)};

        if ((resolution.x > maxTextureSize || resolution.y > maxTextureSize) ||
            texture[0].w < resolution.x)
            break;


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

        string resolutionStr = to_string(resolution.x) + "x" + to_string(resolution.y),
               fname = "result/file_" + resolutionStr + ".tga";

        logger.info("Render " + resolutionStr, to_string(t.getElapsedTime()) + " ms");

        PixelBuffer buffer = getPixelBuffer(resolution.x, resolution.y);
        pixelBufferToTGA(buffer, fname);

        buffer.clear();

        logger.info("Write file '" + fname + '\'', to_string(t.getElapsedTime()) + " ms");

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);

        glDeleteFramebuffers(1, &offscreenBuffer);
        glDeleteTextures(1, &offscreenTexture);
        glDeleteRenderbuffers(1, &rbo);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
