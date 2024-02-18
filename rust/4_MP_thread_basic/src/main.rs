use std::{
    io::{Read, Write},
    net::{TcpListener, TcpStream},
    thread,
};

fn main() {
    let listener = TcpListener::bind("0.0.0.0:12345").unwrap();

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        println!("Client Connected");

        // 쓰레드 생성 (람다 클로저를 이용하여 소유권을 넘김)
        thread::spawn(move || {
            handle_client(stream);
        });
    }
}

fn handle_client(mut stream: TcpStream) {
    let mut buffer = [0; 1024];

    loop {
        let readbytes = match stream.read(&mut buffer) {
            Ok(0) | Err(_) => {
                break;
            }
            Ok(n) => n
        };

        if let Err(e) = stream.write(&buffer[0..readbytes]) {
            println!("Failed to write to socket; err = {:?}", e);
            break;
        }
        stream.flush().unwrap();
    }
    
    println!("Client Disconnected");
} 
