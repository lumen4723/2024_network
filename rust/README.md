# rustup 설치법

-   rustup 은 rust에 관련된 컴파일러 rustc, Cargo 등의 다양한 툴들은 한 번에 관리하는 매니저

## unix(linux, macOS) 통합 방식

-   curl --proto '=https' --tlsv1.2 https://sh.rustup.rs/ -sSf | sh
-   rustup update

-   이러면 업데이트까지 완료가 된다

## windows 방식

-   https://win.rustup.rs/

-   받은 .exe를 실행하면
-   1번이 기본 값으로 받는 것으로 1번을 누르고 설치하면 된다

## 설치 확인 및 버전 확인

-   rustc --version
-   rustc 컴파일러 버전을 확인

-   cargo --version
-   cargo 프레임워크 버전을 확인

# Cargo 사용법

## Cargo 생성

-   Cargo init 을 하면 해당 위치에 Cargo 프로젝트 생성

## Cargo 실행

-   Cargo run 을 하면 /src 와 Cargo.toml을 가지고 /target 과 Cargo.lock을 생성 후 실행

-   그래서 git에 /src 와 Cargo.toml만 올리면 메모리를 적게 쓰고 업로드가 가능하다
-   잘 모르겠으면 ./gitignore를 참고
