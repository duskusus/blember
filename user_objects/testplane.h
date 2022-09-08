#pragma once
#include "program.h"
#include "vertexbuffer.h"
#include <cstdlib>
#include <vector>
class TestPlane {
    public:
    const unsigned int width;
    const unsigned int height;
    unsigned int size;
    float *vertices;
    unsigned int vertCount = 0;
    float *heightMap;
    VertexBuffer *v;
    Program &p;
    TestPlane(unsigned int p_width, unsigned int p_height, Program &p_p) : width(p_width), height(p_height), p(p_p) {
        size = width * height * 6 * 3;
        vertices = new float[size];
        heightMap = new float[width * height];
        for(int i = 0; i < width * height; i++) {
            float factor = 1.0 / 16.0f;
            heightMap[i] = float(rand() % 1024) * factor;
        }
        for (unsigned int i = 0; i < width; i++) {
            for (unsigned int j = 0; j < height; j++) {
                vert(i, j);
                vert(i + 1, j);
                vert(i, j+1);

                vert(i + 1, j);
                vert(i + 1, j + 1);
                vert(i, j + 1);
            }
            v = new VertexBuffer("aPos", p);
            v->buffer();
            v->set(vertices, size);
            v->vertexSpec(3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            v->enable();
            v->upload();
        }

    }
    void vert(unsigned int x, unsigned int z) {
        vertices[vertCount] = x;
        vertCount ++;
        vertices[vertCount] = heightMap[x + z*height];
        vertCount ++;
        vertices[vertCount] = z;
        vertCount ++;
    }
    void render() {
        glEnable(GL_CULL_FACE);
        p.use();
        v->bind();
        glDrawArrays(GL_TRIANGLES, 0, size);
    }
};