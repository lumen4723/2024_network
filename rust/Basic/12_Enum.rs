fn main() {
    #[derive(Debug)]
    enum IpAddrKind { // enum은 합타입
        V4,
        V6,
    }

    fn route(ip_type: IpAddrKind) {
        println!("ip_type = {:?}", ip_type);
    }

    let four = IpAddrKind::V4;
    let six = IpAddrKind::V6;

    route(IpAddrKind::V4);
    route(IpAddrKind::V6);



    struct IpAddr_1 {
        kind: IpAddrKind,
        address: String,
    }

    let home = IpAddr_1 {
        kind: IpAddrKind::V4,
        address: String::from("127.0.0.1"),
    };
    println!("home = {}", home.address);
    
    let loopback = IpAddr_1 {
        kind: IpAddrKind::V6,
        address: String::from("::1"),
    };
    println!("loopback = {}", loopback.address);



    #[derive(Debug)]
    enum IpAddr_2 {
        V4(String),
        V6(String),
    }

    let home = IpAddr_2::V4(String::from("127.0.0.1"));
    let loopback = IpAddr_2::V6(String::from("::1"));
    println!("home = {:?}", home); // {:?}는 디버깅 출력 포맷으로
    println!("loopback = {:?}", loopback); // enum은 #[derive(Debug)]가 필요함



    #[derive(Debug)]
    enum IpAddr_3 { // enum은 각 케이스가 다른 타입일 수 있음
        V4(u8, u8, u8, u8),
        V6(String),
    }

    let home = IpAddr_3::V4(127, 0, 0, 1);
    let loopback = IpAddr_3::V6(String::from("::1"));
    println!("home = {:?}", home);
    println!("loopback = {:?}", loopback);



    // enum및 구조체 함수 내부에서 구현 가능
    #[derive(Debug)]
    enum Message { // 다양한 케이스를 가지는 enum
        Quit,
        Move { x: i32, y: i32 },
        Write(String),
        ChangeColor(i32, i32, i32),
    }

    #[derive(Debug)]
    struct QuitMessage; // 유닛 구조체: 자신만을 가지는 구조체

    #[derive(Debug)]
    struct MoveMessage { // 튜플 구조체(곱 타입)
        x: i32,
        y: i32,
    }

    #[derive(Debug)]
    struct WriteMessage(String);

    #[derive(Debug)]
    struct ChangeColorMessage(i32, i32, i32); // 튜플 구조체

    let m = Message::Write(String::from("hello"));
    let q = Message::Quit;
    let mv = Message::Move { x: 1, y: 2 };
    let c = Message::ChangeColor(0, 0, 0);
    println!("m = {:?}", m);
    println!("q = {:?}", q);
    println!("mv = {:?}", mv);
    println!("c = {:?}", c);



    #[derive(Debug)]
    enum Optional<T> { // 제네릭 enum, Optional<T>는 Some(T)와 None 두 가지 케이스를 가짐(Option<T>와 같음)
        Some(T),
        None,
    }

    let some_number = Optional::Some(5); // Optional::Some(5)는 Optional<i32> 타입
    let some_string = Optional::Some("a string"); // Optional::Some("a string")은 Optional<&str> 타입

    let absent_number: Optional<i32> = Optional::None; // Optional::None은 NULL과 같음



    let x: i8 = 5;
    let y: Optional<i8> = Optional::Some(5);

    // let sum = x + y; // 에러: i8와 Optional<i8>은 다른 타입이므로 연산 불가
}