#pragma once

#include "lib.h"

class Bullet {
private:
    float startX;
    float startY;
    float posX;
    float posY;
    float speed;
    float dirX;
    float dirY;
    uint32_t shaderProgram;
    string shooterName;

public:
    Bullet(
        float posX = 0,
        float posY = 0,
        float speed = 0,
        float dirX = 0,
        float dirY = 0,
        uint32_t shaderProgram = 0,
        string shooterName = "default"
    );
    ~Bullet();

    void set_posX(float posX);
    void set_posY(float posY);
    void set_speed(float speed);
    void set_dirX(float dirX);
    void set_dirY(float dirY);

    float get_posX();
    float get_posY();
    float get_speed();
    float get_dirX();
    float get_dirY();
    uint32_t get_shaderProgram();
    string get_shooterName();

    void move();

    // float totalmoved();
};