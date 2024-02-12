struct User { // struct 도 tuple처럼 곱 타입을 기본으로 사용
    username: String,
    email: String,
    sign_in_count: u64,
    active: bool,
}

struct Color(i32, i32, i32); // tuple struct
struct Point(i32, i32, i32); // 튜플도 구조체화 가능

// struct User_ptr {
//     username: &str, // 스트럭트의 참조는 라이프타임을 명시해야함
//     email: &str,
//     sign_in_count: u64,
//     active: bool,
// }


fn main() {
    let user1 = User {
        email: String::from("1234@example.com"),
        username: String::from("someusername123"),
        active: true,
        sign_in_count: 1,
    };
    
    // user1.email = String::from("4321@example.com");
    // 내부의 값을 변경 가능 한 보통의 언어와 달리, rust는 불변으로 잡힌 모든 범위가 변경 불가능함
    
    println!("user1.email = {}, user1.username = {}", user1.email, user1.username);

    let mut user2 = User {
        email: String::from("1234@example.com"),
        username: String::from("someusername321"),
        active: true,
        sign_in_count: 1,
    };

    user2.email = String::from("4321@example.com"); // 가변으로 정의했기 때문에 변경 가능함
    println!("user2.email = {}, user2.username = {}", user2.email, user2.username);

    let user3 = build_user(String::from("1234@example.com"), String::from("someusername"));
    println!("user3.email = {}, user3.username = {}", user3.email, user3.username);


    let user4 = User {
        email: String::from("4321@example.com"),
        username: String::from("someusername"),
        ..user3 // 나머지는 user3에서 가져옴
    };

    println!("user4.active is equal to user3.active? {}", user4.active == user3.active);
    println!("user4.sic is equal to user3.sic? {}", user4.sign_in_count == user3.sign_in_count);



    // let user5 = User_ptr {
    //     email: "someone@example.com", // 타입은 맞지만 라이프타임을 명시하지 않아서 에러
    //     username: "someusername123", // 라이프타임을 명시하지 않아서 에러
    //     active: true,
    //     sign_in_count: 1,
    // };
    // println!("user5.email = {}, user5.username = {}", user5.email, user5.username);



    let black = Color(0, 0, 0);
    let origin = Point(0, 0, 0);
    println!(
        "black = ({}, {}, {}), origin = ({}, {}, {})",
        black.0, black.1, black.2,
        origin.0, origin.1, origin.2
    );
}

fn build_user(email: String, username: String) -> User { // 이렇게 함수를 통해 생성하는 것도 가능함
    User {
        email: email, // 변수와 필드의 이름이 같다면 이렇게 줄일 수 있음
        username, // 이렇게 줄일 수 있음
        active: true,
        sign_in_count: 1,
    } // 소유권 이동 반환
}