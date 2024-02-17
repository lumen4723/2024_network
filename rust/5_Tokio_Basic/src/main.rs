use tokio::{
    io::{AsyncReadExt, AsyncWriteExt},
    net::TcpListener,
};

#[tokio::main]
async fn main() {
    let listener = TcpListener::bind("0.0.0.0:12345").await.unwrap();

    loop {
        let (mut sock, _ /*mut sockaddr*/) = listener.accept().await.unwrap();

        println!("Client Connected");

        tokio::spawn(async move {
            let mut buffer = [0; 1024];

            loop {
                let recvbytes = match sock.read(&mut buffer).await {
                    Ok(0) | Err(_)=> {
                        println!("Client Disconnected");
                        return;
                    }
                    Ok(n) => n
                };

                if let Err(e) = sock.write_all(&buffer[0..recvbytes]).await {
                    println!("Failed to write to socket; err = {:?}", e);
                    return;
                }
            }
        });
    }
}
