#include "lib.h"

#include "Tools.h"
#include "Input.h"
#include "Chara.h"
#include "Bullet.h"
#include "Shader.h"

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

int main() {
    GLFWwindow* window = init_glfw(WIDTH, HEIGHT, "Minu GL");

    Shader shader = Shader();

    vector<uint32_t> VBOs, VAOs, EBOs;

    vector<Chara> players;
    vector<Bullet> bullets;

    // 내 첫 위치는 서버로부터 받아온 값으로 설정할 것
    players.push_back(Chara("Me", 100, rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, shader.get_shaderProgram("Red")));

    // 상대는 아래 반복문 내에서 소켓을 통해 동적으로 받아올 것
    players.push_back(Chara("Enemy1", 100, rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, shader.get_shaderProgram("Blue")));
    players.push_back(Chara("Enemy2", 100, rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, shader.get_shaderProgram("Green")));

    // 배경색
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window, &players[0]);

        // 상대의 실시간 정보는 소켓을 통해 받아올 것
        // players[i].add_pos(readsock...x, readsock...y);

        // for (int i = 1; i < players.size(); i++) {
        //     players[i].add_pos(rand() % 10 / 100.0f - 0.2f, rand() % 10 / 100.0f - 0.2f);
        // }

        vector<vector<float>> vertices;
        vector<vector<uint32_t>> indices;
        float posX, posY;

        for (int i = players.size() - 1; i >= 0; i--) {
            if (players[i].get_hp() <= 0) {
                players.erase(players.begin() + i);
                continue;
            }
            else if (players[i].get_actionState() == State::action::ATTACKING) {
                bullets.push_back(Bullet(
                    players[i].get_posX(),
                    players[i].get_posY(),
                    0.01f,
                    players[i].get_attack_dir().first,
                    players[i].get_attack_dir().second,
                    shader.get_shaderProgram("White")
                ));

                players[i].set_actionState(State::action::ATTACKDELAY);
                // 비동기 타이머를 통해 1초 후에 다시 ATTACKABLE로 변경
                thread t([](Chara *player) {
                    this_thread::sleep_for(1s);
                    player->set_actionState(State::action::ATTACKABLE);
                }, &players[i]);

                t.detach();
            }

            posX = players[i].get_posX();
            posY = players[i].get_posY();

            vertices.push_back({
                -0.05f + posX, -0.05f + posY, 0.0f,
                0.05f + posX, -0.05f + posY, 0.0f,
                0.05f + posX, 0.05f + posY, 0.0f,
                -0.05f + posX, 0.05f + posY, 0.0f
            });
            indices.push_back({
                0, 1, 3,
                1, 2, 3
            });
        }

        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].move();

            posX = bullets[i].get_posX();
            posY = bullets[i].get_posY();

            

            // 팔각형 하고싶다
            vertices.push_back({
                -0.02f + posX, -0.02f + posY, 0.0f,
                0.02f + posX, -0.02f + posY, 0.0f,
                0.02f + posX, 0.02f + posY, 0.0f,
                -0.02f + posX, 0.02f + posY, 0.0f
            });
            indices.push_back({
                0, 1, 3,
                1, 2, 3
            });
        }

        uint32_t size = players.size();
        size += bullets.size();
        VAOs.resize(size);
        VBOs.resize(size);
        EBOs.resize(size);

        make_Vertex_Attribs(
            VAOs, VBOs, EBOs,
            players.size(), bullets.size(), players, bullets,
            vertices, indices
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteBuffers(EBOs.size(), EBOs.data());

    shader.~Shader();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
