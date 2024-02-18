use std::{
    io::{prelude::*, BufReader},
    net::TcpListener,
    fs,
};

fn main() {
    let listener = TcpListener::bind("0.0.0.0:12345").unwrap();

    for stream in listener.incoming() {
        let mut stream = stream.unwrap();

        println!("Client Connected");

        // 버퍼 리더를 생성하고 스트림에서 읽어들임
        let buf_reader = BufReader::new(&mut stream);

        // 버퍼를 사용하기 편하게 벡터로 변환
        let readstr: Vec<_> = buf_reader
            .lines()
            .map(|result| result.unwrap())
            .take_while(|line| !line.is_empty())
            .collect();

        if readstr.is_empty() { continue; }
        // println!("Request: {:#?}", readstr);
        
        // HTTP 요청을 패턴 매칭하여 응답
        let sendstr = match readstr[0].as_str() {
            "GET / HTTP/1.1" => {
                "HTTP/1.1 200 OK\r\n\r\n<html><h1>Hello, World!</h1></html>".to_string()
            }
            "GET /html HTTP/1.1" => {
                let html = fs::read_to_string("index.html").unwrap();
                format!("HTTP/1.1 200 OK\r\n\r\n{html}")
            }
            _ => {
                "HTTP/1.1 404 NOT FOUND\r\n\r\n".to_string()
            }
        };

        if let Err(e) = stream.write(sendstr.as_bytes()) {
            println!("Failed to write to socket; err = {:?}", e);
            break;
        }

        println!("Client Disconnected");
    }
}
