fn main() {
    // Iterator 트레잇
    trait Iterator {
        type Item;
    
        fn next(&mut self) -> Option<Self::Item>;
    }

    struct Counter {
        count: u32,
        end: u32,
    }

    // Iterator 트레잇은 next 메서드를 통해 다음 값을 반환함
    impl Iterator for Counter {
        type Item = u32;
    
        fn next(&mut self) -> Option<Self::Item> {
            if self.count < self.end {
                self.count += 1;
                Some(self.count)
            } else {
                None
            }
        }
    }

    let mut counter = Counter { count: 0, end: 10 };
    println!("counter.next(): {:?}", counter.next());



    // Add 트레잇
    use std::ops::Add;

    struct Point {
        x: i32,
        y: i32,
    }
    
    // Add 트레잇은 add 메서드를 통해 두 값을 더함
    impl Add for Point {
        type Output = Point;
    
        fn add(self, other: Point) -> Point {
            Point {
                x: self.x + other.x,
                y: self.y + other.y,
            }
        }
    }

    let p1 = Point { x: 1, y: 0 };
    let p2 = Point { x: 2, y: 3 };
    let p3 = p1 + p2;
    println!("p3.x: {}, p3.y: {}", p3.x, p3.y);



    // Display 트레잇
    use std::fmt;

    struct Point2 {
        x: i32,
        y: i32,
    }

    // Display 트레잇은 fmt::Display 트레잇을 구현함
    impl fmt::Display for Point2 {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            write!(f, "({}, {})", self.x, self.y)
        }
    }

    let p = Point2 { x: 0, y: 7 };
    println!("p: {}", p);



    // Default 트레잇
    struct Point3 {
        x: i32,
        y: i32,
    }

    // Default 트레잇은 기본값을 반환하는 메서드를 구현함
    impl Default for Point3 {
        fn default() -> Point3 {
            Point3 { x: 0, y: 0 }
        }
    }

    let p = Point3::default();
    println!("p.x: {}, p.y: {}", p.x, p.y);



    // 타입 별칭(trait alias)
    type kilometer = i32;

    let x: i32 = 5;
    let y: kilometer = 5;

    println!("x + y = {}", x + y); // 두 타입은 동일하게 동작함



    // 부정 타입(negative trait)
    fn bar() -> ! { // !는 부정 타입으로 함수가 결코 반환하지 않음을 나타냄
        panic!("This function never returns!");
    }



    // 동적 크기 타입(dynamic size type == DST)

    // str은 동적 크기 타입으로 크기가 컴파일 시점에 결정되지 않음
    let s1: &str = "Hello, world!";
    // String은 동적 크기 타입으로 크기가 컴파일 시점에 결정되지 않음
    let s2: String = String::from("Hello, world!");

    println!("s1: {}, s2: {}", s1, s2);



    fn generic_1<T>(t: T) {
        // --생략--
    }
    // 위 아래 함수는 동치이다
    fn generic_2<T: Sized>(t: T) {
        // --생략--
    }

    // 하지만 아래처럼 크기를 완화할 수도 있다
    // ?Sized는 Sized일 수도 있고, Sized가 아닐 수도 있다 타입을 나타냄
    // 보통 제네릭에서 컴파일 시점에서 크기가 정적이어야 하는 기본 값을 덮어쓰기 위해 사용함
    fn generic_3<T: ?Sized>(t: &T) {
        // --생략--
    }



    // 함수 포인터
    fn add_one(x: i32) -> i32 {
        x + 1
    }

    fn do_twice(f: fn(i32) -> i32, arg: i32) -> i32 { // fn 키워드를 사용하여 함수 포인터를 정의함
        f(arg) + f(arg)
    }
    
    let answer = do_twice(add_one, 5);
    println!("The answer is: {}", answer);



    // 클로저 반환하기

    // fn returns_closure() -> dyn Fn(i32) -> i32 {
    //     |x| x + 1
    // } // 이 코드는 클로저를 직접 반환하려고 하지만, 에러가 발생함
    // 클로저는 크기가 컴파일 시점에 결정되지 않기 때문에 Sized 트레잇을 만족하지 않음

    fn returns_closure() -> Box<dyn Fn(i32) -> i32> {
        Box::new(|x| x + 1)
    } // 이는 트레잇 객체를 사용하여 다른 타입의 값을 허용한 후, Box로 감싸서 반환함

    let f = returns_closure(); // 사용 예제
    let answer = f(5);
    println!("The answer is: {}", answer);
}