#include "heightmap.h"

#include <stdlib.h>
#include <math.h>

#include <iostream>
Heightmap::Heightmap(uint32_t p_sideLength) : sideLength(p_sideLength)
{
    map = new int[size];
}
Heightmap::~Heightmap() { delete[] map; }
void Heightmap::generate()
{
    
    for(int i = 0; i < size; i++) {
        map[i] = 1;
    }
    slowNoise(0.4, 2, 4 * sideLength, 1);
    slowNoise(0.3, 20, sideLength / 2, sideLength / 2);
    convolve(40);
    normalize();
}
void Heightmap::convolve(uint32_t kwid)
{
    int *new_heightmap = new int[size]{1};

    for (int x = 0; x < sideLength; x++) {
        for (int y = 0; y < sideLength; y++) {
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
    delete[] map;
    map = new_heightmap;
}
int *Heightmap::getHeightmapPtr(int x, int z)
{
    return getHeightmapPtr(x, z, map);
}
int *Heightmap::getHeightmapPtr(int x, int z, int *p_heightmap)
{
    if (x < 0 || x >= sideLength || z < 0 || z >= sideLength) return NULL;

    const int index = x + z * sideLength;
    return &p_heightmap[index];
}
void Heightmap::slowNoise(const float sloperange, int count, const int sizeOffset, const int sizeRange)
{
    for (int i = 0; i < count; i++) {

        int x = rand() % sideLength - sideLength / 2;
        int y = rand() % sideLength - sideLength / 2;

        int disturbSizeX = rand() % sizeRange + sizeOffset;
        int disturbSizeY = rand() % sizeRange + sizeOffset;

        float disturbSlopeX =
            float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;
        float disturbSlopeY =
            float(rand()) / float(RAND_MAX) * sloperange - sloperange / 2.0;

        printf("slopes %f, %f\n", disturbSlopeX, disturbSlopeY);
        for (int q = -disturbSizeX / 2; q < disturbSizeX / 2; q++) {
            for (int p = -disturbSizeY / 2; p < disturbSizeY / 2; p++) {

                int xx = p + x;
                int yy = q + y;

                int *h = getHeightmapPtr(xx, yy);
                if (h) {
                    
                    const int inc = p * disturbSlopeX +
                          q * disturbSlopeY + 0.5;
                    *h += inc;
                }
            }
        }
    }
}
void Heightmap::normalize()
{
    int min = 999999;
    int sum = 0;
    for (int i = 0; i < size; i++) {
        if (map[i] < min) {
            min = map[i];
        }
        sum += map[i];
    }
    const int offset = min;
    for (int i = 0; i < size; i++) {
        map[i] -= offset;
    }
}