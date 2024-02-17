fn main() {
    let mut s = String::new();

    // 아래 세 줄은 모두 같은 결과를 반환함
    let data = "initial contents";
    let s = data.to_string();

    let s = "initial contents".to_string();

    let s = String::from("initial contents");



    // rust의 string 은 기본적으로 utf-8로 인코딩됨
    let hello = String::from("السلام عليكم");
    let hello = String::from("Dobrý den");
    let hello = String::from("Hello");
    let hello = String::from("שָׁלוֹם");
    let hello = String::from("नमस्ते");
    let hello = String::from("こんにちは");
    let hello = String::from("안녕하세요");
    let hello = String::from("你好");
    let hello = String::from("Olá");
    let hello = String::from("Здравствуйте");
    let hello = String::from("Hola");



    let mut s = String::from("foo");
    s.push_str("bar"); // push_str() 메소드는 문자열을 추가함(String을 받음)
    println!("s is {}", s); // s is foobar

    let mut s = String::from("lo");
    s.push('l'); // 한 글자를 추가할 경우에는 push('?')도 사용 가능
    println!("s is {}", s);

    let mut s1 = String::from("foo");
    let s2 = "bar";
    s1.push_str(&s2); // push_str()는 문자열 슬라이스 인 &str도 받을 수 있음
    println!("s1 is {}", s1);
    println!("s2 is {}", s2); // 문자열 슬라이스는 소유권이 이동되지 않기 때문에 여전히 유효함



    let s1 = String::from("Hello, ");
    let s2 = String::from("world!");
    let s3 = s1 + &s2; // s1은 소유권이 사용되고 s2는 불변 참조로 사용됨
    // println!("s1 is {}", s1); // s1은 더 이상 유효하지 않아서 error
    println!("s2 is {}", s2);
    println!("s3 is {}", s3);



    let s1 = String::from("tic");
    let s2 = String::from("tac");
    let s3 = String::from("toe");

    let s = s1 + "-" + &s2 + "-" + &s3; // s1은 소유권이 사용되고 s2, s3는 불변 참조로 사용됨
    println!("s is {}", s);

    let s1 = String::from("tic");
    let s2 = String::from("tac");
    let s3 = String::from("toe");

    let s = format!("{}-{}-{}", s1, s2, s3); // format! 매크로는 그냥 적어도 소유권을 사용하지 않음
    println!("s1 is {}, s2 is {}, s3 is {}", s1, s2, s3); // 소유권이 있으므로 Ok
    // 물론 불변 참조를 사용할 수도 있음
    println!("s is {}", s);



    let s1 = String::from("hello");
    // let h = s1[0]; // 인덱스로 접근하는 것은 불가능함 error
    // 이는 rust가 기본 UTF-8(2 ~ 3Byte)을 사용하는데 UTF_8은 각 단어의 문자열의 바이트 길이를 확실하게 알 수 없기 때문에,
    
    // 예를 들어 안녕하세요는 3Byte * 5 => [0xec, 0x95, 0x88, 0xeb, 0x85, 0x95, 0xed, 0x95, 0x98, 0xec, 0x84, 0xb8, 0xec, 0x9a, 0x94]가 되는데
    // "안녕하세요"[0] 은 [0xec, 0x95, 0x88] 을 말하지만,
    // "Здравствуйте"는 2Byte * 12 => [0xd0, 0x97, 0xd0, 0xb4, 0xd1, 0x80, 0xd0, 0xb0, 0xd0, 0xb2, 0xd1, 0x81, 0xd1, 0x82, 0xd0, 0xb2, 0xd1, 0x83, 0xd0, 0xb9, 0xd1, 0x82, 0xd0, 0xb5]가 되는데
    // 이는 "Здравствуйте"[0] 은 [0xd0, 0x97]을 말해서 언어별 문자열의 바이트 길이를 확실하게 알 수 없음

    // 그래서 이를 위반하는 접근을 사전에 막기 위해 기능을 제한함

    let hello = "Здравствуйте";

    let s = &hello[0..4]; // 하지만 스트링 슬라이스를 사용하면 불안정한 인덱스 접근을 일부 할 수 있음
    println!("s is {}", s); // Зд

    let hello = "안녕하세요";

    let s = &hello[0..6]; //
    // let s = &hello[0..5]; // 예를 들어 '녕'이 잘리면 error
    println!("s is {}", s); // 안녕



    for c in "안녕하세요".chars() { // 문자열의 각 문자를 순회함
        print!("{} ", c);
    }
    println!();

    for b in "안녕하세요".bytes() { // 문자열의 각 바이트를 순회함
        print!("{} ", b);
    }
    println!();
}