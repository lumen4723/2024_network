fn main() {
    // String 42를 u32로 변환하고 에러가 뜨면 에러 처리를 한다
    // 아래 코드는 guess의 타입을 명시적으로 지정하지 않았기 때문에 에러가 발생한다
    // let guess = "42".parse().expect("Not a number!");

    // 타입을 명시적으로 지정하면 에러가 발생하지 않는다
    let guess: u32 = "42".parse().expect("Not a number!");



    let int8: i8 = 127; // 8비트 부호 있는 정수
    let int16: i16 = 32767; // 16비트 부호 있는 정수
    let int32: i32 = 2147483647; // 32비트 부호 있는 정수
    let int64: i64 = 9223372036854775807; // 64비트 부호 있는 정수
    let int128: i128 = 170141183460469231731687303715884105727; // 128비트 부호 있는 정수
    
    let uint8: u8 = 255; // 8비트 부호 없는 정수

    let float: f32 = 2.0; // 32비트 부호 있는 실수
    let double: f64 = 4.0; // 64비트 부호 있는 실수

    let _bool: bool = true; // 부울 값

    let _char: char = 'A'; // 유니코드 문자 값
    let string: &str = "asasd"; // 스트링 참조 값

    let tuple: (i32, f64, &str) = (1, 2.3, "45"); // 튜플 타입( 일명 곱타입 )

    

    let (x, y, z) = tuple; // 디스트럭쳐링

    println!("x = {x}, y = {y}, z = {z}");

    let z = tuple.0; // 또는 값에 직접 접근
    let x = tuple.1;
    let y = tuple.2;

    println!("x = {x}, y = {y}, z = {z}");

    let a = [1, 2, 3, 4, 5];
    let index = 10;

    let element = a[index];
    // C/C++은 Out Of Index가 발생하긴 하지만, 접근은 여전히 가능하여 문제가 커질 수 있다
    // Rust는 미리 size보다 큰지 검사한 뒤, Panic상태를 주고 즉시 크래시 종료가 실행된다

    println!("The value of element is: {}", element);
}