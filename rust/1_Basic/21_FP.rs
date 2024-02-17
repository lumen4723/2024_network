fn main() {
    // 고전적인 함수 중첩 방식의 코드
    fn like_many_cal_1(intensity: u32) -> u32 {
        println!("calculating slowly...");
        std::thread::sleep(std::time::Duration::from_secs(0));
        intensity
    }

    fn working_1(intensity: u32, random_number: u32) {
        if intensity < 25 {
            println!(
                "Today, do {} pushups!",
                like_many_cal_1(intensity)
            );
            println!(
                "Next, do {} situps!",
                like_many_cal_1(intensity)
            );
        } else {
            if random_number == 3 {
                println!("Take a break today! Remember to stay hydrated!");
            } else {
                println!(
                    "Today, run for {} minutes!",
                    like_many_cal_1(intensity)
                );
            }
        }
    }

    let users_value = 10;
    let random_number = 7;

    working_1(users_value, random_number);



    // 클로저를 이용한 함수 중첩 방식의 코드
    fn working_2(intensity: u32, random_number: u32) {
        let like_many_cal_2 = |intensity| { // 람다식으로 함수를 정의함
            println!("calculating slowly...");
            std::thread::sleep(std::time::Duration::from_secs(0));
            intensity
        };

        if intensity < 25 {
            println!(
                "Today, do {} pushups!",
                like_many_cal_2(intensity)
            );
            println!(
                "Next, do {} situps!",
                like_many_cal_2(intensity)
            );
        } else {
            if random_number == 3 {
                println!("Take a break today! Remember to stay hydrated!");
            } else {
                println!(
                    "Today, run for {} minutes!",
                    like_many_cal_2(intensity)
                );
            }
        }
    }

    working_2(users_value, random_number);



    let x = 4;
    // 일반적인 프로그래밍에서는 내부의 함수에 외부의 변수를 사용할 수 있음(Side-Effect)
    // fn equal_to_x(z: i32) -> bool { z == x }
    // 하지만, rust는 일반적으로 외부의 변수를 사용할 수 없음

    // 하지만 아래 람다식에서는 외부의 변수를 사용할 수 있음
    // 이 때, 람다식은 외부의 변수를 캡쳐하여 사용함
    // 이를 클로저(closure)라고 함
    let equal_to_x = |z| z == x;
    let y = 4;

    println!("x == y ? {}", equal_to_x(y));

    // 클로저는 외부의 변수를 캡쳐할 때 다음 3가지 Fn 트레잇 중 하나를 구현함
    // 1. FnOnce: 클로저가 외부의 변수를 소비할 때 사용됨
    //  즉, 소유권을 가져가는 클로저
    // 2. FnMut: 클로저가 외부의 변수를 가변으로 빌릴 때 사용됨
    //  즉, 가변 참조(클로저가 있으면 직접적으로 변수에 접근할 수 없고 클로저에선 변경 가능)을 사용하는 클로저
    // 3. Fn: 클로저가 외부의 변수를 불변으로 빌릴 때 사용됨
    //  즉, 불변 참조(클로저가 있으면 직접적으로 변수에 접근할 수 없고 클로저에서도 변경 불가능)를 사용하는 클로저

    let x = vec![1, 2, 3];

    // 이는 Fn으로 기본 구현되어 있어,
    // 클로져가 있으면 직접적으로 변수 접근을 막고, 클로저에서도 변경 불가능하게 함
    // let equal_to_x = move |z| z == x;

    // println!("can't use x here: {:?}", x);



    let v1 = vec![1, 2, 3];
    let v1_iter = v1.iter(); // iter()는 불변 참조를 반복자 패턴으로 사용함

    for val in v1_iter { // 반복문(for문)을 통한 출력
        print!("Got: {} ", val);
    }
    println!();


    let v1 = vec![1, 2, 3, 4];

    let mut v1_iter = v1.iter(); // iter()는 불변 참조를 반복자 패턴으로 사용함

    println!("{:?}", v1_iter.next()); // next()는 Option을 반환함

    loop {
        match v1_iter.next() { // 루프와 패턴 매칭을 사용해서도 출력을 할 수 있음
            Some(val) => print!("Got: {} ", val),
            None => break,
        }
    }
    println!();



    let v1: Vec<i32> = vec![1, 2, 3];

    let _ = v1.iter().map(|x| x + 1); // map()은 클로저를 인자로 받아서 새로운 반복자를 반환함
    // 하지만, 이는 아무것도 반환하진 않고 그저 Future 값을 가짐, 왜냐하면 반복자는 lazy한 특성을 가지고 있기 때문에
    // 즉, 반복자는 실제로 값을 사용할 때까지 계산을 하지 않음 (하스켈의 lazy evaluation과 비슷함)

    let v2: Vec<_> = v1.iter().map(|x| x + 1).collect(); // collect()는 반복자를 벡터로 변환함
    // 이는 값을 사용하기 때문에 반복자가 계산을 시작함

    println!("{:?}", v2);



    #[derive(Debug)] // 값을 출력하기 위한 디버깅 트레잇을 자동으로 구현해주는 속성(Attribute)
    struct Shoe {
        size: u32,
        style: String,
    }
    
    fn shoes_in_my_size(shoes: Vec<Shoe>, shoe_size: u32) -> Vec<Shoe> {
        shoes.into_iter()
            .filter(|s| s.size == shoe_size)
            .collect()
    }
    
    let shoes = vec![
        Shoe { size: 10, style: String::from("sneaker") },
        Shoe { size: 13, style: String::from("sandal") },
        Shoe { size: 10, style: String::from("boot") },
    ];

    let in_my_size = shoes_in_my_size(shoes, 10);

    println!("{:?}", in_my_size);



    struct Counter {
        count: u32,
    }
    
    impl Counter {
        fn new() -> Counter {
            Counter { count: 0 }
        }
    }

    impl Iterator for Counter {
        type Item = u32;
    
        fn next(&mut self) -> Option<Self::Item> {
            self.count += 1;
    
            if self.count < 6 {
                Some(self.count)
            } else {
                None
            }
        }
    }

    let mut counter = Counter::new(); // 0으로 초기화된 카운터 구조체 생성
    for i in counter { // 반복자가 구현된 구조체 반복문(for문)을 사용하면 자동으로 next()를 호출함
        // next()가 호출 될 때마다 FnMut 규칙에 따라 가변 참조로 값을 변경함
        print!("{} ", i);
    } // 반복문은 next()가 None을 반환할 때까지 자동으로 반복함
    println!();



    let sum: u32 = Counter::new() // 0으로 초기화된 카운터 구조체 생성
        .zip(Counter::new().skip(1)) // zip()은 두 반복자를 묶어서 튜플로 반환함,
            // skip(n)은 반복자의 앞의 n개를 건너뛰는 반복자를 반환함
        .map(|(a, b)| a * b) // map도 next()를 사용하여 순회하고(none에서 멈춤), 페어가 안되면 결과에서 배제됨
            // 그래서 결과는 (1*2), (2*3), (3*4), (4*5)로 2, 6, 12, 20이 됨
        .filter(|x| x % 3 == 0) // 이 중 6과 12만 남음
        .sum(); // 결과는 18: u32

    println!("sum: {}", sum);

    // 반복자는 rust의 제로 비용 추상화(Zero Cost Abstraction)의 대표적인 예시임
    // C++의 제로 오버헤드 원리와 비슷한 개념으로,
    // rust는 언롤링(Unrolling)과 인라인화(Inline)를 통해 오버헤드를 제거함
}