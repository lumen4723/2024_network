use std::{
    io::prelude::*,
    net::TcpListener,
};

fn main() {
    // TCP 리스너(바인드 소켓)을 생성하고 12345 포트로 바인딩
    let listener = TcpListener::bind("0.0.0.0:12345").unwrap();

    // 비동기 for문은 알아서 연결을 수행할 것임
    for stream in listener.incoming() {
        let mut stream = stream.unwrap();

        println!("Client Connected");

        let mut buffer = [0; 1024];

        loop {
            // 무한 루프내에서 에코를 입출력하는 고전적인 에코 서버
            let readbytes = match stream.read(&mut buffer) {
                Ok(0) | Err(_) => {
                    break;
                }
                Ok(n) => n
            };

            if let Err(e) = stream.write(&buffer[0..readbytes]) {
                println!("Error: {}", e);
                break;
            }
            stream.flush().unwrap();
        }

        println!("Client Disconnected");
    }
}
