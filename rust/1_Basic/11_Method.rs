fn main() {
    struct Rectangle { // 사각형 구조체
        length: u32,
        width: u32,
    }
    
    impl Rectangle { // 구조체에 메소드 추가
        fn area(&self) -> u32 { // &self는 자신의 참조를 가져옴
            self.length * self.width // self.length는 self의 length 필드를 가져오고 리턴함
        }
    
        fn can_hold(&self, other: &Rectangle) -> bool {
            // 크기를 비교하여 self가 other보다 크면 true를 리턴
            self.length > other.length && self.width > other.width
        }
    
        fn square(size: u32) -> Rectangle { // 사각형을 생성하는 연관(associated) 함수
            // 연관 함수는 self 매개변수를 갖지 않는 메소드를 말함
            Rectangle { length: size, width: size }
        }
    }

    let rect1 = Rectangle { length: 50, width: 30 };

    println!(
        "rectangle.area() = {}",
        rect1.area()
    );



    let rect1 = Rectangle { length: 50, width: 30 };
    let rect2 = Rectangle { length: 40, width: 10 };
    let rect3 = Rectangle { length: 45, width: 60 };

    println!("Can rect1 hold rect2? {}", rect1.can_hold(&rect2));
    println!("Can rect1 hold rect3? {}", rect1.can_hold(&rect3));



    let sq = Rectangle::square(3); // 연관 함수 호출
    println!("sq.area() = {}", sq.area());
}