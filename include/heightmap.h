#pragma once
#include <stdint.h>
#include <string>
#include <fstream>
static int nullblock = 512;
class Heightmap {
    uint32_t sideLength = 256;
    uint32_t size = sideLength * sideLength;
    int averageBlockHeight = 0;
    int *map = nullptr;

   public:
    Heightmap(uint32_t p_sideLength);
    ~Heightmap();
    void generate();

    void convolve(uint32_t kwid);
    void slowNoise(const float sloperange, int count, const int sizeOffset,
                   const int sizeRange);
    inline int *getHeightmapPtr(int x, int z) { return getHeightmapPtr(x, z, map); }
    inline int *getHeightmapPtr(int x, int z, int *p_heightmap)
    {
        if (x < 0 || x >= sideLength || z < 0 || z >= sideLength){
            nullblock = 256;
            return &nullblock;
        }

        const int index = x + z * sideLength;
        return &p_heightmap[index];
    }
    void normalize();
    void writeToFile(const std::string &fname);
    void readFromFile(const std::string &fname);
};