use std::ops::Deref; // Deref 트레잇을 사용하기 위해 불러옴

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
    struct CustomSmartPointer { // 사용자 정의 스마트 포인터 구조체
        data: String,
    }
    
    impl Drop for CustomSmartPointer {
        fn drop(&mut self) { // Drop 트레잇을 구현한 스마트 포인터 구조체
            println!("Dropping CustomSmartPointer with data `{}`!", self.data);
        } // 스마트 포인터의 소유권이 끝날 때 수행할 동작을 정의함
    }

    let c = CustomSmartPointer {
        data: String::from("my stuff"),
    };
    let d = CustomSmartPointer {
        data: String::from("other stuff"),
    };
    println!("CustomSmartPointers created.");

    // c.drop(); // drop 메소드는 이처럼 직접 호출 할 수 있을 것 같지만, rust는 이를 허용하지 않음
    drop(c); // 대신 drop 함수를 사용해야 함, drop()은 스코프를 벗어나도 자동으로 호출됨
    // drop이 되면 스코프가 끝난 것으로 처리되어 값의 스코프가 다른 것과 맞지 않으면 에러가 발생함



    // 스마트 포인터 이전 개념 3. Rc<T> 타입: 하나의 값이 여러 소유자를 가질 수 있는 스마트 포인터
    // Rc<T>는 불변 참조 카운팅(Reference Counting)으로 C++에서도 익숙한 개념임

    use std::rc::Rc; // Rc 타입을 사용하기 위해 불러옴
    
    #[derive(Debug)]
    enum List1 {
        Cons(i32, Box<List1>),
        Nil,
    }
    #[derive(Debug)]
    enum List2 {
        Cons(i32, Rc<List2>),
        Nil,
    }

    let a = List1::Cons(5, Box::new(List1::Cons(10, Box::new(List1::Nil)))); // a의 소유권을 생성
    let b = List1::Cons(3, Box::new(a)); // a의 소유권을 b로 이동
    // let c = List1::Cons(4, Box::new(a)); // 이동된 소유권 a는 c로 이동할 수 없음

    // println!("a == {:?}", a); // a는 이동되어 사용할 수 없음
    println!("b == {:?}", b); // b는 이동된 소유권을 가지고 있음

    let a = Rc::new(List2::Cons(5, Rc::new(List2::Cons(10, Rc::new(List2::Nil))))); // a의 소유권을 생성
    let b = List2::Cons(3, Rc::clone(&a)); // a의 소유권을 b로 복제, 이 때 참조 카운트가 늘어남
    let c = List2::Cons(4, Rc::clone(&a)); // a의 소유권을 c로 복제, 이 때 참조 카운트가 늘어남

    println!("a == {:?}", a); // a는 여러 소유권을 가지고 있음
    println!("b == {:?}", b); // b는 a의 소유권을 가지고 있음
    println!("c == {:?}", c); // c는 a의 소유권을 가지고 있음



    let a = Rc::new(List2::Cons(5, Rc::new(List2::Cons(10, Rc::new(List2::Nil))))); // a의 소유권을 생성
    println!("a strong count 1 = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 기본값은 1

    let b = List2::Cons(3, Rc::clone(&a)); // a의 소유권을 b로 복제, 이 때 참조 카운트가 늘어남
    println!("a strong count 2 = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 2
    
    {
        let c = List2::Cons(4, Rc::clone(&a)); // a의 소유권을 c로 복제, 이 때 참조 카운트가 늘어남
        println!("a strong count 3 = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 3
    } // c는 라이프타임에 의해 소유권이 끝나면 참조 카운트가 줄어듦
    
    println!("a strong count 4 = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 2



    // 스마트 포인터 이전 개념 4. RefCell<T> 타입: 런타임에 불변 참조 규칙을 검사하는 스마트 포인터

    let x = 5;
    // let y = &mut x; // 불변 참조 규칙에 위배되어 에러가 발생함

    use std::cell::RefCell; // RefCell 타입을 사용하기 위해 불러옴

    let y = RefCell::new(x); // x를 RefCell로 래핑하고 y로 소유권을 이동함 (이는 safe한 동작임)

    // y의 소유권을 z로 이동 (이는 불변 값을 가변으로 복사가 아닌, 이동을 수행한 것으로 unsafe한 동작임)
    let mut z = y.borrow_mut();
    *z += 1; // z의 값을 변경함
    println!("z: {}", z); // z의 값을 출력함
    
    // let w = y.borrow_mut(); // z가 라이프타임에 의해 소유권이 끝나면 w의 소유권을 생성할 수 없음
    // RefCell은 런타임에 불변 참조 규칙을 검사하기 때문에 불변 참조 규칙에 위배되어 에러가 발생함
    
    // println!("w: {}", w); // w의 값을 출력함



    // Rc<T>와 RefCell<T>를 함께 사용하기

    #[derive(Debug)]
    enum List3 {
        Cons(Rc<RefCell<i32>>, Rc<List3>),
        Nil,
    }

    let value = Rc::new(RefCell::new(5)); // Rc<T>와 RefCell<T>를 함께 사용하기

    let a = Rc::new(List3::Cons(Rc::clone(&value), Rc::new(List3::Nil))); // value의 소유권을 a로 이동
    let b = List3::Cons(Rc::new(RefCell::new(3)), Rc::clone(&a)); // a의 소유권을 b로 RefCell로 래핑하고 이동
    let c = List3::Cons(Rc::new(RefCell::new(4)), Rc::clone(&a)); // a의 소유권을 c로 RefCell로 래핑하고 이동

    *value.borrow_mut() += 10; // value의 값을 변경함

    println!("a == {:?}", a); // a는 value의 소유권을 가지고 있음, value의 값이 변경되었으므로 a의 값도 변경됨
    println!("b == {:?}", b); // b는 a의 소유권을 가지고 있음, a의 값이 변경되었으므로 b의 값도 변경됨
    println!("c == {:?}", c); // c는 a의 소유권을 가지고 있음, a의 값이 변경되었으므로 c의 값도 변경됨



    // 순환 참조(Reference Cycle)를 해결하기 위한 방법: Weak<T> 타입을 사용하기
    #[derive(Debug)]
    enum List4 {
        Cons(i32, RefCell<Rc<List4>>),
        Nil,
    }

    impl List4 {
        fn tail(&self) -> Option<&RefCell<Rc<List4>>> {
            match self {
                List4::Cons(_, item) => Some(item),
                List4::Nil => None,
            }
        }
    }

    let a = Rc::new(List4::Cons(5, RefCell::new(Rc::new(List4::Nil)))); // a의 소유권을 생성

    println!("1. ref count a = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 1
    println!("2. tail a = {:?}", a.tail()); // a의 tail을 출력: Some(RefCell<Rc<List4>>)

    let b = Rc::new(List4::Cons(10, RefCell::new(Rc::clone(&a)))); // a의 소유권을 b로 복제, 이 때 a는 참조 카운트가 늘어남

    println!("3. ref count a = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 2
    println!("4. ref count b = {}", Rc::strong_count(&b)); // b의 참조 카운트를 출력: 1

    println!("5. tail b = {:?}", b.tail()); // b의 tail을 출력: Some(RefCell<Rc<List4>>)

    if let Some(link) = a.tail() { // link는 RefCell<Rc<List4>> 타입이므로
        *link.borrow_mut() = Rc::clone(&b); // link의 값을 b로 변경함
    } // 이렇게 되면 b의 tail은 a가 되고, a의 tail은 b가 되어 순환 참조가 발생함

    println!("6. ref count b = {}", Rc::strong_count(&b)); // b의 참조 카운트를 출력: 2
    println!("7. ref count a = {}", Rc::strong_count(&a)); // a의 참조 카운트를 출력: 2



    use std::rc::Weak;

    #[derive(Debug)]
    struct Node {
        value: i32,
        parent: RefCell<Weak<Node>>, // Weak<T> 타입을 사용하기
        // Weak<T>는 소유권을 가지지 않는 참조를 생성함 즉, 순환 참조를 방지하기 위해 사용함
        children: RefCell<Vec<Rc<Node>>>, // Rc<T> 타입을 사용하기
        // 고전적인 참조 카운팅인 Rc<T> 타입을 사용함
    }

    let leaf = Rc::new(Node { // leaf의 소유권을 생성
        value: 3,
        parent: RefCell::new(Weak::new()), // leaf는 부모를 가지고 있지 않으므로 Weak::new()로 초기화함
        children: RefCell::new(vec![]), // leaf는 자식을 가지고 있지 않으므로 빈 벡터로 초기화함
    });

    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade()); 
    // borrow()는 RefCell의 불변 참조를 반환하고, upgrade()는 Weak<T>를 Option<Rc<T>>로 업그레이드함
    // 즉, leaf의 부모를 Option<Rc<T>>로 변경해서 출력함

    let branch = Rc::new(Node { // branch의 소유권을 생성
        value: 5,
        parent: RefCell::new(Weak::new()), // branch는 부모를 가지고 있지 않으므로 Weak::new()로 초기화함
        children: RefCell::new(vec![Rc::clone(&leaf)]), // leaf의 소유권을 branch로 복제함
    });

    *leaf.parent.borrow_mut() = Rc::downgrade(&branch);
    // borrow_mut()는 RefCell의 가변 참조를 반환하고, downgrade()는 Rc<T>를 Weak<T>로 다운그레이드함
    // 즉, leaf의 부모를 branch, Weak<T>로 변경함

    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());



    let leaf = Rc::new(Node { // strong_count와 weak_count를 비교하기 위한 예제
        value: 3,
        parent: RefCell::new(Weak::new()),
        children: RefCell::new(vec![]),
    });

    println!( // 각 strong_count와 weak_count를 출력함
        "leaf strong = {}, weak = {}",
        Rc::strong_count(&leaf),
        Rc::weak_count(&leaf),
    );

    { // 내부 스코프를 생성함
        let branch = Rc::new(Node {
            value: 5,
            parent: RefCell::new(Weak::new()),
            children: RefCell::new(vec![Rc::clone(&leaf)]), // leaf의 소유권을 branch로 복제함
        });

        *leaf.parent.borrow_mut() = Rc::downgrade(&branch); // leaf의 부모를 branch, Weak<T>로 변경함

        println!( // branch는 strong_count: 0 -> 1, weak_count: 0 -> 1로 증가함
            "1. branch strong = {}, weak = {}",
            Rc::strong_count(&branch),
            Rc::weak_count(&branch),
        );

        println!( // leaf는 strong_count: 1 -> 2, weak_count: 0 -> 0로 증가함
            "2. leaf strong = {}, weak = {}",
            Rc::strong_count(&leaf),
            Rc::weak_count(&leaf),
        );
    } // branch의 소유권이 끝남

    // leaf의 부모를 Option<Rc<T>>로 변경해서 출력함
    println!("3. leaf parent = {:?}", leaf.parent.borrow().upgrade()); // branch의 소유권이 끝나면 leaf의 부모는 None이 됨
    
    println!( // leaf는 strong_count: 2 -> 1, weak_count: 0 -> 0로 감소함
        "4. leaf strong = {}, weak = {}",
        Rc::strong_count(&leaf),
        Rc::weak_count(&leaf),
    );

}
// Drop 트레잇에 의해 CustomSmartPointer d가 스코프를 벗어나면 drop 메소드가 호출됨 