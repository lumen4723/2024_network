#include "Chara.h"

#include "lib.h"

Chara::Chara(
    string name,
    int hp,
    float posX,
    float posY,
    uint32_t shaderProgram
) {
    this->name = name;
    this->hp = hp;
    this->posX = posX;
    this->posY = posY;
    this->shaderProgram = shaderProgram;
    move = State::IDLE;
    action = State::ATTACKABLE;
}

Chara::~Chara() {}

void Chara::set_name(string name) {
    this->name = name;
}

void Chara::set_hp(int hp) {
    this->hp = hp;
}

void Chara::set_posX(float posX) {
    this->posX = posX;
}

void Chara::set_posY(float posY) {
    this->posY = posY;
}

string Chara::get_name() {
    return name;
}

int Chara::get_hp() {
    return hp;
}

float Chara::get_posX() {
    return posX;
}

float Chara::get_posY() {
    return posY;
}

void Chara::add_pos(float x, float y) {
    this->posX += x;
    this->posY += y;
}

uint32_t Chara::get_shaderProgram() {
    return shaderProgram;
}

void Chara::set_moveState(State::move move) {
    this->move = move;
}

void Chara::set_actionState(State::action action) {
    this->action = action;
}

State::move Chara::get_moveState() {
    return move;
}

State::action Chara::get_actionState() {
    return action;
}

void Chara::set_attack_dir(float x, float y) {
    attack_dir = make_pair(x, y);
}

pair<float, float> Chara::get_attack_dir() {
    return attack_dir;
}

void Chara::damaged(int damage) {
    hp -= damage;
}