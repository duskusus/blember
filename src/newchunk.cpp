#include "newchunk.h"

#include "cube.h"
NewChunk::NewChunk(Program &p_sp, Uniform &p_model, uint32_t p_chunksize)
    : sp(p_sp), u_model(p_model), chunksize(p_chunksize)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glm::mat4 modelView(1.0);
    blocks = new block[blockcount];

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vertPos);
    glVertexAttribPointer(vertPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

    block::registerType();  // make shader program aware of existence
    // of block type

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

void NewChunk::generate()
{
    if(heightmap) delete[] heightmap;
    heightmap = new int[colcount]{1};

    slowNoise(1, 100, 200, 50);
    slowNoise(0.2, 100, chunksize, chunksize);
    slowNoise(0.4, 10, chunksize, 1);
    //fastNoise();
    convolveHeightmap(40);
    normalizeHeightmap();
    clear();


    // fill in holes
    for (int x = 1; x < chunksize - 1; x++) {
        for (int z = 1; z < chunksize - 1; z++) {
            int neighbors[4];

            int p = getHeightmapVal(x, z);

            neighbors[0] = getHeightmapVal(x + 1, z);
            neighbors[1] = getHeightmapVal(x - 1, z);
            neighbors[2] = getHeightmapVal(x, z + 1);
            neighbors[3] = getHeightmapVal(x, z - 1);

            int min = p;

            for (int i = 0; i < 4; i++) {
                if (min > neighbors[i]) min = neighbors[i];
            }

            if (p - min > stacklimit) continue;
            for (int h = min; h <= p; h++) {
                newBlock(glm::vec3(x, h, z));
            }
        }
    }
}
void NewChunk::convolveHeightmap(uint32_t kwid)
{

    int *new_heightmap = new int[colcount]{1};

    for (int x = 0; x < chunksize; x++) {
        for (int y = 0; y < chunksize; y++) {
            int total = 0;
            uint32_t blocks_sampled = 0;
            for (int i = 0; i < kwid; i++) {
                for (int j = 0; j < kwid; j++) {
                    int *h =
                        getHeightmapPtr(x + i - kwid / 2, y + j - kwid / 2);
                    if (h) {
                        total += *h;
                        blocks_sampled++;
                    }
                }
            }

            if (blocks_sampled > 0) {
                const int newval =
                    int(float(total) / float(blocks_sampled) + 0.5);
                *getHeightmapPtr(x, y, new_heightmap) = newval;
            }
            else {
                *getHeightmapPtr(x, y, new_heightmap) = *getHeightmapPtr(x, y);
            }
        }
    }
    delete[] heightmap;
    heightmap = new_heightmap;
}

void NewChunk::fastNoise()
{
    for (int i = 0; i < colcount; i++) {
        heightmap[i] += rand() % 20;
    }
}

void NewChunk::normalizeHeightmap()
{
    int min = 9999;
    int sum = 0;
    for (int i = 0; i < colcount; i++) {
        if (heightmap[i] < min) {
            min = heightmap[i];
        }
        sum += heightmap[i];
    }
    const int avg = int(float(sum) / float(blockcount) + 0.5);
    const int offset = min;
    averageBlockHeight = avg - min;
    for (int i = 0; i < colcount; i++) {
        heightmap[i] -= offset;
    }
}

 int *NewChunk::getHeightmapPtr(int x, int y)
{
    return NewChunk::getHeightmapPtr(x, y, heightmap);
}

int NewChunk::getHeightmapVal(int x, int y)
{
    int *h = getHeightmapPtr(x, y);
    if (h) return *h;
    return 0;
}

 int *NewChunk::getHeightmapPtr(int x, int y, int *h)
{
    if (x < 0 || x >= chunksize || y < 0 || y >= chunksize) return NULL;
    const int index = x + y * chunksize;
    return &h[index];
}

block &NewChunk::at(int index)
{
    if (index == renderableBlockCount) {
        return blocks[index];
    }
    return nullblock;
}

void NewChunk::slowNoise(const float sloperange, int count, const int sizeOffset, const int sizeRange)
{
    for (int i = 0; i < count; i++) {

        int x = rand() % chunksize - chunksize / 2;
        int y = rand() % chunksize - chunksize / 2;

        int disturbSizeX = rand() % sizeRange + sizeOffset;
        int disturbSizeY = rand() % sizeRange + sizeOffset;

        float disturbSlopeX =
            float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;
        float disturbSlopeY =
            float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;
        
        disturbSlopeX *= disturbSlopeX;
        disturbSlopeY *= disturbSlopeY;

        for (int q = -disturbSizeX / 2; q < disturbSizeX / 2; q++) {
            for (int p = -disturbSizeY / 2; p < disturbSizeY / 2; p++) {

                int xx = p + x;
                int yy = q + y;

                int *h = getHeightmapPtr(xx, yy);
                if (h) {
                    *h += p * disturbSlopeX +
                          q * disturbSlopeY + 0.5;
                }
            }
        }
    }
}
void NewChunk::clear() { renderableBlockCount = 0; }

block &NewChunk::newBlock(const glm::vec3 position)
{
    if (renderableBlockCount < blockcount - 1) renderableBlockCount++;

    block &b = at(renderableBlockCount);
    b.position = position;
    b.type = b.position.y + rand() % 20;
    return b;
}

void NewChunk::render()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElementsInstanced(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, 0,
                            renderableBlockCount);
}

void NewChunk::sync()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, renderableBlockCount * sizeof(block), blocks,
                 GL_DYNAMIC_DRAW);
}

NewChunk::~NewChunk()
{
    if (heightmap != nullptr) delete[] heightmap;
    delete[] blocks;
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &instanceBuffer);
    glDeleteVertexArrays(1, &vao);
}