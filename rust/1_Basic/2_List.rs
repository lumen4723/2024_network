fn main() {
    let list1 = [1, 2, 3, 4, 5];
    for it in list1.iter() {
        println!("{}", it);
    }

    list1.iter().for_each(|x| println!("{}", x)); // 람다식으로도 가능하다



    let list2 = (1..10).collect::<Vec<i32>>(); // collect()는 반복자를 벡터로 만든다
    println!("{:?}", list2);

    // `iter()`를 사용하여 불변 참조자 반복자를 얻는다
    list2.iter().map(|x| x * 2).for_each(|x| print!("{} ", x));
    println!();

    // `into_iter()`를 사용하여 소유권을 얻는다
    list2.into_iter().map(|x| x * 2).for_each(|x| print!("{} ", x)); // 소유권을 사용
    println!();

    // println!("{:?}", list2); // list2는 소유권을 이미 사용 했으므로 더 이상 사용할 수 없다



    // fold == reduce
    let sum100_2 = (1..101).fold(0, |acc, x| acc + x); // |x + y| x + y 은 람다식이다
    println!("sum of 1 to 100: {}", sum100_2);



    let list3 = vec![1, 2, 3, 4, 5];
    // let list3 = vec!(1, 2, 3, 4, 5); // ()도 가능하다
    println!("{:?}", list3);

    let list4 = [1..10]; // 1부터 10까지 범위를 가진 배열
    println!("{:?}", list4);

    let list5 = vec!(1..10); // 벡터 내부에 범위를 가진 배열을 넣는다
    println!("{:?}", list5);


    
    // flatten()은 고차원 벡터를 일차원 벡터로 만든다
    let list6 = vec![1..10].into_iter().flatten().collect::<Vec<i32>>();
    println!("{:?}", list6);

    // flat_map()은 map()과 flatten()을 합친 것이다
    // flat 해서 x + 1한 결과를 벡터로 출력
    let list7 = vec![1..10].into_iter().flat_map(|x| x).map(|x| x + 1).collect::<Vec<i32>>();
    println!("{:?}", list7);
}