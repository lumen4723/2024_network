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

    let mut stream = TcpStream::connect(input).unwrap();

    let mut buffer = [0; 1024];

    loop {
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

    // stream.shutdown(Shutdown::Both).unwrap();
}
