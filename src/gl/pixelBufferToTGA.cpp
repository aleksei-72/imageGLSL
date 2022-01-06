#include <GL/glew.h>
#include <src/public/public.h>
#include <src/gl/pixelBufferToTGA.h>

int pixelBufferToTGA(PixelBuffer buffer, string fname)
{
    FILE *sFile = 0;

    unsigned char tgaHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char header[6];
    unsigned char bits = 0;
    int colorMode = 0;
    unsigned char tempColors = 0;

    sFile = fopen(fname.c_str(), "wb");

    if(!sFile)
    {
        logger.error("can not open file", "file :" + fname);
        return 1;
    }

    colorMode = 3;
    bits = 24;

    header[0] = buffer.imgWidth % 256;
    header[1] = buffer.imgWidth / 256;
    header[2] = buffer.imgHeight % 256;
    header[3] = buffer.imgHeight / 256;
    header[4] = bits;
    header[5] = 0;

    fwrite(tgaHeader, sizeof(tgaHeader), 1, sFile);
    fwrite(header, sizeof(header), 1, sFile);

// ѕоскольку в формате TGA цвета хран€тс€ не в RGB, а в BRG, нам нужно
// помен€ть местами наши данные:
    for(int i=0; i < buffer.imgWidth * buffer.imgHeight * colorMode; i+=colorMode)
    {
        tempColors = buffer.buffer[i];
        buffer.buffer[i] = buffer.buffer[i+2];
        buffer.buffer[i+2] = tempColors;
    }

    fwrite(buffer.buffer, buffer.imgWidth * buffer.imgHeight * colorMode, 1, sFile);
    fclose(sFile);

    return 0;
}
