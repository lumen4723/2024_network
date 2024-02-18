use std::{
    io::{prelude::*, BufReader, stdin, stdout},
    net::TcpStream,
};

fn main() {
    let mut input = String::new();

    print!("Input server IP: ");
    stdout().flush().unwrap();

    stdin().read_line(&mut input).unwrap();

    input = input.trim().to_string() + ":12345";

    // TCP 스트림(커넥트 소켓)을 생성하고 서버에 연결
    let mut stream = TcpStream::connect(input).unwrap();

    let mut buffer = [0; 1024];

    loop {
        // 무한 루프내에서 에코를 입출력하는 고전적인 에코 클라이언트
        buffer = [0; 1024];

        print!("Input: ");
        stdout().flush().unwrap();

        stdin().read(&mut buffer).unwrap();
        
        if let Err(e) = stream.write(&buffer) {
            println!("Failed to write to socket; err = {:?}", e);
            break;
        }



        let mut reader = BufReader::new(&stream);

        if let Err(e) = reader.read(&mut buffer) {
            println!("Failed to read from socket; err = {:?}", e);
            break;
        }

        let msg = String::from_utf8_lossy(&buffer);

        print!("Echo: {}", msg);
        stdout().flush().unwrap();
    }

    // 우아한 종료
    // stream.shutdown(Shutdown::Both).unwrap();
}
