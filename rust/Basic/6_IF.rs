fn main() {
    let num = 3;

    if num < 5 {
        println!("smaller than 5");
    }
    else {
        println!("bigger or equal than 5");
    }

    // if num { // 이 코드는 num이 bool타입이 아니라서 error
    //     println!("num is not Zero");
    // }

    let number = 6;

    if number % 4 == 0 {
        println!("number is divisible by 4");
    }
    else if number % 3 == 0 {
        println!("number is divisible by 3");
    }
    else if number % 2 == 0 {
        println!("number is divisible by 2");
    }
    else {
        println!("number is not divisible by 4, 3, or 2");
    }

    let condition = true;

    let number = // if문은 리턴 값을 줄 수 있음
        if condition { 5 }
        else { 6 };

    println!("The value of number is: {}", number);

    // let condition = true;

    // let number =
    //     if condition { 5 }
    //     else { "six" }; // 하지만, 타입이 안 맞는 두 값을 if문 리턴으로 줄 수는 없음

    // println!("The value of number is: {}", number);

    let mut i = 0;

    loop {
        println!("i = {}", i);
        i = 
            if i == 5 { break } // break가 걸린 즉시 종료되고 할당은 없기 때문에, 그대로 5를 유지
            else { i + 1 } // i += 1;
    }

    println!();

    while i != 0 {
        println!("i = {}", i);

        i -= 1; // i--; 는 지원하지 않음
    }


    let a = [10, 20, 30, 40, 50];
    let mut index = 0;

    while index < 5 {
        println!("value = {}", a[index]);

        index = index + 1;
    }

    for i in a.iter() {
        println!("value = {}", i);
    }

    for number in (1..10).rev() {
        println!("{}!", number);
    }
    println!("Zero Count");
}