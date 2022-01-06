#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <src/gl/texture.h>
#include <src/public/public.h>

#include <iostream>

using namespace std;

GLuint sfmlLoadTexture(string filename, int *width, int *height)
{
    sf::Image image;
    if (!image.loadFromFile(filename))
    {
        logger.error("fail for load texture: " + filename);
        throw TEXTURE_LOAD_ERROR;
    }

    image.flipVertically();


    *width = image.getSize().x;
    *height = image.getSize().y;

    const unsigned char *tex = image.getPixelsPtr();

    logger.info("Read texture '" + filename + '\'', "size: " + to_string(*width) + "x" + to_string(*height));

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, tex);

    return texture;
}



Texture loadTexture(string filename)
{
    int width, height;
    GLuint texture;

    try {
        texture = sfmlLoadTexture(filename, &width, &height);
    }
    catch (...) {
        texture = sfmlLoadTexture("default.png", &width, &height);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // Disable MipMapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    //glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return {texture, width, height};
}
