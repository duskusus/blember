#include "newchunk2.h"
const glm::vec3 cubeVertices[] = {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1},
                                  {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
void deConvertIndex(uint8_t *x, uint8_t *y, uint8_t *z, uint32_t index)
{
    *z = index & 0x000000ff;
    *y = (index & 0xff00) >> 8;
    *x = (index & 0xff0000) >> 16;
}
inline uint32_t convertIndex(int x, int y, int z)
{
    return ((x & 0xff) << 16) | ((y & 0xff) << 8) | (z & 0xff);
}
NewChunk2::NewChunk2(int xOffset, int yOffset, int zOffset, int *p_heightmap, Program &p_p)
    : heightmapXOffset(xOffset),
      heightmapYOffset(yOffset),
      heightmapZOffset(zOffset),
      heightmap(p_heightmap),
      p(p_p)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    blocks = new int[maxBlockCount];
}
NewChunk2::~NewChunk2()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    if (blocks) delete[] blocks;
    if (vertices) delete[] vertices;
}
int nullblock = 0;
int *NewChunk2::getBlock(uint8_t x, uint8_t y, uint8_t z)
{
    uint32_t index = convertIndex(x, y, z);
    if(index < blockCount)return &blocks[index];
    printf("missed blocks\n");
    return &nullblock;
}
void NewChunk2::newVertex(const glm::vec3 &x)
{
    vertices[vertexCount] = x;
    if (vertexCount < maxVertexCount)
        vertexCount++;
    else
        throw(0);
}
void NewChunk2::prepareMesh()
{
    if (vertices) delete[] vertices;
    vertexCount = 0;
    countBlocks();
    vertices = new glm::vec3[maxVertexCount];

    for (int x = 0; x < sideWidth; x++) {

        for (int y = 0; y < sideWidth; y++) {

            for (int z = 0; z < sideWidth; z++) {

                int &b = blocks[convertIndex(x, y, z)];

                const glm::vec3 bp(x, y, z);
                if(b > 0){
                if (blocks[convertIndex(x - 1, y, z)] < 1) {
                    // left face
                    const uint8_t indices[] = {0, 3, 7, 4};

                    newQuad(bp + cubeVertices[indices[0]],
                            bp + cubeVertices[indices[1]],
                            bp + cubeVertices[indices[2]],
                            bp + cubeVertices[indices[3]]);
                }

                if (blocks[convertIndex(x, y, z)] < 1) {
                    // right face
                    const uint8_t indices[] = {1, 5, 6, 2};

                    newQuad(bp + cubeVertices[indices[0]],
                            bp + cubeVertices[indices[1]],
                            bp + cubeVertices[indices[2]],
                            bp + cubeVertices[indices[3]]);
                }

                if (blocks[convertIndex(x, y - 1, z)] < 1) {
                    // bottom face
                    const uint8_t indices[] = {0, 4, 5, 1};

                    newQuad(bp + cubeVertices[indices[0]],
                            bp + cubeVertices[indices[1]],
                            bp + cubeVertices[indices[2]],
                            bp + cubeVertices[indices[3]]);
                }

                if (blocks[convertIndex(x, y + 1, z)] < 1) {
                    // top face
                    const uint8_t indices[] = {3, 2, 6, 7};

                    newQuad(bp + cubeVertices[indices[0]],
                            bp + cubeVertices[indices[1]],
                            bp + cubeVertices[indices[2]],
                            bp + cubeVertices[indices[3]]);
                }

                if (blocks[convertIndex(x, y, z - 1)] < 1) {
                    // back face
                    const uint8_t indices[] = {7, 6, 5, 4};

                    newQuad(bp + cubeVertices[indices[0]],
                            bp + cubeVertices[indices[1]],
                            bp + cubeVertices[indices[2]],
                            bp + cubeVertices[indices[3]]);
                }

                if (blocks[convertIndex(x, y + 1, z + 1)] < 1) {
                    // front face
                    const uint8_t indices[] = {0, 1, 2, 3};

                    newQuad(bp + cubeVertices[indices[0]],
                            bp + cubeVertices[indices[1]],
                            bp + cubeVertices[indices[2]],
                            bp + cubeVertices[indices[3]]);
                }
                }
            }
        }
    }
}
void NewChunk2::newQuad(const glm::vec3 &a, const glm::vec3 &b,
                        const glm::vec3 &c, const glm::vec3 &d)
{
    newVertex(a);
    newVertex(b);
    newVertex(c);

    newVertex(a);
    newVertex(c);
    newVertex(d);
}
int NewChunk2::countBlocks()
{
    int count = 0;
    for (uint32_t i = 0; i < maxBlockCount; i++) {
        if (blocks[i] > 0) count++;
    }
    blockCount = count;
    maxVertexCount = 40 * blockCount;
    return count;
}
void NewChunk2::render()
{
    p.use();
    glBindVertexArray(vao);
    glDisable(GL_CULL_FACE);    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
void NewChunk2::prepare()
{
    prepareMesh();
    printf("VertexCount %d\n", vertexCount);
    for(int i = 0; i < 100; i++){
        printf("X: %f, Y: %f, Z: %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertexCount, vertices, GL_STATIC_DRAW);
}
int *NewChunk2::newBlock(int x, int y, int z)
{
    uint32_t index = convertIndex(x, y, z);
    if(index > maxBlockCount) throw(0);
    return &blocks[index];
    blockCount ++;
}
void NewChunk2::loadFromHeightmap()
{
    for(int i = 0; i < 50; i++) {
        *newBlock(rand() % 50, rand() % 50, rand() % 50)  = 10;
    }
    countBlocks();
}