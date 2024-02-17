use std::collections::HashMap; // 해시맵은 표준 라이브러리에 있음

fn main() {
    let mut scores1 = HashMap::new(); // HashMap은 키와 값의 쌍을 저장하는 데이터 구조체

    scores1.insert(String::from("Blue"), 10); // insert() 메소드로 키와 값을 추가함
    scores1.insert(String::from("Yellow"), 50);
    // 이 때 scores는 <String, i32> 타입이 됨



    let field_name = String::from("Favorite color");
    let field_value = String::from("Blue");

    let mut map = HashMap::new();
    map.insert(field_name, field_value); // insert() 메소드는 소유권을 이동시킴

    // println!("field_name is {}", field_name); // 더 이상 소유권이 없어서 error



    let teams  = vec![String::from("Blue"), String::from("Yellow")];
    let initial_scores = vec![10, 50];
    // iter() 메소드로 값을 순회, zip() 메소드로 두 벡터를 합침, collect() 메소드로 자동 HashMap 타입으로 변환
    let mut scores2: HashMap<_, _> = teams.iter().zip(initial_scores.iter()).collect();
    // 이 때 자동으로 타입이 맞춰지려면 타입을 명시하고, _를 사용하면 컴파일러가 추론함
    // 이 때 scores는 <&String, &i32> 타입이 됨



    let team_name = String::from("Blue");
    let score = scores1.get(&team_name); // get() 메소드로 키에 해당하는 값을 얻음
    println!("score is {:?}", score); // get()의 반환값은 Option<T> 타입이 됨



    for (key, value) in &scores1 { // HashMap은 순회할 때 순서가 보장되지 않음
        println!("{}: {}", key, value);
    }



    scores.insert(String::from("Blue"), 25); // 이미 있는 키에 값을 추가하면 덮어씀
    println!("{:?}", scores); // {"Blue": 25, "Yellow": 50}



    scores.entry(String::from("Blue")).or_insert(50); // entry() 메소드로 키에 해당하는 값이 없을 때만 추가함
    scores.entry(String::from("Red")).or_insert(50); // Red는 없으므로 추가됨
    println!("{:?}", scores); // {"Blue": 25, "Red": 50, "Yellow": 50}



    let text = "hello world wonderful world";
    let mut map = HashMap::new();

    // split_whitespace() 메소드로 공백을 기준으로 단어를 분리함
    for word in text.split_whitespace() { // 단어를 분리하고 하나씩 순회함
        
        // entry() 메소드로 키에 해당하는 값이 없을 때만 추가함, 넣을 때는 0으로 넣음
        let count = map.entry(word).or_insert(0);
        // or_insert() 메소드는 해당 키에 대한 값을 가변 참조로 반환함
        
        *count += 1; // 가변 참조로 값을 증가시킴
    }
    println!("{:?}", map); // {"world": 2, "hello": 1, "wonderful": 1}
}