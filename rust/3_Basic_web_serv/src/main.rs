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

        let buf_reader = BufReader::new(&mut stream);

        let readstr: Vec<_> = buf_reader
            .lines()
            .map(|result| result.unwrap())
            .take_while(|line| !line.is_empty())
            .collect();

        if readstr.is_empty() { continue; }
        // println!("Request: {:#?}", readstr);
        
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
