use std::{
    io::prelude::*,
    net::TcpListener,
};

fn main() {
    let listener = TcpListener::bind("0.0.0.0:12345").unwrap();

    for stream in listener.incoming() {
        let mut stream = stream.unwrap();

        println!("Client Connected");

        let mut buffer = [0; 1024];

        loop {
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
