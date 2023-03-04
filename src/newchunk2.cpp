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
                    if (*getBlock(x - 1, y, z) < 1) {
                        // left face
                        const uint8_t indices[] = {0, 3, 7, 4};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]]);
                    }

                    if (*getBlock(x + 1, y, z) < 1) {
                        // right face
                        const uint8_t indices[] = {1, 5, 6, 2};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]]);
                    }

                    if (*getBlock(x, y - 1, z) < 1) {
                        // bottom face
                        const uint8_t indices[] = {0, 4, 5, 1};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]]);
                    }

                    if (*getBlock(x, y + 1, z) < 1) {
                        // top face
                        const uint8_t indices[] = {3, 2, 6, 7};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]]);
                    }

                    if (*getBlock(x, y, z - 1) < 1) {
                        // back face
                        const uint8_t indices[] = {7, 6, 5, 4};

                        newQuad(bp + cubeVertices[indices[0]],
                                bp + cubeVertices[indices[1]],
                                bp + cubeVertices[indices[2]],
                                bp + cubeVertices[indices[3]]);
                    }

                    if (*getBlock(x, y, z + 1) < 1) {
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
    glm::vec3 color(float(a.y + rand() % 20) / 200.0, 1.0, 1.0);

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
    prepareMesh();
    darkenVertices();
    snow();
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
void NewChunk2::loadFromHeightmap(Heightmap &heightmap, int xoffset,
                                  int zoffset)
{
    if (xoffset > 0) heightmapXOffset = xoffset;
    if (zoffset > 0) heightmapZOffset = zoffset;
    std::cout << "loading from heightmap\n";

    for (int x = heightmapXOffset; x < sideWidth + heightmapXOffset; x++) {
        for (int z = heightmapZOffset; z < sideWidth + heightmapZOffset; z++) {
            int h = *heightmap.getHeightmapPtr(x, z);
            h = h % sideWidth;
            for (int y = 0; y < h; y++) {
                *newBlock(x, y, z) = 10;
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

        for (int p = -1; p < 2; p += 2) {
            for (int q = -1; q < 2; q += 2) {
                for (int r = -1; r < 2; r += 2) {
                    if (*getBlock(x + p, y + q, z + r) > 1) fillCount++;
                }
            }
        }

        float brightness = 1.0;
        for (int i = 0; i < fillCount - 4; i++) {
            brightness *= 0.9;
        }

        v.hsv.z = brightness;
    }
}
void NewChunk2::snow()
{
    for (int i = 0; i < vertexCount; i++) {
        nc2VertexType &v = vertices[i];
        if(v.pos.y > 192.0)
            v.hsv.y = 1.0 - v.pos.y / 256.0;
    }
}