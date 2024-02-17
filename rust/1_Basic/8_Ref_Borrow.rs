fn main() {
    fn cal_length(s: &String) -> usize { // s는 불변 참조로 소유권을 대여받음
        s.len() // 대여 받은 값을 사용하고 
    } // 스코프가 끝나면 대여는 자동으로 회수함

    let s1 = String::from("hello"); // s1은 스코프 내에서 불변으로 생성됨
    let len = cal_length(&s1); // 불변 참조로 인자를 넘김 == 소유권을 빌려줌
    // 값을 빌려줄 때는 해당 함수가 끝날때까지는 s1을 사용할 수는 없음

    // 다시 소유권을 되찾고 len 소유권은 새로 생겨서 둘 다 사용 가능
    println!("The length of '{}' is {}.", s1, len);



    fn add_world(s: &mut String) { // s는 가변 참조로 소유권을 대여받음
        s.push_str(" world!"); // 대여받은 값을 수정(사용)하고
    } // 스코프가 끝나면 대여는 자동으로 회수함

    let mut s1 = String::from("hello"); // s1은 스코프 내에서 가변으로 재 생성됨
    add_world(&mut s1); // s1을 가변 참조로 소유권을 빌려줌
    // 값을 빌려줄 때는 해당 함수가 끝날때까지는 s1을 사용할 수는 없음

    // 다시 소유권을 되찾고 s1은 수정 된 값을 가지고 있음
    println!("s1 = {}", s1);



    let s1 = String::from("hello"); // s1은 스코프 내에서 불변으로 재 생성됨
    let r1 = &s1; // 불변 참조는 복수 개로 대여가 가능함
    let r2 = &s1;

    println!("r1 = {}, r2 = {}", r1, r2); // 소유권을 대여받은 것으로 동시에 사용 가능



    let mut s1 = String::from("world"); // s1은 스코프 내에서 가변으로 재 생성됨
    let r1 = &mut s1; // 가변 참조는 복수 개로 대여가 불가능함
    // let r2 = &mut s1; // error

    println!("r1 = {}", r1); // 소유권을 대여받은 것으로 사용가능



    // 다만 아래의 방식으로는 복수 번 대여를 할 수는 있음
    let mut s1 = String::from("hello"); // s1은 스코프 내에서 가변으로 재 생성됨

    {
        let r1 = &mut s1; // 가변 참조를 한 번 받았음
        println!("r1 = {}", r1);
    } // 여기서 대여는 자동으로 회수가 됨

    let r2 = &mut s1; // 이후 회수된 소유권을 다시 빌려 받을 수 있음
    println!("r2 = {}", r2);



    let mut s1 = String::from("world"); // s1은 스코프 내에서 가변으로 재 생성됨

    let r1 = &s1; // 불변 참조를 수행하는 순간 복수 개의 불변 참조는 문제가 없지만 
    let r2 = &s1; // 그래서 문제 없음

    // let r3 = &mut s1; // 가변 참조는 하나라도 불변 참조가 있는 상태라면 할 수 없음
    // println!("r1 = {}, r2 = {}, r3 = {}", r1, r2, r3);
    // 역은 당연하게 하나의 가변 참조라도 있으면 가변/불변 모두 금지됨

    println!("r1 = {}, r2 = {}", r1, r2);



    // fn dangle_1() -> &String { // dangle_1 함수는 String 불변 참조를 반환함
    //     let s = String::from("hello"); // s가 스코프 내에서 생성

    //     &s // s를 불변 참조로 대여함
    // } // 하지만, s의 수명(life time)이 끝났기 때문에, dangling pointer가 생길 것임

    fn dangle_2() -> String { // dangle_2 함수는 String을 그대로 반환함
        let s = String::from("hello"); // s가 스코프 내에서 생성

        s // s의 소유권을 이동하여 넘김
    } // s의 수명(life time)은 끝나도 소유권 이동 반환으로 문제없음

    // let x = dangle_1(); // rust는 이런 dangling 문제를 컴파일 타임에서 잡는다
    let x = dangle_2(); // 소유권 이동 반환은 문제가 없음
    println!("x = {}", x);
}
