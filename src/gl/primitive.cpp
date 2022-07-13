#include <GL/glew.h>
#include <src/gl/primitive.h>

SimpleMesh generateQuad()
{
    GLuint quadVao;

    //QUAD
    float buffer[] =
    {
        1.0, 1.0, 0.0, 1.0, 1.0,
        -1.0, 1.0, 0.0, 0.0, 1.0,
        -1.0, -1.0, 0.0, 0.0, 0.0,

        -1.0, -1.0, 0.0, 0.0, 0.0,
        1.0, -1.0, 0.0, 1.0, 0.0,
        1.0, 1.0, 0.0, 1.0, 1.0

    };

    GLuint quadVbo;
    glGenBuffers(1, &quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);


    glGenVertexArrays(1, &quadVao);

    glBindVertexArray(quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void *)((char*)0 + 3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindVertexArray(0); // unbind VAO

    return {quadVao, 6};
}
