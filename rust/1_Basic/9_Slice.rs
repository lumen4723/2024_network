fn main() {
    let mut s = String::from("hello world");

    let len = first_word_len(&s);
    println!("len = {}", len);

    s.clear(); // s를 비워도

    println!("len = {}", len); // 여전히 word는 5를 가질 것임
    // 하지만, word는 s의 첫 단어의 길이를 의미 했는데, 이는 의미가 없는 값이 됨



    let s1 = String::from("hello world");

    // 스트링의 슬라이스를 생성
    let hello = &s1[0..5]; // 0부터 5까지의 슬라이스를 생성 후 불변 참조
    let world = &s1[6..11]; // 6부터 11까지의 슬라이스를 생성 후 불변 참조

    println!("hello = {}, world = {}", hello, world);

    let slice1 = &s1[0..2]; // 0 ~ 2 슬라이스 불변 참조 
    let slice2 = &s1[..2]; // 0 ~ 2 슬라이스 불변 참조
    println!("is slice1 == slice2? {}", slice1 == slice2);

    let slice3 = &s1[3..s1.len()]; // 3 ~ 끝까지 슬라이스 불변 참조
    let slice4 = &s1[3..]; // 3 ~ 끝까지 슬라이스 불변 참조
    println!("is slice3 == slice4? {}", slice3 == slice4);

    let slice5 = &s1[0..s1.len()]; // 전체 슬라이스 불변 참조
    let slice6 = &s1[..]; // 전체 슬라이스 불변 참조
    println!("is slice5 == slice6? {}", slice5 == slice6);



    let mut s = String::from("hello world");

    let word = first_word_str_1(&s);
    println!("word = {}", word);

    // s를 비우면 word는 더 이상 의미가 없기 때문에
    // word를 불변 참조 사용자로 하여금 s의 직접 사용을 막음
    // s.clear(); // 그래서 빌림 상태이므로 error

    println!("word = {}", word);



    let word = first_word_str_2(&s[1..]); // 스트링 슬라이스를 넘길 수도 있음
    println!("word = {}", word);



    let a = [1, 2, 3, 4, 5]; // 배열, 리스트도 슬라이스를 가질 수 있음
    let slice = &a[1..3]; // 배열 슬라이스 불변 참조
    println!("slice = {:?}", slice);
}

fn first_word_len(s: &String) -> usize {
    let bytes = s.as_bytes(); // 스트링을 바이트로 변환

    //  bytes.iter().enumerate() => (인덱스: usize, 각 원소의 바이트 값: &u8)
    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' { // 바이트 공백인지 확인
            return i; // 공백이 있는 공간의 인덱스를 반환
        }
    }

    s.len() // 공백이 없는 단어면 전체 길이를 반환
}

fn first_word_str_1(s: &String) -> &str { // 스트링 슬라이스는 &str로 반환
    let bytes = s.as_bytes(); // 스트링을 바이트로 변환

    //  bytes.iter().enumerate() => (인덱스: usize, 각 원소의 바이트 값: &u8)
    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' { // 바이트 공백인지 확인
            return &s[0..i]; // 공백이 있는 공간의 슬라이스를 불변 참조로 반환
        }
    }

    &s[..] // 공백이 없는 단어면 전체 슬라이스를 불변 참조로 반환
}

fn first_word_str_2(s: &str) -> &str { // &str은 &String보다 더 일반적인 타입으로 사용 가능
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }

    &s[..]
}