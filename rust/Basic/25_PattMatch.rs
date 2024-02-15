fn main() {
    // 고전적인 디스트럭처링(패턴매칭) 방식
    let (x, y, z) = (1, 2, 3);
    println!("x: {}, y: {}, z: {}", x, y, z);



    // match 구문을 사용하여 패턴 매칭을 할 수 있음
    let some_u8_value: Option<u8> = Some(0u8);
    match some_u8_value {
        Some(x) => {
            println!("x is {}", x);
        },
        _ => {
            println!("None");
        }
    }



    // if let 구문을 사용하여 패턴 매칭을 할 수 있음
    let favorite_color: Option<&str> = None;
    let is_tuesday = false;
    let age: Result<u8, _> = "34".parse();

    if let Some(color) = favorite_color {
        println!("Using your favorite color, {color}, as the background");
    }
    else if is_tuesday {
        println!("Tuesday is green day!");
    }
    else if let Ok(age) = age {
        if age > 30 {
            println!("Using purple as the background color");
        }
        else {
            println!("Using orange as the background color");
        }
    }
    else {
        println!("Using blue as the background color");
    }



    // while let 구문을 사용하여 패턴 매칭을 할 수 있음
    let mut stack = Vec::new();

    stack.push(1);
    stack.push(2);
    stack.push(3);

    while let Some(top) = stack.pop() {
        print!("{} ", top);
    }
    println!();



    // for 구문에서도 패턴 매칭을 할 수 있음
    let v = vec!['a', 'b', 'c'];

    for (index, value) in v.iter().enumerate() {
        println!("{} is at index {}", value, index);
    }



    // 함수의 매개변수 패턴 매칭
    fn print_coordinates(&(x, y): &(i32, i32)) {
        println!("Current location: ({}, {})", x, y);
    }

    let point = (3, 5);
    print_coordinates(&point);



    // 리터럴 값 패턴 매칭
    let x = 1;
    match x {
        1 => println!("one"),
        2 => println!("two"),
        3 => println!("three"),
        _ => println!("anything"),
    }

    // 명명된 변수 패턴 매칭
    let x = Some(5);
    let y = 10;

    match x {
        Some(50) => println!("Got 50"),
        Some(y) => println!("Matched, y = {:?}", y),
        _ => println!("Default case, x = {:?}", x),
    }

    println!("at the end: x = {:?}, y = {:?}", x, y);

    // 다수의 패턴 매칭
    let x = 1;
    match x {
        1 | 2 => println!("one or two"),
        3 => println!("three"),
        _ => println!("anything"),
    }

    // 범위 패턴 매칭
    let x = 5;
    match x {
        1..=5 => println!("one through five"),
        _ => println!("something else"),
    }

    let x = 'c';
    match x {
        'a'..='j' => println!("early ASCII letter"),
        'k'..='z' => println!("late ASCII letter"),
        _ => println!("something else"),
    }



    // 구조체 패턴 매칭
    struct Point {
        x: i32,
        y: i32,
    }

    let p = Point { x: 0, y: 7 };

    let Point { x: a, y: b } = p;
    println!("a: {}, b: {}", a, b);

    match p {
        Point { x, y: 0 } => println!("On the x axis at {}", x),
        Point { x: 0, y } => println!("On the y axis at {}", y),
        Point { x, y } => println!("On neither axis at ({}, {})", x, y),
    }



    // 열거형(enum) 패턴 매칭
    enum Message_1 {
        Quit,
        Move { x: i32, y: i32 },
        Write(String),
        ChangeColor(i32, i32, i32),
    }

    let msg = Message_1::ChangeColor(0, 160, 255);

    match msg {
        Message_1::Quit => {
            println!("The Quit variant has no data to destructure.");
        },
        Message_1::Move { x, y } => {
            println!("Move in the x direction {} and in the y direction {}", x, y);
        },
        Message_1::Write(text) => println!("Text message: {}", text),
        Message_1::ChangeColor(r, g, b) => {
            println!("Change the color to red {}, green {}, and blue {}", r, g, b);
        },
    }



    // 중첩 구조체 패턴 매칭 1
    enum Color {
        Rgb(i32, i32, i32),
        Hsv(i32, i32, i32),
    }

    enum Message_2 {
        Quit,
        Move { x: i32, y: i32 },
        Write(String),
        ChangeColor(Color),
    }

    let msg = Message_2::ChangeColor(Color::Hsv(0, 160, 255));

    match msg {
        Message_2::ChangeColor(Color::Rgb(r, g, b)) => {
            println!("Change the color to red {}, green {}, and blue {}", r, g, b);
        },
        Message_2::ChangeColor(Color::Hsv(h, s, v)) => {
            println!("Change the color to hue {}, saturation {}, and value {}", h, s, v);
        },
        _ => (),
    }

    // 중첩 구조체 패턴 매칭 2
    let ((feet, inches), Point { x, y }) = ((3, 10), Point { x: 3, y: -10 });



    // 패턴 매칭으로 인자 값 무시하기 1
    fn foo(_: i32, y: i32) {
        println!("This code only uses the y parameter: {}", y);
    }

    foo(3, 4);

    // 패턴 매칭으로 인자 값 무시하기 2
    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (first, _, third, _, fifth) => {
            println!("Some numbers: {first}, {third}, {fifth}")
        }
    }



    // _로 시작하는 변수이름을 사용하여 패턴 매칭하기
    let s = Some(String::from("Hello!"));

    if let Some(_s) = s {
        println!("found a string");
    }

    // println!("{:?}", s); // 이는 에러가 발생함
    // _s가 s의 소유권을 가져가기 때문에 s는 사용할 수 없음

    let s = Some(String::from("Hello!"));

    if let Some(_) = s {
        println!("found a string");
    }

    println!("{:?}", s); // 이는 에러가 발생하지 않음
    // _는 패턴매칭에서 변수를 무시하는 로직으로 작용하여 소유권을 가져가지 않음



    // 패턴 매칭에서 모호한 표현을 하는 경우
    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (first, .., last) => { // 이는 첫 값과 마지막 값만을 가져옴
            println!("Some numbers: {first}, {last}");
        }
    }

    // match numbers {
    //     (.., second, ..) => { // 이는 중간에 어떤 값인지 특정이 되지 않아 에러가 발생함
    //         println!("Some numbers: {second}");
    //     }
    // }

    match numbers {
        (_, second, ..) => { // 이는 _가 값을 무시하고 바로 두 번째를 가리켜 정상적으로 작동함
            println!("Some numbers: {second}");
        }
    }



    // 매치 가드를 사용하여 패턴 매칭하기 1
    let num = Some(4);

    match num {
        // 좌측 값이 매칭 된 경우, 우측 조건을 검사함
        Some(x) if x < 5 => println!("less than five: {x}"),
        Some(x) => println!("{x}"),
        None => (),
    }

    // 매치 가드를 사용하여 패턴 매칭하기 2
    let x = Some(5);
    let y = 10;
    match x {
        Some(50) => println!("Got 50"),
        Some(n) if n == y => println!("Matched, n = {n}"),
        _ => println!("Default case, x = {:?}", x),
    }

    println!("at the end: x = {:?}, y = {y}", x);

    // 매치 가드를 사용하여 패턴 매칭하기 3
    let x = 4;
    let y = false;

    match x {
        // 이는 좌측과 우측의 경계가 if로 구분되어 있다는 것을 의미함
        // 즉, 4 | 5 | (6 if y) 이 아니라 (4 | 5 | 6) if y로 해석됨
        4 | 5 | 6 if y => println!("yes"),
        _ => println!("no"),
    }



    // @를 사용하여 패턴 매칭하기
    enum Message {
        Hello { id: i32 },
    }

    let msg = Message::Hello { id: 5 };

    match msg {
        Message::Hello {
            id: id_variable @ 3..=7, // id_variable에 3..=7의 범위를 할당함
            // 즉, 3, 4, 5, 6, 7중 하나인 값이 들어오면 그 값을 id_variable에 할당함
            // 예시에서는 msg의 id가 5이므로, id_variabledms 3에서 7사이의 값을 받으므로, 5가 할당됨
        } => println!("Found an id in range: {}", id_variable),
        Message::Hello { id: 10..=12 } => {
            println!("Found an id in another range")
        }
        Message::Hello { id } => println!("Found some other id: {}", id),
    }
}