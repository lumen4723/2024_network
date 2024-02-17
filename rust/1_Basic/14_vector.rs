fn main() {
    let v: Vec<i32> = Vec::new(); // 벡터 컨테이너 생성

    let v = vec![1, 2, 3]; // 벡터 컨테이너 생성과 초기화

    let mut v = Vec::new(); // 가변 벡터 컨테이너 생성
    v.push(5); // 벡터에 원소 추가
    v.push(6);
    v.push(7);
    v.push(8);



    {
        let v = vec![1, 2, 3, 4];
    } // 벡터는 스코프를 벗어나면서 모든 원소를 해제함

    let v = vec![1, 2, 3, 4, 5];

    let val: &i32 = &v[2];
    println!("val = {}", val);

    let val: Option<&i32> = v.get(3);
    match val {
        Some(x) => println!("val = {}", x),
        None => println!("None"),
    }



    let v = vec![1, 2, 3, 4, 5];

    // 벡터의 인덱스가 범위를 벗어나면 패닉을 일으킴(이는 index of out of bounds와 같음)
    // let does_not_exist = &v[10];
    // 하지만, get 메소드는 Option<T>를 반환하므로 패닉 대신 None을 반환함
    let does_not_exist = v.get(10);
    match does_not_exist {
        Some(x) => println!("x = {}", x),
        None => println!("None"),
    }



    let mut v = vec![1, 2, 3, 4, 5];
    // 벡터의 원소를 사용하는 과정은 참조를 사용하므로 불변 참조를 여러 개 만들 수 있음
    let first = &v[0];

    // 하지만, 참조(빌림)를 사용하는 과정에서 본래의 값을 변경한다면 대여가 끝난 것으로 판단
    v.push(6); // 대여한 상태에서 값을 변경하면 first의 소유권이 즉시 끝나서 더 이상 사용할 수 없음

    // 이는 v의 사용과 first의 사용이 겹치지 않도록하여 세이프하게 사용할 수 있음
    // println!("first = {}", first);

    println!("v = {:?}", v);



    let vi = vec![100, 32, 57];
    for i in &vi { // 불변 참조를 사용하여 벡터의 원소를 순회함
        println!("vi = {}", i);
    }

    let mut vv = vec![100, 32, 57];
    for i in &mut vv { // 가변 참조를 사용하여 벡터의 원소를 순회함
        *i += 50; // 가변 참조는 * 연산자를 사용하여 값을 변경 가능함
        println!("vv = {}", i);
    }



    enum SpreadsheetCell {
        Int(i32),
        Float(f64),
        Text(String),
    }
    
    let row = vec![ // 벡터는 모든 원소가 같은 타입이어야 하지만, enum을 사용하면 다양한 타입을 가질 수 있음
        SpreadsheetCell::Int(3),
        SpreadsheetCell::Text(String::from("blue")),
        SpreadsheetCell::Float(10.12),
    ];

    for cell in &row {
        match cell { // 벡터의 원소는 enum이므로 match를 사용하여 패턴 매칭을 할 수 있음
            SpreadsheetCell::Int(x) => println!("Int = {}", x),
            SpreadsheetCell::Float(x) => println!("Float = {}", x),
            SpreadsheetCell::Text(x) => println!("Text = {}", x),
        }
    }
}
