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
    heightmap = new int[colcount]{1};

    slowNoise();
    fastNoise();
    convolveHeightmap();
    normalizeHeightmap();
    clear();

    for (int x = 0; x < chunksize; x++) {
        for (int z = 0; z < chunksize; z++) {
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
void NewChunk::convolveHeightmap()
{
    uint32_t kwid = 40;
    uint32_t kwidVariance = 1;

    int *new_heightmap = new int[colcount]{1};

    for (int x = 0; x < chunksize; x++) {
        for (int y = 0; y < chunksize; y++) {
            int total = 0;
            uint32_t blocks_sampled = 0;
            uint32_t lkwid = kwid + rand() % kwidVariance - kwidVariance / 2;
            for (int i = 0; i < lkwid; i++) {
                for (int j = 0; j < lkwid; j++) {
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
    int min = 0;
    int sum = 0;
    for (int i = 0; i < colcount; i++) {
        if (heightmap[i] < min) {
            min = heightmap[i];
        }
        sum += heightmap[i];
    }
    const int avg = int(float(sum) / float(blockcount) + 0.5);
    const int offset = avg;
    for (int i = 0; i < colcount; i++) {
        heightmap[i] -= offset;
    }
}

inline int *NewChunk::getHeightmapPtr(int x, int y)
{
    return NewChunk::getHeightmapPtr(x, y, heightmap);
}

inline int NewChunk::getHeightmapVal(int x, int y)
{
    int *h = getHeightmapPtr(x, y);
    if (h) return *h;
    return 0;
}

inline int *NewChunk::getHeightmapPtr(int x, int y, int *h)
{
    if (x < 0 || x >= chunksize || y < 0 || y >= chunksize) return NULL;
    const int index = x + y * chunksize;
    return &h[index];
}

block &NewChunk::at(int index)
{
	if(index <= renderableBlockCount)
	{
		return blocks[index];
	}
	return nullblock;
}

void NewChunk::slowNoise()
{
    for (int i = 0; i < 25; i++) {
        const int disturbMagnitude = 1;
        const float sloperange = 0.4;

        int x = rand() % chunksize;
        int y = rand() % chunksize;

        int disturbSize = rand() % chunksize;
        int disturbHeight = rand() % disturbMagnitude;
        float disturbAngle = float(rand()) / float(RAND_MAX) * 0.1 * M_PI;
        float cosDisturbAngle = cos(disturbAngle);
        float sinDisturbAngle = sin(disturbAngle);
        float disturbSlopeX =
            float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;
        float disturbSlopeY =
            float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;

        for (int q = 0; q < disturbSize; q++) {
            for (int p = 0; p < disturbSize; p++) {
                float xx = q - disturbSize / 2 + float(x);
                float yy = p - disturbSize / 2 + float(y);

                xx = xx * cosDisturbAngle - yy * sinDisturbAngle;
                yy = xx * sinDisturbAngle + yy * cosDisturbAngle;

                int *h = getHeightmapPtr(xx, yy);
                if (h) {
                    *h += disturbHeight + xx * disturbSlopeX +
                          yy * disturbSlopeY + 0.5;
                }
            }
        }
    }
}
void NewChunk::clear() { renderableBlockCount = 0; }

block &NewChunk::newBlock(const glm::vec3 position)
{
    block &b = at(renderableBlockCount);
    b.position = position;
    b.type = int(position.y + rand() % 50);
    if (renderableBlockCount < blockcount - 1) renderableBlockCount++;
    return b;
}

void NewChunk::render()
{
    glBindVertexArray(vao);
    sp.use();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
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