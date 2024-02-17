#pragma once

#include "lib.h"

struct State {
    enum move {
        IDLE,
        WALK,
        RUN,
    };
    enum action {
        ATTACKABLE,
        ATTACKING,
        ATTACKDELAY,
    };
};

class Chara {
private:
    string name;
    int hp;

    uint32_t shaderProgram;

    float posX;
    float posY;

    State::move move;
    State::action action;
    pair<float, float> attack_dir;

public:
    Chara(
        string name = "default",
        int hp = 100,
        float posX = 0,
        float posY = 0,
        uint32_t shaderProgram = 0
    );
    ~Chara();

    void set_name(string name);
    void set_hp(int hp);
    void set_posX(float posX);
    void set_posY(float posY);

    void add_pos(float x, float y);

    string get_name();
    int get_hp();
    float get_posX();
    float get_posY();
    uint32_t get_shaderProgram();

    void set_moveState(State::move move);
    void set_actionState(State::action action);

    State::move get_moveState();
    State::action get_actionState();

    void set_attack_dir(float x, float y);
    pair<float, float> get_attack_dir();

    void damaged(int damage);
};
