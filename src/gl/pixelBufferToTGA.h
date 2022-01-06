#ifndef FRAME_BUFFER_TO_TGA
#define FRAME_BUFFER_TO_TGA

#include <GL/glew.h>
#include <iostream>
#include <src/gl/pixelBuffer.h>

int pixelBufferToTGA(PixelBuffer buffer, std::string fname);

#endif // FRAME_BUFFER_TO_TGA
