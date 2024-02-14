fn main() {
    let x = 5;

    func1(x);

    let y = {
        let x = 3; // 해당 스코프 내에서는 내부 x가 더 높은 우선권을 가진다
        x + 1 // 3 + 1 이 들어감
    };

    let z = x + 1; // 5 + 1 이 들어감

    println!("y = {}, z = {}", y, z);

    let w = add1(y, z);
    println!("w = {}", w);

    let v = add2(w, z);
    println!("v = {}", v);
}

fn func1(x: i32) {
    println!("x = {}", x);
}

fn add1(x: i32, y:i32) -> i32 { // i32, i32를 받으면 i32를 돌려주는 함수
    return x + y;
}

fn add2(x:i32, y:i32) -> i32 {
    x + y // 이것도 가능하다, 다만 세미콜론이 없어야 리턴 값으로 인정된다
}