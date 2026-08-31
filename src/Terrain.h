#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include "Shader.h"

class Terrain {
public:
    unsigned int VAO, VBO, EBO;
    int width, height;
    float scale;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> heights;
    
    Terrain(int w, int h, float s) : width(w), height(h), scale(s) {
        setupMesh();
        generateMountains();
    }
    
    float noise(float x, float y) {
        int n = (int)(x * 57.0f + y * 131.0f);
        n = (n << 13) ^ n;
        return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    }
    
    float smoothNoise(float x, float y) {
        float corners = (noise(x-1, y-1) + noise(x+1, y-1) + noise(x-1, y+1) + noise(x+1, y+1)) / 16.0f;
        float sides = (noise(x-1, y) + noise(x+1, y) + noise(x, y-1) + noise(x, y+1)) / 8.0f;
        float center = noise(x, y) / 4.0f;
        return corners + sides + center;
    }
    
    float getHeight(float x, float y) {
        float total = 0;
        float frequency = 0.02f;
        float amplitude = 15.0f;
        float persistence = 0.5f;
        
        for (int i = 0; i < 4; i++) {
            total += smoothNoise(x * frequency, y * frequency) * amplitude;
            amplitude *= persistence;
            frequency *= 2.0f;
        }
        
        return total;
    }
    
    void generateMountains() {
        heights.clear();
        
        for (int z = 0; z < height; z++) {
            for (int x = 0; x < width; x++) {
                float worldX = x * scale;
                float worldZ = z * scale;
                float worldY = getHeight(worldX, worldZ);
                heights.push_back(worldY);
            }
        }
        
        updateMesh();
    }
    
    void setupMesh() {
        for (int z = 0; z < height; z++) {
            for (int x = 0; x < width; x++) {
                vertices.push_back(x * scale);
                vertices.push_back(0.0f);
                vertices.push_back(z * scale);
                
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
        }
        
        for (int z = 0; z < height - 1; z++) {
            for (int x = 0; x < width - 1; x++) {
                unsigned int topLeft = z * width + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (z + 1) * width + x;
                unsigned int bottomRight = bottomLeft + 1;
                
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    void updateMesh() {
        for (int i = 0; i < heights.size(); i++) {
            vertices[i * 6 + 1] = heights[i];
        }
        
        for (int z = 0; z < height; z++) {
            for (int x = 0; x < width; x++) {
                glm::vec3 normal(0.0f);
                
                if (x > 0 && x < width - 1 && z > 0 && z < height - 1) {
                    float hL = heights[z * width + (x - 1)];
                    float hR = heights[z * width + (x + 1)];
                    float hD = heights[(z + 1) * width + x];
                    float hU = heights[(z - 1) * width + x];
                    
                    normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hU - hD));
                }
                
                int index = (z * width + x) * 6;
                vertices[index + 3] = normal.x;
                vertices[index + 4] = normal.y;
                vertices[index + 5] = normal.z;
            }
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    }
    
    float getTerrainHeight(float worldX, float worldZ) {
        int x = (int)(worldX / scale);
        int z = (int)(worldZ / scale);
        
        if (x < 0 || x >= width - 1 || z < 0 || z >= height - 1)
            return 0.0f;
            
        return heights[z * width + x];
    }
    
    void Draw(Shader& shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};