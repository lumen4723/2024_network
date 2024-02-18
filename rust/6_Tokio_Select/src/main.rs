use tokio::{
    net::{TcpListener, TcpStream},
    io::{AsyncReadExt, AsyncWriteExt},
    sync::{mpsc, Mutex},
    select,
};

use std::{
    sync::Arc,
    // net::SocketAddr,
};

// 이벤트 처리를 위한 열거형
enum Event {
    Read(Arc<Mutex<TcpStream>>), // 쓰레드 safe를 위해 Arc<Mutex<TcpStream>> 사용
    Write(Arc<Mutex<TcpStream>>, Vec<u8>),
}

#[tokio::main]
async fn main() {
    let listener = TcpListener::bind("0.0.0.0:12345").await.unwrap();
    // 채널링 개념을 사용하여 이벤트를 처리함
    let (tx, mut rx) = mpsc::channel(1024);

    loop {
        // select는 여러 비동기 이벤트를 동시에 처리할 수 있음
        select! {
            Ok((sock, _ /*addr*/)) = listener.accept() => {
                let tx = tx.clone();
                let sock = Arc::new(Mutex::new(sock));

                println!("Client Connected");

                tx.send(Event::Read(sock)).await.unwrap();
            }
            
            Some(event) = rx.recv() => {
                let tx = tx.clone();

                // 이벤트 기반으로 처리
                match event {
                    Event::Read(sock) => {
                        let sock_clone = Arc::clone(&sock);

                        // 비동기 입출력
                        tokio::spawn(async move {
                            let mut buffer = vec![0; 1024];
                            let mut sock = sock.lock().await;

                            match sock.read(&mut buffer).await {
                                Ok(0) | Err(_)=> {
                                    println!("Client Disconnected");
                                    return;
                                }
                                Ok(n) => {
                                    tx.send(
                                        Event::Write(sock_clone, buffer[..n].to_vec())
                                    ).await.unwrap();
                                }
                            }
                        });
                    }
                    Event::Write(sock, buffer) => {
                        let sock_clone = Arc::clone(&sock);
                        
                        // 비동기 입출력
                        tokio::spawn(async move {
                            let mut sock = sock.lock().await;

                            if let Err(_) = sock.write_all(&buffer).await {
                                println!("Error writing to socket");
                            }

                            tx.send(Event::Read(sock_clone))
                            .await.unwrap();
                        });
                    }
                }
            }
        }
    }
}
