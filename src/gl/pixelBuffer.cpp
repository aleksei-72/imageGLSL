#include <GL/glew.h>
#include <src/gl/pixelBufferToTGA.h>

PixelBuffer getPixelBuffer(int bufferWidth, int bufferHeight)
{
    // ширина*высота*3 цветовых бита
    int bufferLenght = bufferWidth * bufferHeight * 3;

    unsigned char *output = new unsigned char[bufferLenght];

    glReadPixels(0,0, bufferWidth, bufferHeight, GL_RGB, GL_UNSIGNED_BYTE, output);

    return {output, bufferLenght, bufferWidth, bufferHeight};
}
