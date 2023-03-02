#pragma once
#include <stdint.h>
class Heightmap{
    uint32_t sideLength = 256;
    uint32_t size = sideLength * sideLength;
    int *map = nullptr;

    public:
    Heightmap(uint32_t p_sideLength);
    ~Heightmap();
    void generate();

    void convolve(uint32_t kwid);
    void slowNoise(const float sloperange, int count, const int sizeOffset, const int sizeRange);
    int *getHeightmapPtr(int x, int z);
    int *getHeightmapPtr(int x, int z, int *p_heightmap);
    void normalize();

};