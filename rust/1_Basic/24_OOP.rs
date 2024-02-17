fn main() {
    // rust는 Hybird(잡종) 언어로 C++과 유사하게 OOP, FP, Procedural 등의 패러다임을 지원함
    // 그 중 OOP에서는 트레잇 오브젝트를 활용한 다형성을 지원함

    // 트레잇 오브젝트 예제
    trait Draw { // 트레잇을 정의함
        fn draw(&self); // 트레잇 메서드를 정의함
    }

    struct Screen {
        components: Vec<Box<dyn Draw>>, // Box<dyn Draw>는 트레잇 오브젝트를 나타냄
    }

    impl Screen {
        fn run(&self) {
            // self.components.iter()로 각 컴포넌트를 가져와서 draw() 메서드를 호출함
            for component in self.components.iter() {
                component.draw();
            }
        }
    }

    struct Button {
        width: u32,
        height: u32,
        label: String,
    }

    impl Draw for Button {
        //Draw 트레잇을 오버라이딩함
        fn draw(&self) {
            println!("Button: {}, {}, {}", self.width, self.height, self.label);
        }
    }

    struct Text {
        content: String,
    }

    impl Draw for Text {
        //Draw 트레잇을 오버라이딩함
        fn draw(&self) {
            println!("Text: {}", self.content);
        }
    }

    let screen = Screen {
        components: vec![
            Box::new(Button {
                width: 75,
                height: 10,
                label: String::from("OK"),
            }),
            Box::new(Text {
                content: String::from("This is a text"),
            }),
        ],
    };

    screen.run();



    // State 패턴 예제
    trait State { // 트레잇 State를 정의함
        // self: Box<Self>는 해당 메소드가 현재 객체의 소유권을 이동으로 받는다는 것을 의미함
        // Box<dyn State>은 트레잇 오브젝트로 dyn State는 해당 트레잇이 구현된 모든 객체를 나타냄
        fn request_review(self: Box<Self>) -> Box<dyn State>;
        fn approve(self: Box<Self>) -> Box<dyn State>; 
        fn content<'a>(&self, post: &'a Post) -> &'a str { // 라이프타임('a)을 사용하여 참조를 반환함
            ""
        }
    }
    
    struct Draft {} // 유닛 구조체 Draft를 정의함

    impl State for Draft { // Draft에 State 트레잇을 구현함
        fn request_review(self: Box<Self>) -> Box<dyn State> { 
            Box::new(PendingReview {}) // PendingReview로 상태를 변경함
        } 
        fn approve(self: Box<Self>) -> Box<dyn State> {
            self // Draft 상태에서는 approve를 호출해도 상태가 변경되지 않음
        }
    }

    struct PendingReview {} // 유닛 구조체 PendingReview를 정의함

    impl State for PendingReview {
        fn request_review(self: Box<Self>) -> Box<dyn State> {
            self // PendingReview 상태에서는 request_review를 호출해도 상태가 변경되지 않음
        }
        fn approve(self: Box<Self>) -> Box<dyn State> {
            Box::new(Published {}) // Published로 상태를 변경함
        }
    }

    struct Published {} // 유닛 구조체 Published를 정의함

    impl State for Published {
        fn request_review(self: Box<Self>) -> Box<dyn State> {
            self // Published 상태에서는 request_review를 호출해도 상태가 변경되지 않음
        }
        fn approve(self: Box<Self>) -> Box<dyn State> {
            self // Published 상태에서는 approve를 호출해도 상태가 변경되지 않음
        }
        fn content<'a>(&self, post: &'a Post) -> &'a str {
            &post.content // Published 상태에서는 content를 반환함
        }
    }

    struct Post { 
        state: Option<Box<dyn State>>, // State 트레잇 오브젝트를 Option으로 감싼다
        content: String,
    }

    impl Post {
        fn new() -> Post {
            Post {
                state: Some(Box::new(Draft {})), // Draft 상태로 초기화함
                content: String::new(),
            }
        }

        fn add_text(&mut self, text: &str) { // 텍스트를 추가하는 메서드를 정의함
            self.content.push_str(text);
        }

        fn content(&self) -> &str {
            // .as_ref()는 Option<T>에서 T에 대한 참조를 반환함
            // .unwrap()은 Option<T>에서 T를 반환함
            // .content(self)는 self의 content를 반환함
            self.state.as_ref().unwrap().content(self)
        }

        fn request_review(&mut self) {
            if let Some(s) = self.state.take() { // if let을 사용하여 self.state를 가져옴
                self.state = Some(s.request_review()); // request_review를 호출하여 상태를 변경함
            }
        }

        fn approve(&mut self) {
            if let Some(s) = self.state.take() { // if let을 사용하여 self.state를 가져옴
                self.state = Some(s.approve()); // approve를 호출하여 상태를 변경함
            }
        }
    }

    let mut post = Post::new();

    post.add_text("I ate a salad for lunch today");
    println!("1. {}", post.content());

    post.request_review();
    println!("2. {}", post.content());

    post.approve();
    println!("3. {}", post.content());
}