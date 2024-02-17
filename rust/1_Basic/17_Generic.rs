fn main() {
    let numbers = vec![34, 50, 25, 100, 65];

    let result = largest_i32(&numbers);
    println!("The largest number is {}", result);

    let chars = vec!['y', 'm', 'a', 'q'];

    let result = largest_char(&chars);
    println!("The largest char is {}", result);



    let numbers = vec![34, 50, 25, 100, 65];

    // let result = largest_1(&numbers); // 이는 에러가 난다
    // 이유는 제네릭은 모든 타입을 받을 수 있지만, 그중에는 비교 연산자를 사용할 수 없는 타입도 있다
    // 일반적인 언어는 이런 것을 바이트로 대소 비교하기도 하고, 비교 가능 인터페이스를 받도록 제한을 두기도 한다
    // 러스트는 이런 것을 PartialOrd Trait으로 제한을 둔다
    let result = largest_2(&numbers);
    println!("The largest number is {}", result);

    let chars = vec!['y', 'm', 'a', 'q'];

    // let result = largest_1(&chars); // 이는 에러가 난다
    let result = largest_2(&chars);
    println!("The largest char is {}", result);



    struct Point_1<T> {
        x: T,
        y: T,
    }

    let integer = Point_1 { x: 5, y: 10 };
    let float = Point_1 { x: 1.0, y: 4.0 };
    // let wont_work = Point_1 { x: 5, y: 4.0 }; // 이는 에러가 난다 타입 추론이 안되기 때문이다

    struct Point_2<T, U> {
        x: T,
        y: U,
    }
    let now_work = Point_2 { x: 5, y: 4.0 }; // 이것은 가능하다



    // enum Option<T> { // 이전에 본 Option은 제네릭의 예시이다
    //     Some(T),
    //     None,
    // }

    enum Result<T, E> { // Result도 제네릭이다
        Ok(T),
        Err(E),
    }



    struct Point_3<T> {
        x: T,
        y: T,
    }
    
    impl<T> Point_3<T> { // 이렇게 메소드에도 제네릭을 사용할 수 있다
        fn x(&self) -> &T {
            &self.x
        }
    }

    let p = Point_3 { x: 5, y: 10 };
    println!("p.x = {}", p.x());



    struct Point_4<T, U> {
        x: T,
        y: U,
    }

    impl<T, U> Point_4<T, U> { // 이렇게 메소드에도 제네릭을 사용할 수 있다
        fn mixup<V, W>(self, other: Point_4<V, W>) -> Point_4<T, W> {
            Point_4 { // x는 self의 x, y는 other의 y
                x: self.x,
                y: other.y,
            }
        } // 리턴은 소유권 이동이다
    }

    let p1 = Point_4 { x: 5, y: 10.4 };
    let p2 = Point_4 { x: "Hello", y: 'c'};

    let p3 = p1.mixup(p2);

    println!("p3.x = {}, p3.y = {}", p3.x, p3.y);



    let integer = Some(5);
    let float = Some(5.0);

    // enum Option<T> {
    //     Some(T),
    //     None,
    // }

    enum Option_i32 {
        Some(i32),
        None,
    }
    
    enum Option_f64 {
        Some(f64),
        None,
    }

    // 보기에 제네릭이 더 편리하지만, 훨씬 느릴 것 같아 보인다
    // 하지만, rust는 제네릭을 사용하면 컴파일 타임에 타입이 아래 처럼 변환이 되어
    // 아래 네 개의 변수는 모두 같은 성능을 가짐
    // 이를 단형성화(monomorphization)라고 한다

    let integer = Option::Some(5); // 이는 제네릭이다
    let float = Option::Some(5.0); // Option<T> 로 받고 T를 타입 추론하여 사용한다
    let integer = Option_i32::Some(5);
    let float = Option_f64::Some(5.0);
}

fn largest_i32(list: &[i32]) -> i32 { // 제네릭이 없다면 이렇게 여러개의 함수를 만들어야 한다
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn largest_char(list: &[char]) -> char { // 그나마 메소드 오버로딩이 있으면 같은 이름을 가능 할 수도 있다
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

// fn largest_1<T>(list: &[T]) -> &T { // 제네릭을 사용하면 이렇게 하나의 함수로 가능하다
//     let mut largest = &list[0];

//     for item in list.iter() {
//         if item > largest {
//             largest = item;
//         }
//     }

//     largest
// } // 하지만 문제가 하나 있다

fn largest_2<T: std::cmp::PartialOrd>(list: &[T]) -> &T { // 이렇게 PartialOrd Trait을 사용하면 된다
    let mut largest = &list[0];

    for item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}