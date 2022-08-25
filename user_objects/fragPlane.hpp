#include <glm/glm.hpp>
#include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include "program.h"
class FragPlane {
    public:
    float vertices[12] = {-1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0};
    unsigned int indices[6] = {0, 1, 2, 1, 2, 3};
    unsigned int vertexBuffer = 0;
    unsigned int indexBuffer = 0;
    unsigned int vertexArray = 0;
    Program &program;
    FragPlane(Program &p_p): program(p_p) {
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(glGetAttribLocation(program.getID(), "aPos"));

        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }
    void render() {
        glBindVertexArray(vertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
};