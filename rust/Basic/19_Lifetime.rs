fn main() {
    // rust는 라이프타임(Lifetime)을 사용하여 컴파일 타임에 참조가 유효한지 검사함
    // 라이프타임은 참조자의 유효범위를 검사하는게 핵심이다
    let r;
    {
        let x = 5; // 소유권 생성
        r = &x; // 정상적으로 불변 참조가 이루어짐
    } // 하지만 x의 소유권이 끝나면서 r의 참조가 라이프타임의 검사에 걸려서 r도 x와 수명을 같이 함

    // println!("r: {}", r); // x와 함께 소멸 된 r을 사용하려고 하면 컴파일 에러가 발생함

    // 라이프타임 검사는 구체적으로 빌림 검사기(Borrow checker)를 통해 이루어짐
    // 빌림 검사기는 모든 소유와 빌림 스코프를 검사해서 스택 괄호 문제 처럼
    // 정상적으로 스코프가 잘림 없이 이루어져 있는지 검사함

    // 이런 과정을 통해 rust는 dangling reference error를 방지함



    // 반환 값이 참조자이면 dangling reference error가 생기기 너무 쉬움
    // 그래서 rust는 반환 값 참조자에 라이프타임을 명시하라고 강제함
    // fn longest_1(x: &str, y: &str) -> &str {
    //     if x.len() > y.len() { x }
    //     else { y }
    // }

    // &i32 // 일반적인 참조자
    // &'a i32 // 라이프타임이 'a로 명시된 불변 참조자
    // &'a mut i32 // 라이프타임이 'a로 명시된 가변 참조자

    // 이렇게 하면 문제가 해결될 것 같지만 아직 에러가 발생함
    // fn longest_2<'a>(x: &str, y: &str) -> &'a str {
    //     // 이유는 x와 y의 라이프타임이 명시되지 않은 상태로 반환 값은 x와 y의 영향을 받는데
    //     // x, y의 라이프타임이 반환 값의 'a보다 긴지 짧은지 알 수 없기 때문임
    //     if x.len() > y.len() { x }
    //     else { y }
    // }

    fn longest_3<'a>(x: &'a str, y: &'a str) -> &'a str {
        // 이렇게 하면 x와 y의 라이프타임이 명시되어 반환 값의 라이프타임이 명확해짐
        if x.len() > y.len() { x }
        else { y }
    }

    // 사용은 다음과 같이 하면 됨
    let string1 = String::from("abcd");
    let string2 = "xyz";

    let result = longest_3(string1.as_str(), string2);
    println!("The longest string is {}", result);



    // 이것도 가능 할 것임
    let string1 = String::from("long string is long");

    {
        let string2 = String::from("xyz");
        let result = longest_3(string1.as_str(), string2.as_str());
        println!("The longest string is {}", result);
    }



    // //하지만 이것은 불가능 할 것임
    // let string1 = String::from("long string is long");
    // let result;
    // {
    //     let string2 = String::from("xyz");
    //     result = longest_3(string1.as_str(), string2.as_str());
    // }
    // println!("The longest string is {}", result);



    // 함수의 인자의 라이프타임을 명시하는 것은 항상 필요한 것은 아님
    // 아래 함수는 반환 값 참조자가 x와 y의 영향을 전혀 받지 않기 때문에 라이프타임을 명시할 필요가 없음
    // 물론 아래 함수는 result의 스코프가 끝나면서 dangling reference error가 발생해서 사용할 수 없음
    // fn longest<'a>(x: &str, y: &str) -> &'a str {
    //     let result = String::from("really long string");
    //     result.as_str()
    // }
    fn always_left<'a>(x: &'a str, y: &str) -> &'a str { // 이런건 가능함
        x
    }

    let string1 = String::from("abcd");
    let string2 = "xyz";

    println!("The left string is {}", always_left(string1.as_str(), string2));



    // 라이프타임은 함수에서만 정의할 필요는 없음
    struct ImportantExcerpt<'a> { // 구조체에서도 라이프타임을 명시할 수 있음
        // 구조체의 필드에도 라이프타임을 명시할 수 있음
        part: &'a str, // 그래서 참조자를 필드로 가지는 코드도 가능함
    }
    
    let i;
    {
        let novel = String::from("Call me Ishmael. Some years ago...");
        let first_sentence = novel.split('.')
        .next()
        .expect("Could not find a '.'");

        {
            // 생성 될 떄 사용된 first_sentence의 라이프타임이 'a가 되어 i의 라이프타임이 됨 
            // 즉, i의 라이프 타임은 first_sentence의 라이프타임과 같아짐
            i = ImportantExcerpt { part: first_sentence }; 

            println!("1_i.part = {}", i.part); // 여기는 당연히 사용 가능함
        }

        println!("2_i.part = {}", i.part); // 여기는 여전히 first_sentence의 라이프타임이 남아서 사용 가능함
    }

    // println!("3_i.part = {}", i.part); // 여기는 first_sentence의 라이프타임이 끝나서 사용 불가능함



    // 다시 보는 스트링 슬라이스 함수
    // 스트링 슬라이스 함수는 라이프타임을 명시할 필요가 없음
    // 그 이유는 사실 스트링 슬라이스는 반드시 라이프타임을 명시해야 하기 때문에
    // 컴파일러는 자동추론을 통해 fn first_word_str_2<'a>(s: &'a str) -> &'a str; 로 변환하기 때문
    // 이를 rust에서는 라이프타임 생략 규칙(lifetime elision rules)을 정하고 이를 통해 생략을 가능하게 함
    fn first_word_str_2(s: &str) -> &str {
        let bytes = s.as_bytes();
    
        for (i, &item) in bytes.iter().enumerate() {
            if item == b' ' {
                return &s[0..i];
            }
        }
    
        &s[..]
    }
    // 정확한 생략 규칙 순서는 다음과 같다
    // 1. 인자의 라이프타임이 고유하게('a) 존재한다
    // 2. 리턴의 결과 값이 반드시 'a로 명시된 인자의 스트링 슬라이스(&'a str)이다
    // 3. 즉, 이 함수의 반환 값의 라이프 타임은 'a이므로 이 함수는 라이프타임을 생략해도 문제가 없다



    impl<'a> ImportantExcerpt<'a> { // 메소드에서도 라이프타임을 명시할 수 있음
        
        // 상수 함수는 반환타입이 i32이므로 라이프타임을 명시할 필요가 없음
        fn level(&self) -> i32 {
            3
        }

        // 반환 값이 참조자이므로 라이프타임을 명시해야 함
        // 하지만, 라이프타임 생략 규칙에 의해 명시할 필요가 없음
        fn announce_and_return_part(&self, announcement: &str) -> &str {
            println!("Attention please: {}", announcement);
            self.part
        }
    }



    // 'static 라이프타임은 프로그램 전체에 존재하는 라이프타임임
    // 이는 정적 라이프타임이라고 부르며 프로그램이 종료될 때까지 존재함
    // 사실 선언 스트링 값은 시스템에서 bss나 data 섹션에 저장되어 프로그램이 종료될 때까지 사용하기 때문에
    // 기본 정적 라이프타임을 가지고 있음
    let s: &'static str = "I have a static lifetime.";
    let s: &str = "I have a static lifetime."; // 둘은 묵시적으로 같은 의미임


    // 'a은 라이프타임, T는 제네릭 타입이고,
    // 특히 T는 Display를 하한으로 가지는 트레잇 바운드를 사용함
    fn longest_with_an_announcement<'a, T>(x: &'a str, y: &'a str, ann: T) -> &'a str
        where T: std::fmt::Display
    {
        println!("Announcement! {}", ann);
        if x.len() > y.len() { x }
        else { y }
    }
}