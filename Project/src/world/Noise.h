#pragma once
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include "Chunk.h"

class Noise {
public:
    Noise(unsigned int seed = 42) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end());
    }

    void GenerateChunk(Chunk& chunk) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                // Global block coordinates
                float gx = (float)(chunk.chunkPos.x * CHUNK_SIZE + x);
                float gz = (float)(chunk.chunkPos.z * CHUNK_SIZE + z);
                
                float noiseVal = FBM(gx * 0.05f, gz * 0.05f);
                int height = (int)(32 + noiseVal * 15);

                for (int y = 0; y < CHUNK_SIZE; y++) {
                    int gy = chunk.chunkPos.y * CHUNK_SIZE + y;
                    if (gy < height - 3)      chunk.SetBlock(x, y, z, BLOCK_STONE);
                    else if (gy < height)     chunk.SetBlock(x, y, z, BLOCK_DIRT);
                    else if (gy == height)    chunk.SetBlock(x, y, z, BLOCK_GRASS);
                    else                      chunk.SetBlock(x, y, z, BLOCK_AIR);
                }
            }
        }
    }

    float Perlin2D(float x, float y) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        x -= std::floor(x); y -= std::floor(y);
        float u = fade(x), v = fade(y);
        int aa = p[p[X]+Y], ab = p[p[X]+Y+1], ba = p[p[X+1]+Y], bb = p[p[X+1]+Y+1];
        return lerp(v, lerp(u, grad(aa, x, y), grad(ba, x-1, y)),
                       lerp(u, grad(ab, x, y-1), grad(bb, x-1, y-1)));
    }

    float FBM(float x, float y) const {
        float v = 0, amp = 0.5f, freq = 1.0f;
        for(int i=0; i<4; i++) { v += amp * Perlin2D(x*freq, y*freq); amp *= 0.5f; freq *= 2.0f; }
        return v;
    }

private:
    std::vector<int> p;
    static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    static float lerp(float t, float a, float b) { return a + t * (b - a); }
    static float grad(int hash, float x, float y) {
        int h = hash & 3;
        float u = h < 2 ? x : y, v = h < 2 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }
};
