#pragma once

#include "lib.h"
#include "Chara.h"
#include "Bullet.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLFWwindow* init_glfw(
    const uint32_t WIDTH = 1600,
    const uint32_t HEIGHT = 900,
    const string title = "lumen GL"
);

void make_Vertex_Attribs (
    vector<uint32_t> &VAOs,
    vector<uint32_t> &VBOs,
    vector<uint32_t> &EBOs,
    uint32_t psize,
    uint32_t bsize,
    vector<Chara> players,
    vector<Bullet> bullets,
    vector<vector<float>> vertices,
    vector<vector<uint32_t>> indices
);
