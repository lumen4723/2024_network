#include "lib.h"

#include "Tools.h"
#include "Input.h"
#include "Chara.h"
#include "Bullet.h"
#include "Shader.h"

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

void Init_Comp(vector<Chara> &players, vector<Bullet> &bullets, Shader &shader);
void Update_Comp(vector<Chara> &players, vector<Bullet> &bullets, Shader &shader);

int main() {
    GLFWwindow* window = init_glfw(WIDTH, HEIGHT, "Minu GL");

    Shader shader = Shader();

    vector<uint32_t> VBOs, VAOs, EBOs;

    // 배경색
    glClearColor(0.4f, 0.4f, 0.4f, 0.4f);

    // 최적화는 추가로 지원 안해서, 혹시 멀티쓰레드 기반 GL를 쓰고 싶으면 개인이 추가로 구현할 것
    // 일반적으로 Init_Comp와 Update_Comp 함수를 수정하는 방식으로 프로젝트를 진행하면 됨
    vector<Chara> players;
    vector<Bullet> bullets;

    // 컴포넌트 초기화 로직 함수
    Init_Comp(players, bullets, shader);


    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window, &players[0]);


        // 구현할 업데이트 로직 함수
        Update_Comp(players, bullets, shader);


        // 그리기
        // 함수 사용 규칙은 players와 bullets에 값을 정상적으로 넣으면 두 객체를 그릴 수 있게 됨
        make_Vertex_Attrs_And_Draw(
            VAOs, VBOs, EBOs,
            players.size(), bullets.size(),
            players, bullets
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteBuffers(EBOs.size(), EBOs.data());
    // glDeleteBuffers(VBOs.size(), VBOs.data());
    // glDeleteVertexArrays(VAOs.size(), VAOs.data());

    // shader.~Shader();

    // glfwDestroyWindow(window);
    // glfwTerminate();
    return 0;
}


void Init_Comp(vector<Chara> &players, vector<Bullet> &bullets, Shader &shader) {
    // 초기화 로직

    // 내 첫 위치는 서버로부터 받아온 값으로 설정할 것
    players.push_back(Chara("Me", 100, rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, shader.get_shaderProgram("Red")));

    // 상대는 아래 반복문 내에서 소켓을 통해 동적으로 받아올 것
    players.push_back(Chara("Enemy1", 100, rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, shader.get_shaderProgram("Blue")));
    players.push_back(Chara("Enemy2", 100, rand() % 100 / 100.0f - 0.5f, rand() % 100 / 100.0f - 0.5f, shader.get_shaderProgram("Green")));
}


void Update_Comp(vector<Chara> &players, vector<Bullet> &bullets, Shader &shader) {
    // 메인 업데이트 로직

    // 상대의 실시간 정보는 소켓을 통해 받아올 것
    // players[i].add_pos(readsock...x, readsock...y);

    // for (int i = 1; i < players.size(); i++) {
    //     players[i].add_pos(rand() % 10 / 100.0f - 0.2f, rand() % 10 / 100.0f - 0.2f);
    // }

    // 로직 구현
    for (int i = 0; i < players.size(); i++) {
        if (players[i].get_hp() <= 0) {
            cout << players[i].get_name() << " is dead" << endl;
            players.erase(players.begin() + i--);
            continue;
        }
        else if (players[i].get_actionState() == State::action::ATTACKING) {
            bullets.push_back(Bullet(
                players[i].get_posX(),
                players[i].get_posY(),
                0.01f,
                players[i].get_attack_dir().first,
                players[i].get_attack_dir().second,
                shader.get_shaderProgram("White"),
                players[i].get_name()
            ));

            players[i].set_actionState(State::action::ATTACKDELAY);
            // 비동기 타이머를 통해 1초 후에 다시 ATTACKABLE로 변경
            thread t([](Chara *player) {
                this_thread::sleep_for(1s);
                player->set_actionState(State::action::ATTACKABLE);
            }, &players[i]);

            t.detach();
        }
    }

    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].move();

        if (
            bullets[i].get_posX() < -2.0f
            || bullets[i].get_posX() > 2.0f
            || bullets[i].get_posY() < -1.125f
            || bullets[i].get_posY() > 1.125f
        ) {
            bullets.erase(bullets.begin() + i--);
            continue;
        }
        
        for (int j = 0; j < players.size(); j++) {
            if (
                bullets[i].get_shooterName() != players[j].get_name() // 자신이 쏜거에 대한 충돌은 무시
                && Check_Collision_CB(players[j], bullets[i])  // 충돌 검사, Chara, Bullet 순서임
            ) {
                players[j].damaged(10);
                if (players[j].get_hp() <= 0) {
                    cout << players[j].get_name() << " is dead" << endl;
                    players.erase(players.begin() + j--);
                }

                bullets.erase(bullets.begin() + i--);
                break;
            }
        }
    }
}