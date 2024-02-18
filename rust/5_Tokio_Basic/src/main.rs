use tokio::{
    io::{AsyncReadExt, AsyncWriteExt},
    net::TcpListener,
};

// tokio는 async/await를 사용하여 비동기 입출력을 지원함
#[tokio::main]
async fn main() {
    // 비동기 TCP 리스너(바인드 소켓)을 생성하고 12345 포트로 바인딩
    let listener = TcpListener::bind("0.0.0.0:12345").await.unwrap();

    loop {
        // 비동기 accept를 사용하여 연결을 수행할 것임
        let (mut sock, _ /*mut sockaddr*/) = listener.accept().await.unwrap();

        println!("Client Connected");

        // 비동기 spawn을 사용하여 쓰레드를 생성하고 소유권을 넘김
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
