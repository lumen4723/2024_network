#include "Input.h"

#include "lib.h"
#include "Chara.h"

void processInput(GLFWwindow *window, Chara *player) {
    float speed = 0.005f;

    // 키보드 입력
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player->set_moveState(State::WALK);
        player->add_pos(0, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player->set_moveState(State::WALK);
        player->add_pos(0, -speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player->set_moveState(State::WALK);
        player->add_pos(speed, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player->set_moveState(State::WALK);
        player->add_pos(-speed, 0);
    }

    // 마우스 입력
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (player->get_actionState() == State::ATTACKABLE) {
            // 공격 상태로 전환
            player->set_actionState(State::ATTACKING);

            double mposX, mposY;
            glfwGetCursorPos(window, &mposX, &mposY);

            // 마우스 좌표를 윈도우 좌표로 변환
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            // mposX = (mposX - width / 2) / (width / 2);
            // mposY = (height / 2 - mposY) / (height / 1.125); // (height / width - height) == 1.125
            mposX = mposX / width * 2 - 1;
            mposY = (0.5 - mposY / height) * 1.125;
            double x = mposX - player->get_posX();
            double y = mposY - player->get_posY();

            // 명령 추가
            double sqrtdist = sqrt(x * x + y * y);
            player->set_attack_dir(x / sqrtdist, y / sqrtdist);
        }
    }

    if (
        glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE
        && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE
        && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE
        && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE
    ) {
        player->set_moveState(State::IDLE);
    }

    // ESC 키를 누르면 창을 닫는다
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
