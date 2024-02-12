fn main() {
    // 변수는 기본적으로 불변이다
    let x = 5;
    println!("1_ x = {x}");
    // x = 6; // x는 불변이므로 error
    // println!("1_ x = {}", x);

    let a: Vec<i32> = Vec::new(); // 불변 동적 리스트
    // a.push(1); // a는 불변이므로 error

    let mut b: Vec<i32> = Vec::new(); // 가변 동적 리스트
    b.push(1); // b는 가변이므로 ok
    b.push(2);

    println!("2_ b = {:?}", b);


    // let은 불변이지만, 변수의 값이 기본 데이터 타입(스택에 저장되는 타입)일 경우 복사, 빌림이 가능하다
    let x = 1;
    // let mut은 가변이고, 처음 자신이 소유권을 가진다
    let mut y = 2;

    // 소유권을 복사 (복사가 일어나므로 x는 여전히 독립적인 소유권을 가진다)
    // 복사는 빌린 그 시점의 값을 가지고 새로운 변수를 만드는 것이다
    let z = x;
    println!("2_ z = {}, x = {}", z, x); // x는 기본 데이터 타입이므로 복사가 일어나서 가능하다

    // 소유권을 복사
    let w = y;
    y = 3; // 복사는 둘의 소유권이 독립적이라 다른 값이 된다
    println!("3_ w = {}, y = {}", w, y); 

    // 소유권을 복사
    let mut v = y;
    y = 5; // 복사가 일어나므로 y의 값을 변경할 수 있다
    v = 4; // 복사가 일어나므로 v의 값을 변경할 수 있다
    println!("4_ v = {}, y = {}", v, y); // 복사가 일어나므로 y와 v는 독립적이다



    // 소유권을 빌림: 불변 참조
    let a = &y;
    // y = 4; // 값을 빌려줬으므로 y의 값을 직접 변경할 수 없다
    // *a = 4; // 불변 참조이므로 error
    println!("5_ y = {}, a = {}", y, a); // 불변 참조는 참조된 값을 읽는 것만 허용된다


    // 소유권을 빌려준다: 가변 참조
    let b = &mut y;
    // y = 5; // 값을 빌려줬으므로 y의 값을 직접 변경할 수 없다
    *b = 5; // 가변 참조이므로 ok
    
    // b를 통해 y는 가변 참조가 존재하는 동안 두 값을 동시에 불변 참조를 할 수 없다
    // println!("6_ y = {}, b = {}", y, b);

    println!("7_ b = {}", b); // 대신 빌린 값은 불변 참조로 여러번 읽을 수 있다
    *b = 6; // 가변 참조이므로 ok, 가변참조도 여러번 할 수 있다
    println!("7_ b = {}", b);

    // 하지만, y가 불변 참조를 하면 대여는 끝난다
    println!("7_ y = {}", y);

    // *b = 7; // 대여가 끝났으므로 error
    // println!("7_ b = {}", b); // 대여가 끝났으므로 error


    

    let spaces = "   "; // 묵시적으로 타입은 &str이다
    // spaces = spaces.len(); // 묵시적으로 타입은 usize이므로 타입이 바뀐다 -> 타입 변경은 불가능하다
    let spaces = spaces.len(); // 타입의 변경은 안되지만, 다른 타입의 같은 이름 새로운 변수로 할당하는 거라면 ok
    println!("8_ spaces = {}", spaces);

    
    let mut spaces = "   "; // 묵시적으로 타입은 &str이다

    // spaces = spaces.len(); // 타입의 변경은 기본적으로 불가능 error
    // spaces = spaces.len().to_string(); // &str과 String은 다른 타입이므로 error

    // spaces = &spaces.len().to_string(); // 이렇게 하면 타입이 같으므로 ok
    // println!("9_ spaces = {}", spaces); // 하지만 소유권이 없으므로 spaces는 더 이상 사용할 수 없다

    let binding = spaces.len().to_string(); // 소유권을 가지므로 ok
    spaces = &binding; // 소유권을 빌려주므로 ok
    println!("9_ spaces = {}", spaces); // spaces는 binding에 대여중으로 출력이 가능해진다
}