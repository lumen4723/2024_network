use std::ops::Deref;

fn main() {
    // 스마트 포인터 이전 개념 1. Deref 트레잇: 스마트포인터(박스)의 *동작을 기본 역참조처럼 사용할 수 있게 해줌
    let x = 1;
    let y = &x; // 불변 참조
    let z = Box::new(x); // 박스를 통한 소유권 이전

    println!("x: {}", x); // 직접적으로 변수를 사용

    print!("y: {} ", y); // 참조를 통한 변수 사용
    print!("y: {} ", &y); // 참조를 통한 참조 사용
    println!("y: {}", *y); // 역참조를 통한 직접 변수 사용

    print!("z: {} ", z); // 박스를 통한 변수 사용
    print!("z: {} ", &z); // 박스를 통한 참조 사용
    println!("z: {}", *z); // 역참조를 통한 직접 변수 사용



    // 표준 Box<T> 타입을 직접 구현해보기
    struct MyBox<T>(T); // 단일 타입 튜플 구조체

    impl<T> MyBox<T> { // MyBox 구조체에 대한 메소드 구현
        fn new(x: T) -> MyBox<T> { // 생성자
            MyBox(x)
        }
    }

    let x = 2;
    let y = MyBox::new(x);
    println!("y: {}", y.0); // 위의 코드를 참고하면 2가 출력됨을 알 수 있음
    // println!("y: {}", *y); // 박스 컨테이너에서 변수를 직접적인 사용을 수행하려면 역참조를 사용해야 함
    // 아쉽게도, 역참조를 사용하려고 하면 에러가 발생함



    impl<T> std::ops::Deref for MyBox<T> { // Deref 트레잇을 받아서 역참조를 받는 메소드 구현
        type Target = T; // 연관 타입: Deref 트레잇을 구현하는 타입이 어떤 타입을 역참조할지 명시함
    
        fn deref(&self) -> &T { // 역참조를 하면 기본적으로 사용될 메소드
            &self.0
        }
    }

    let x = MyBox::new(String::from("Rust"));
    println!("x: {}", x.0); // x의 내부 변수를 직접적으로 사용
    println!("x: {}", *x); // 역참조를 통한 직접적인 사용
    println!("x: {}", *(x.deref())); // 역참조를 사용하면 Deref 트레잇의 deref 메소드가 내부적으로 호출됨
    // rust는 *x == *(x.deref()) == *(&x.0) == x.0 로 처리함
    // 그래서 위 세 줄은 모두 같은 행동을 수행함



    fn hello(name: &str) { // 인자를 소유권 불변 참조로 받음
        println!("Hello, {}!", name);
    }

    let m = MyBox::new(String::from("Rust"));

    hello(&m); // 이는 원래 불가능한 동작이다 (&MyBox<String>를 &str에 넣은 것이기 때문에)
    // 하지만 동작하는 이유는 Deref 트레잇이 구현된 구조체이기 때문에
    // rust는 역참조 강제 변환(Deref coercion) 규칙을 따라서
    // &MyBox<String> -> &String -> &str 로 변환함
    // 즉, &m == &(*m) == &(*m.deref()) == &(*(&m.0)) == &m.0 로 동작함

    // rust는 &m (m은 Deref 트레잇을 구현한 구조체) -> &(*m) (역 참조된 문자열 슬라이스)
    // -> &(*m)[..] (역 참조된 문자열의 전역 슬라이스(&str)) 로 변환함
    hello(&(*m)[..]); // 즉, &(*m)[..] == &(*m) == &m 로 동작함

    // 결론은 rust는 Deref 트레잇을 구현한 구조체에 한해 역참조 강제 변환을 통해 코드를 간결하게 만들 수 있음



    // 역참조 강제 변환(Deref coercion) 규칙은 다음 3가지 규칙을 따름
    // 1. &T -> &U 일 때, T: Deref<Target=U> 이면 &T -> &*T 로 변환됨
    // 2. &mut T -> &mut U 일 때, T: DerefMut<Target=U> 이면 &mut T -> &mut *T 로 변환됨
    // 3. &mut T -> &U 일 때, T: Deref<Target=U> 이면 &mut T -> &*T 로 변환됨 (단, 역은 성립하지 않음)



    // 스마트 포인터 이전 개념 2. Drop 트레잇: 스마트포인터(박스)의 소유권이 끝날 때 수행할 동작을 정의함
    struct CustomSmartPointer {
        data: String,
    }
    
    impl Drop for CustomSmartPointer {
        fn drop(&mut self) {
            println!("Dropping CustomSmartPointer with data `{}`!", self.data);
        }
    }

    let c = CustomSmartPointer {
        data: String::from("my stuff"),
    };
    let d = CustomSmartPointer {
        data: String::from("other stuff"),
    };
    println!("CustomSmartPointers created.");



    //...
}