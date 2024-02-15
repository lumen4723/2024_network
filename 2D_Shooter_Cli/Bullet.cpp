#include "Bullet.h"

Bullet::Bullet(
    float posX,
    float posY,
    float speed,
    float dirX,
    float dirY,
    uint32_t shaderProgram
) {
    this->startX = posX;
    this->startY = posY;
    this->posX = posX;
    this->posY = posY;
    this->speed = speed;
    this->dirX = dirX;
    this->dirY = dirY;
    this->shaderProgram = shaderProgram;
}

Bullet::~Bullet() {
    // cout << "Bullet is gone." << endl;
}

void Bullet::set_posX(float posX) {
    this->posX = posX;
}

void Bullet::set_posY(float posY) {
    this->posY = posY;
}

void Bullet::set_speed(float speed) {
    this->speed = speed;
}

void Bullet::set_dirX(float dirX) {
    this->dirX = dirX;
}

void Bullet::set_dirY(float dirY) {
    this->dirY = dirY;
}

float Bullet::get_posX() {
    return posX;
}

float Bullet::get_posY() {
    return posY;
}

float Bullet::get_speed() {
    return speed;
}

float Bullet::get_dirX() {
    return dirX;
}

float Bullet::get_dirY() {
    return dirY;
}

uint32_t Bullet::get_shaderProgram() {
    return shaderProgram;
}

void Bullet::move() {
    this->posX += this->dirX * this->speed;
    this->posY += this->dirY * this->speed;
}

float Bullet::totalmoved() {
    return sqrt(
        pow(this->posX - this->startX, 2) +
        pow(this->posY - this->startY, 2)
    );
}