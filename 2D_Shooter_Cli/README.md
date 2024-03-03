# 2D Shooter Client 프로그램

## 프로그램 설명

-   정사각형 폴리곤으로 구성된 2D 탄막 슈팅 게임
-   그래픽 엔진은 OpenGL을 사용 (GLAD, GLFW, GLM)
-   탄막을 맞은 상대는 체력이 깎이고 0 이하가 되면 소멸
-   캐릭터 이동 및 탄막 소멸 등의 최적화 추가

-   Vulkan 버전으로 포팅한 코드를 보고 싶다면 아래로 이동

    -   https://github.com/lumen4723/3D_Basic/tree/2DShooter

-   현재 window에서만 지원
-   싱글 쓰레드 기반 프로그램
