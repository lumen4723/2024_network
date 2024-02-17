fn main() {
    use std::thread;
    use std::time::Duration;

    thread::spawn(|| { // 쓰레드를 생성하고, 클로저를 인자로 넘김
        for i in 1..5 {
            println!("hi number {} from the spawned thread!", i);
            thread::sleep(Duration::from_millis(1));
        }
    });

    for i in 1..5 { // 메인 쓰레드에서 실행
        println!("hi number {} from the main thread!", i);
        thread::sleep(Duration::from_millis(1));
    }
    println!("1. thread end\n");
    // 결과는 둘 다 동시에 실행되어 출력됨



    let handle = thread::spawn(|| { // 동일하게 쓰레드를 생성
        for i in 1..5 {
            println!("hi number {} from the spawned thread!", i);
            thread::sleep(Duration::from_millis(1));
        }
    });

    handle.join().unwrap(); // join을 호출하여 쓰레드가 종료될 때까지 기다림

    for i in 1..5 {
        println!("hi number {} from the main thread!", i);
        thread::sleep(Duration::from_millis(1));
    }
    println!("2. thread end\n");
    // 결과는 쓰레드가 먼저 실행되고, 쓰레드가 종료된 후에 메인이 실행됨



    let v = vec![1, 2, 3];

    // let handle = thread::spawn(|| { // 쓰레드로 같은 벡터를 참조한다면 Data race가 발생할 수 있음
    //     println!("Here's a vector: {:?}", v);
    // }); // rust는 이를 사전에 방지하기 위해 컴파일 에러를 발생시킴

    let handle = thread::spawn(move || { // move 키워드를 사용하여 쓰레드로 소유권을 넘김
        println!("Here's a vector: {:?}", v);
    }); // 이 경우에는 컴파일 에러가 발생하지 않음

    handle.join().unwrap();



    // Channel을 사용한 쓰레드 간 데이터 송수신
    // 해당 방식은 네트워크에서도 자주 쓰이는 채널링 방식이고, 원래는 PI-Calculus에서 유래됨
    
    use std::sync::mpsc; // mpsc는 multiple producer, single consumer의 약자
    let (tx, rx) = mpsc::channel(); // 채널을 생성하고, tx와 rx를 반환함

    thread::spawn(move || {
        let val = String::from("hi");
        tx.send(val).unwrap(); // tx로 send를 호출하여 데이터를 전송함

        // println!("val is {}", val); // val은 이후에 사용할 수 없음
        // 사실 tx.send(val)은 val의 소유권을 이동해서 전송하기 때문
    });

    let received = rx.recv().unwrap(); // rx로 recv를 호출하여 데이터를 수신함
    println!("Got: {}", received);



    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let vals = vec![ // 쓰레드에서 스트링 벡터를 생성 
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals { // 벡터의 각 요소를 채널로 전송
            tx.send(val).unwrap();
            thread::sleep(Duration::from_secs(1)); // 1초 대기
        }
    });

    for received in rx { // 메인 쓰레드에서 채널로부터 데이터를 수신
        println!("Got: {}", received);
    }
    // 사실 생각해보면 신기한 동작이다
    // 쓰레드는 1초마다 데이터를 전송하고, 메인 쓰레드는 채널로부터 데이터를 즉각 수신 중인데
    // 메인 쓰레드의 반복문은 쓰레드의 1초들을 기다리고, 모두 수신받고 나서야 종료된다
    println!("3. thread end\n");



    let (tx, rx) = mpsc::channel();

    let tx1 = tx.clone(); // 복수의 송신자를 생성하기 위해 tx를 복제함
    thread::spawn(move || {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx1.send(val).unwrap(); // tx1로 send를 호출하여 데이터를 전송함
            thread::sleep(Duration::from_secs(0));
        }
    });

    thread::spawn(move || {
        let vals = vec![
            String::from("more"),
            String::from("messages"),
            String::from("for"),
            String::from("you"),
        ];

        for val in vals {
            tx.send(val).unwrap(); // tx로 send를 호출하여 데이터를 전송함
            thread::sleep(Duration::from_secs(0));
        }
    });

    for received in rx { // tx, tx1 모두 rx로부터 데이터를 수신
        println!("Got: {}", received);
    }



    // Mutex를 사용한 쓰레드 간 데이터 공유
    use std::sync::Mutex; // Mutex를 사용하기 위함

    let m = Mutex::new(5); // Mutex를 생성하고, 5로 초기화함

    {
        let mut num = m.lock().unwrap(); // lock을 호출하여 뮤텍스의 잠금(락킹)을 획득함
        *num = 6; // 뮤텍스의 값을 변경함
    } // 뮤텍스의 잠금이 해제됨

    println!("m = {:?}", m); // 뮤텍스의 값을 출력함



    // let counter = Mutex::new(0);
    // let mut handles = vec![];

    // for _ in 0..10 { // loop를 10번 반복
    //     let handle = thread::spawn(move || { // move로 소유권을 넘김
    //         let mut num = counter.lock().unwrap(); // lock을 호출하여 뮤텍스의 잠금을 획득함

    //         *num += 1; // 뮤텍스의 값을 변경함
    //     });
    //     handles.push(handle); // 생성된 쓰레드의 핸들을 벡터에 추가함
    // } // 하지만, rust는 락(counter)의 소유권을 복수의 쓰레드에 넘기는 것을 허용하지 않음 error

    // use std::rc::Rc; // 고전적인 ref counting을 사용

    // let counter = Rc::new(Mutex::new(0)); // Rc와 Mutex를 사용하여 뮤텍스를 공유함
    // let mut handles = vec![];

    // for _ in 0..10 {
    //     let counter = Rc::clone(&counter); // Rc::clone을 호출하여 Rc의 소유권을 복제함
    //     let handle = thread::spawn(move || { // move로 소유권을 넘김
    //         let mut num = counter.lock().unwrap();

    //         *num += 1;
    //     });
    //     handles.push(handle);
    // } // 하지만, 여전히 락(counter)의 소유권을 복수의 쓰레드에 넘기는 것을 허용하지 않음
    // // 이유는 Rc는 여전히 쓰레드를 교차하면서 쓰기에는 안전하지 않기 때문
    // // 이 문제는 나중에 한 번 더 다룰 것이다

    use std::sync::Arc; // Atomic Reference Counting을 사용
    let counter = Arc::new(Mutex::new(0)); // Arc와 Mutex를 사용하여 뮤텍스를 공유함
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = Arc::clone(&counter); // Arc::clone을 호출하여 Arc의 소유권을 복제함
        let handle = thread::spawn(move || { // move로 소유권을 넘김
            let mut num = counter.lock().unwrap();

            *num += 1;
        });
        handles.push(handle);
    } // 아토믹 레퍼런스 카운팅은 여러 쓰레드에서 안전하게 공유할 수 있도록 해서 rust는 이를 허용함


    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());



    // Send 마커 트레잇을 사용한다면 소유권이 쓰레드 간에 이동이 가능해짐
    // Sync 마커 트레잇을 사용한다면 소유권을 쓰레드 간에 안전하게 참조가 가능해짐
}