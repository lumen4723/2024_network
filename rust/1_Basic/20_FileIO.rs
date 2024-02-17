use std::io::Read; // .read_to_string() 함수를 사용하기 위해 추가함

fn main() {
    // rust는 실행할 떄 인자를 받을 수 있음
    let args: Vec<String> = std::env::args().collect();
    println!("args = {:?}", args);

    // 0번 인덱스는 실행 할 프로그램(실행 파일) 이름
    let query = &args[1]; // 1번 인덱스는 띄어쓰기로 구분된 첫번째 인자
    let filename = &args[2]; // 2번 인덱스는 띄어쓰기로 구분된 두번째 인자

    println!("Searching for {}", query);
    println!("In file {} \n", filename);



    // rust는 파일을 열고 읽는 기능을 제공함
    // 두번 째 인자를 파일이름으로 받아서 파일을 열고 읽음
    // 파일 경로는 실행파일(자신)을 기준으로 상대경로로 지정함 (예를들어 README.md는 ./README.md로 지정함)
    let mut f = std::fs::File::open(filename).expect("file not found");

    let mut contents = String::new();
    // 가변참조로 값을 받아오고
    f.read_to_string(&mut contents).expect("Something went wrong reading the file");

    // 이를 출력함
    println!("With text:\n{}", contents);



    // 또한 파일을 작성하는 기능도 제공함
    // 파일 경로는 실행파일(자신)을 기준으로 상대경로로 지정함 (예를들어 README.md는 ./README.md로 지정함)
    let mut f = std::fs::File::create("output.txt").expect("file not found");

    // 파일에 쓸 내용을 작성함
    std::fs::write("output.txt", "Hello, world!").expect("Unable to write file");

    // 파일을 닫음
    f.sync_all().expect("Unable to sync file");

    // 파일을 복사
    let result = std::fs::copy("output.txt", "output2.txt");
    match result {
        Ok(_) => println!("File copied successfully"),
        Err(error) => println!("Error: {}", error),
    }

    // 파일을 삭제
    let result = std::fs::remove_file("output.txt");
    match result {
        Ok(_) => println!("File deleted successfully"),
        Err(error) => println!("Error: {}", error),
    }


    // 폴더를 생성
    let result = std::fs::create_dir("output");
    match result {
        Ok(_) => println!("Folder created successfully"),
        Err(error) => println!("Error: {}", error),
    }

    // 폴더를 삭제
    let result = std::fs::remove_dir("output");
    match result {
        Ok(_) => println!("Folder deleted successfully"),
        Err(error) => println!("Error: {}", error),
    }

    // 폴더 복사와 이동은 조금 복잡해서 다른 라이브러리를 사용하는 것이 좋음

    // 표준 출력 대신 파일로 하고 싶다면 배시명령에 다음문법을 추가하면 됨
    // $ [실행할 파일이름] [인자1] [인자2] ... [인자n] > [출력할 파일이름]
    // > 다음에 나올 파일이름으로 출력을 저장함
}