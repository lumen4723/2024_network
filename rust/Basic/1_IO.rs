use std::io::Write; // 이걸 추가해야 .flush가 가능하다

fn main() {
    println!("HELLO"); // 무조건 출력하고 개행이 된다

    print!("INPUT: "); // 개행이 안되긴 하지만 출력시점을 제어할 수 없다
    std::io::stdout() // 이걸로 출력을 강제할 수 있다
        .flush()
        .unwrap();
    
    let mut input = String::new(); // 입력은 기본 String타입으로 받는다
    std::io::stdin()
        .read_line(&mut input)
        .unwrap();

    println!("You typed: {}", input.trim()); // trim()은 문자열의 앞뒤 공백(개행)을 제거한다



    let x = 5;
    let y = 10;

    println!("x = {x}, x + y = {}", x + y);



    print!("INPUT NUMBER: "); // 개행이 안되긴 하지만 출력시점을 제어할 수 없다
    std::io::stdout() // 이걸로 출력을 강제할 수 있다
        .flush()
        .unwrap();

    let mut input = String::new();
    std::io::stdin()
        .read_line(&mut input) // 입력은 반드시 String 타입이어야 한다
        .unwrap();

    // String을 u32로 하려면 parse()를 하고 match로 에러처리를 해야 한다
    let int: u32 = match input.trim().parse() {
        Ok(num) => num,
        Err(_) => 0,
    };

    println!("You typed: {}", int);


    let int: u32 = input.trim().parse().unwrap(); // 에러처리를 안하고 단순 await를 하면 
    println!("You typed: {}", int); // 에러가 그대로 나올 수 있음
}