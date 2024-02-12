fn main () {
    { // s는 유효하지 않음: 아직 선언이 안됨
        let s = "hello"; // s는 이 지점부터 유효

        // s는 소유권이 소모되기까지 유효
    } // 이 스코프는 이제 끝이므로, s는 소유권 소모에 관계없이 유효하지 않음


    let mut s = String::from("hello");
    s.push_str(", world!"); // push_str()은 해당 스트링 리터럴을 스트링에 붙여줌
    println!("{}", s);

    let i1 = 1;
    let i2 = i1; // int32는 스택에 저장되는 타입으로 소유권 복사가 자유로움

    println!("i1 = {}, i2 = {}", i1, i2);



    let s1 = String::from("hello");
    let s2 = s1; // String 타입은 힙에 저장되는 타입으로 소유권이 이동되기 때문에 동시에 사용이 불가함
    // println!("s1 = {:?}, s2 = {:?}", s1, s2);

    println!("s2 = {}", s2); // 소유권이 있기 때문에 ok
    // println!("s1 = {:?}", s1); // 소유권이 완전히 이전되었기 때문에 error


    let s1 = String::from("world");
    let s2 = s1.clone(); // clone을 쓰면 소유권을 복사하여 사용하도록 명시함
    println!("s1 = {}, s2 = {}", s1, s2);



    let s = String::from("hello"); // s가 해당 스코프에서 생성됨

    takes_ownership(s); // s의 값이 함수 안으로 이동 == 스트링이므로 스코프가 이동됨

    // println!("s = {}", s); // 더이상 유효하지 않음

    let x = 5; // x가 해당 스코프에서 생성됨

    makes_copy(x); // x가 함수 안으로 복사 = 정수이므로 독립적인 스코프가 생성

    println!("x = {}", x); // 계속 사용 가능함



    let s1 = gives_ownership(); // some_string의 소유권을 받음

    println!("s1 = {}", s1);

    let s2 = String::from("world");
    let s3 = takes_and_gives_back(s2);

    // println!("s2 = {}", s2); // s2는 소유권이 없기 때문에 error
    println!("s3 = {}", s3);



    let s4 = String::from("Hello world"); // s4가 소유권을 가짐
    let (s5, len) = dont_takes_ownership(s4); // s5와 len의 각자 소유권을 s4, length로부터 받아옴

    // println!("s4 = {}", s4); // s4는 더 이상 소유권이 없기 때문에 error
    println!("s5 = {}, len = {}", s5, len); // 둘다 이후 사용 가능
}

fn takes_ownership(some_string: String) { // some_string이 스코프 안으로 진입(소유권 이동)
    println!("some_string = {}", some_string);
} // 여기서 some_string이 스코프를 벗어나 `drop`이 호출, 더 이상 유효하지 않게 됨

fn makes_copy(some_integer: i32) { // some_integer이 스코프 안으로 진입(소유권 복사)
    println!("some_integer = {}", some_integer);
} // 여기서 some_integer가 스코프 밖으로 벗어나 `drop`이 호출, 내부 값은 유효하지 않지만 외부 값은 무관 

fn gives_ownership() -> String { // gives_ownership 함수는 값을 반환
    let some_string = String::from("hello"); // some_string이 스코프 내에서 생성됨
    some_string // some_string이 반환되면, 소유권은 반환된 곳으로 이동
}

fn takes_and_gives_back(a_string: String) -> String { // a_string이 스코프 안으로 진입
    a_string  // a_string은 반환되면, 소유권은 반환된 곳으로 이동
}

// a_string의 소유권이 넘어가지 않고 작업을 하려면 튜플을 쓰면 된다
fn dont_takes_ownership(a_string: String) -> (String, usize) {
    let length = a_string.len(); // 작업을 했기 때문에 소유권 문제가 있을 수 있음

    (a_string, length) // 하지만 tuple타입으로 넘기면 소유권이 다시 이동하게됨
}