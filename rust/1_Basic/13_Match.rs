fn main() {
    enum Coin_1 {
        Penny,
        Nickel,
        Dime,
        Quarter,
    }

    fn value_in_cents_1(coin: Coin_1) -> u32 {
        // enum과 잘 어울리는 match 표현식
        match coin { // 얼핏보면 스위치문과 비슷해 보이지만 매우 다름
            Coin_1::Penny => {
                println!("Lucky penny!");
                1
            }
            Coin_1::Nickel => 5,
            Coin_1::Dime => 10,
            Coin_1::Quarter => 25,
        }
    } // 패턴 매칭은 스위치문보다 강력하고 다양한 패턴들에 매칭할 수 있음

    let coin = Coin_1::Penny;
    println!("value_in_cents(coin) = {}", value_in_cents_1(coin));
    let coin = Coin_1::Nickel;
    println!("value_in_cents(coin) = {}", value_in_cents_1(coin));
    let coin = Coin_1::Dime;
    println!("value_in_cents(coin) = {}", value_in_cents_1(coin));
    let coin = Coin_1::Quarter;
    println!("value_in_cents(coin) = {}", value_in_cents_1(coin));



    fn plus_one(x: Option<i32>) -> Option<i32> {
        match x {
            None => None,
            Some(i) => Some(i + 1), // 스위치문과 달리 패턴 매칭은 디스트럭처링도 가능함
        }
    }

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

    #[derive(Debug)]
    struct UsState {
        name: String,
    }

    enum Coin_2 {
        Penny,
        Nickel,
        Dime,
        Quarter(UsState), // enum의 케이스에는 구조체와 같이 값이 들어갈 수 있음
    }

    let coin = Coin_2::Quarter(UsState {name: String::from("New York")}); // 케이스에 값이 들어감

    let mut count = 0;
    match coin {
        Coin_2::Quarter(ref state) => println!("State quarter from {:?}!", state), // ref는 참조를 의미함
        // 패턴 매칭에 struct를 디스트럭처링할 때 ref를 사용하면 불변 참조로 대여한다는 의미
        _ => count += 1,
    }

    // 즉, 위의 코드는 아래와 같이 변환 가능함

    let mut count = 0;
    // 패턴 매칭을 위한 키워드인 if let을 사용함
    if let Coin_2::Quarter(state) = coin { // 참조를 사용하지 않으면 소유권이 이동함
        println!("State quarter from {:?}!", state);
    } else {
        count += 1;
    }
}