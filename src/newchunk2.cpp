#include "newchunk2.h"
#include <stdlib.h>
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
NewChunk2::NewChunk2(Program &p_p) : p(p_p)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    nc2VertexType::registerType();
    blocks = new int[maxBlockCount];
}
NewChunk2::~NewChunk2()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    if (blocks) delete[] blocks;
    if (vertices) delete[] vertices;
}
int nc2nullblock = 1;
int *NewChunk2::getBlock(int x, int y, int z)
{
    if (x >= sideWidth || x < 0 || y >= sideWidth || y < 0 || z >= sideWidth ||
        z < 0) {
        nc2nullblock = 1;
        if (y >= sideWidth) nc2nullblock = 0;
        return &nc2nullblock;
    }
    return &blocks[convertIndex(x, y, z)];
}
nc2VertexType &NewChunk2::newVertex(const glm::vec3 &x)
{
    vertices[vertexCount].pos = x;
    if (vertexCount < maxVertexCount)
        vertexCount++;
    else
        throw(0);
    return vertices[vertexCount - 1];
}
void NewChunk2::prepareMesh()
{
    if (vertices) delete[] vertices;
    vertexCount = 0;
    countBlocks();
    vertices = new nc2VertexType[maxVertexCount];

    for (int x = 0; x < sideWidth; x++) {
        for (int y = 0; y < sideWidth; y++) {
            for (int z = 0; z < sideWidth; z++) {
                const glm::vec3 bp(x, y, z);
                if (*getBlock(x, y, z) > 0) {
                    int color = *getBlock(x, y, z);
                    if (*getBlock(x - 1, y, z) < 1) {
                        // left face
                        const uint8_t indices[] = {0, 3, 7, 4};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]], color);
                    }

                    if (*getBlock(x + 1, y, z) < 1) {
                        // right face
                        const uint8_t indices[] = {1, 5, 6, 2};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]], color);
                    }

                    if (*getBlock(x, y - 1, z) < 1) {
                        // bottom face
                        const uint8_t indices[] = {0, 4, 5, 1};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]], color);
                    }

                    if (*getBlock(x, y + 1, z) < 1) {
                        // top face
                        const uint8_t indices[] = {3, 2, 6, 7};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]], color);
                    }

                    if (*getBlock(x, y, z - 1) < 1) {
                        // back face
                        const uint8_t indices[] = {7, 6, 5, 4};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]], color);
                    }

                    if (*getBlock(x, y, z + 1) < 1) {
                        // front face
                        const uint8_t indices[] = {0, 1, 2, 3};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]], color);
                    }
                }
            }
        }
    }
}
void NewChunk2::newQuad(const glm::vec3 &a, const glm::vec3 &b,
                        const glm::vec3 &c, const glm::vec3 &d)
{
    glm::vec3 color(float(a.y + rand() % 20) / 200.0, 1.0, 1.0);

    newVertex(a).hsv = color;
    newVertex(b).hsv = color;
    newVertex(c).hsv = color;

    newVertex(a).hsv = color;
    newVertex(c).hsv = color;
    newVertex(d).hsv = color;
}
void NewChunk2::newQuad(const glm::vec3 &a, const glm::vec3 &b,
                        const glm::vec3 &c, const glm::vec3 &d, int p_color)
{
    glm::vec3 color(p_color & 0xff, (p_color & 0xff00) >> 8, (p_color & 0xff0000) >> 16);
    color.x /= 255.0;
    color.y /= 255.0;
    color.z /= 255.0;

    newVertex(a).hsv = color;
    newVertex(b).hsv = color;
    newVertex(c).hsv = color;

    newVertex(a).hsv = color;
    newVertex(c).hsv = color;
    newVertex(d).hsv = color;
}
int NewChunk2::countBlocks()
{
    int count = 0;
    for (uint32_t i = 0; i < maxBlockCount; i++) {
        if (blocks[i] > 0) count++;
    }
    blockCount = count;
    maxVertexCount = 36 * blockCount;
    return count;
}
int render_count = 0;
void NewChunk2::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    render_count++;
}
void NewChunk2::prepare()
{
    trees(50);
    prepareMesh();
    darkenVertices();
    printf("VertexCount %d\n", vertexCount);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nc2VertexType) * vertexCount, vertices,
                 GL_STATIC_DRAW);
}
int *NewChunk2::newBlock(int x, int y, int z)
{
    uint32_t index = convertIndex(x, y, z);
    return &blocks[index];
}
int *NewChunk2::newBlock(int x, int y, int z, glm::vec3 &color)
{
    int *b = newBlock(x, y, z);
    *b = (int(255 * color.x))  | (int(255 * color.y) << 8) | (int(255 * color.z) << 16);
    return b;
}
int *NewChunk2::getHeightmapPtr(int x, int z)
{
    return hmap->getHeightmapPtr(x + heightmapXOffset, z + heightmapZOffset);
}
void NewChunk2::loadFromHeightmap(Heightmap &heightmap, int xoffset,
                                  int zoffset)
{
    heightmapXOffset = xoffset;
    heightmapZOffset = zoffset;

    hmap = &heightmap;

    for (int x = heightmapXOffset; x < sideWidth + heightmapXOffset; x++) {
        for (int z = heightmapZOffset; z < sideWidth + heightmapZOffset; z++) {
            int h = *heightmap.getHeightmapPtr(x, z);
            h = h % sideWidth;
            for (int y = 0; y < h; y++) {
                glm::vec3 c((y + rand() % 25) / 255.0, 1.0 - (y / 512.0), 1.0);
                if(y < 80){
                    c.x = 200 / 360.0;
                }
                newBlock(x, y, z, c);
            }
        }
    }
    countBlocks();
}
void NewChunk2::darkenVertices()
{
    for (int i = 0; i < vertexCount; i++) {
        int fillCount = 0;
        nc2VertexType &v = vertices[i];
        int x = v.pos.x;
        int y = v.pos.y;
        int z = v.pos.z;
        if(y < 80){
            continue;
        }
        for (int p = -1; p < 2; p += 1) {
            for (int q = 0; q < 2; q += 1) {
                for (int r = -1; r < 2; r += 1) {
                    if (*getBlock(x + p, y + q, z + r) > 1) fillCount++;
                }
            }
        }

        float brightness = 1.0;
        for (int i = 0; i < fillCount; i++) {
            brightness *= 0.95;
        }

        v.hsv.z = brightness;
    }
}
void NewChunk2::snow()
{
    for (int i = 0; i < vertexCount; i++) {
        nc2VertexType &v = vertices[i];
        if(v.pos.y > 192.0)
            v.hsv.y = 0.0;
    }
}
void NewChunk2::trees(int count)
{
    for(int i = 0; i < count; i++)
    {

        int x = rand() % sideWidth;
        int z = rand() % sideWidth;

        int height = 10 + rand() % 60;
        int leafHeight = 9 + rand() % ((3 * height) / 4 + 1);
        int outerLeafWidth = 10 + rand() % (leafHeight / 2 + 1);
        if(x + outerLeafWidth / 2 > sideWidth || x - outerLeafWidth / 2 < 0 || z + outerLeafWidth / 2 > sideWidth || z - outerLeafWidth / 2 < 0){
            i --;
            continue;
        }

        int h = *getHeightmapPtr(x, z);

        if(h < waterlevel) {
            i --;
            continue;
        }

        for(int j = 0; j < height; j++) {
            
            glm::vec3 woodColor(30.0 / 360.0, 1.0, 0.2);
            newBlock(x, j + h, z, woodColor);
            newBlock(x  + 1, j + h, z, woodColor);
            newBlock(x, j + h, z + 1, woodColor);
            newBlock(x + 1, j + h, z + 1, woodColor);
         
        }



        int leafColorOffset = rand() % 100;

        for(int p = 0; p < leafHeight + 2; p++) {

            int leafWidth = (outerLeafWidth * (leafHeight - p + 1)) / leafHeight;

            for(int q = -leafWidth / 2; q < leafWidth / 2 + 1; q++){

                for(int r = -leafWidth / 2; r < leafWidth / 2  + 1; r++) {
                    
                    int y = p + h + height - leafHeight;
                    glm::vec3 leafColor(0.0, 1.0, 0.35);

                    leafColor.x += leafColorOffset + rand() % 40;
                    leafColor.x /= 360.0;

                    newBlock(x + q, p + h + height - leafHeight, z + r, leafColor);
                }
            }
        }
    }
}