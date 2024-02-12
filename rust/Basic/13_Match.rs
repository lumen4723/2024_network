enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter,
}

fn value_in_cents(coin: Coin) -> u32 {
    match coin {
        Coin::Penny => {
            println!("Lucky penny!");
            1
        }
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}

fn plus_one(x: Option<i32>) -> Option<i32> {
    match x {
        None => None,
        Some(i) => Some(i + 1),
    }
}

fn main() {
    let coin = Coin::Penny;
    println!("value_in_cents(coin) = {}", value_in_cents(coin));
    let coin = Coin::Nickel;
    println!("value_in_cents(coin) = {}", value_in_cents(coin));
    let coin = Coin::Dime;
    println!("value_in_cents(coin) = {}", value_in_cents(coin));
    let coin = Coin::Quarter;
    println!("value_in_cents(coin) = {}", value_in_cents(coin));



    let five = Some(5);
    let six = plus_one(five);
    let none = plus_one(None);

    println!("five = {:?}", five);
    println!("six = {:?}", six);
    println!("none = {:?}", none);

    for option in [five, six, none].iter() {
        match option { // match는 패턴 매칭을 위한 키워드
            // Some(x)는 Some 타입을 가지면 x를 디스트럭처링하고 함수를 실행함
            Some(x) => println!("x = {}", x),
            None => println!("None"), // None 타입이면 None을 출력함
        }
    } // rust는 여타 언어의 match와 달리 타입 매칭, 값 매칭 뿐만 아니라 디스트럭처링등의 패턴 매칭도 지원함



    let val = 5;

    match val {
        1 => println!("one"), // 값 매칭
        2 => println!("two"),
        3 | 4 => println!("three or four"), // |는 or 연산자로 3 또는 4에 매칭됨
        _ => println!("something"), // _는 와일드카드 패턴으로 어떤 값에도 매칭됨
        // 5는 어떤 패턴에도 매칭되지 않으므로 _ 패턴에 매칭됨
    }
    println!("val = {}", val);



    let op = Some(1);
    if let Some(1) = op { // if let은 match의 간략한 버전으로 패턴 매칭을 위한 키워드
        println!("one");
    } 
    else if let None = op {
        println!("none");
    }
    else {
        println!("not one or none");
    }



    let mut count = 0;
    match coin {
        Coin::Quarter(state) => println!("State quarter from {:?}!", state),
        _ => count += 1,
    }

    // 즉, 위의 코드는 아래와 같이 변환 가능함

    let mut count = 0;
    if let Coin::Quarter(state) = coin {
        println!("State quarter from {:?}!", state);
    } else {
        count += 1;
    }
}