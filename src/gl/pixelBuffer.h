#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H

struct PixelBuffer
{
    unsigned char* buffer;
    unsigned int bufferLength, imgWidth, imgHeight;

    void clear()
    {
        delete[] buffer;
    }
};

PixelBuffer getPixelBuffer(int bufferWidth, int bufferHeight);

#endif // PIXEL_BUFFER_H
