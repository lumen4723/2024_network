fn main() {
    let tweet = Tweet_1 {
        username: String::from("horse_ebooks"),
        content: String::from("of course, as you probably already know, people"),
        reply: false,
        retweet: false,
    };
    
    println!("1 new tweet: {}", tweet.summary());



    let article = NewsArticle_1 {
        headline: String::from("Penguins win the Stanley Cup Championship!"),
        location: String::from("Pittsburgh, PA, USA"),
        author: String::from("Iceburgh"),
        content: String::from("The Pittsburgh Penguins once again are the best"),
    };

    println!("New article available! {}", article.summary());



    let article = NewsArticle_2 {
        headline: String::from("Penguins win the Stanley Cup Championship!"),
        location: String::from("Pittsburgh, PA, USA"),
        author: String::from("Iceburgh"),
        content: String::from("The Pittsburgh Penguins once again are the best"),
    };

    println!("New article available! {}", article.summary());



    let article = NewsArticle_3 {
        headline: String::from("Penguins win the Stanley Cup Championship!"),
        location: String::from("Pittsburgh, PA, USA"),
        author: String::from("Iceburgh"),
        content: String::from("The Pittsburgh Penguins once again are the best"),
    };

    println!("New article available! {}", article.summary());



    let tweet = Tweet_2 {
        username: String::from("horse_ebooks"),
        content: String::from("of course, as you probably already know, people"),
        reply: false,
        retweet: false,
    };
    
    println!("1 new tweet: {}", tweet.summary());



    let number_list = vec![34, 50, 25, 100, 65];

    let result = largest_1(&number_list); // 내부 함수 값의 소유권 이동으로 result가 생성됨
    println!("The largest number is {}", result);

    let result = largest_2(&number_list); // 외부 인자의 소유권 복사로 result가 생성됨
    println!("The largest number is {}", result);
}

pub trait Summarizable_1 { // 퍼블릭 트레잇을 만들어서 다른 모듈에서도 사용할 수 있게 함
    fn summary(&self) -> String;
} // 추상 메소드와 비슷한 역할을 한다

pub struct Tweet_1 {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

pub struct NewsArticle_1 { // 구조체 생성
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summarizable_1 for NewsArticle_1 { // 특정 구조체에서 트레잇을 구현
    fn summary(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
} // 추상 메소드를 구상 메소드로 구현



impl Summarizable_1 for Tweet_1 {
    fn summary(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}

struct WeatherForecast {
    high_temp: f64,
    low_temp: f64,
    chance_of_precipitation: f64,
}

impl Summarizable_1 for WeatherForecast {
    fn summary(&self) -> String {
        format!("The high will be {}, and the low will be {}. The chance of
        precipitation is {}%.", self.high_temp, self.low_temp,
        self.chance_of_precipitation)
    }
}



pub trait Summarizable_2 { // 트레잇은 디폴트 구현을 할 수 있음
    fn summary(&self) -> String {
        String::from("(Read more...)")
    }
}

pub struct NewsArticle_2 {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

// 추상 메소드와 다른 점은 디폴트를 그대로 사용 할 수 있음
impl Summarizable_2 for NewsArticle_2 {}

pub struct NewsArticle_3 {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

// 여전히 디폴트를 사용할 수 있지만 오버라이딩으로 덮어쓸 수 있음
impl Summarizable_2 for NewsArticle_3 {
    fn summary(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub trait Summarizable_3 { // 트레잇은 여러개의 메소드를 가질 수 있음
    fn author_summary(&self) -> String; // 디폴트가 구현 안 됨

    fn summary(&self) -> String { // 디폴트가 구현 됨
        format!("(Read more from {}...)", self.author_summary())
    }
}



pub struct Tweet_2 {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summarizable_3 for Tweet_2 { // 트레잇을 구현할 때는 모든 메소드가 구상 메소드여야 함
    fn author_summary(&self) -> String { // 따라서 author_summary만 구현하면 사용 가능함
        format!("@{}", self.username)
    }
}

pub fn notify<T: Summarizable_3>(item: T) { // 트레잇은 제네릭과 함께 사용할 수 있음
    // 제네릭을 사용하면 최소한 Summarizable 이 보장된 T만 사용할 수 있음
    // 이를 수학적으로는 하한(Lower Bound)이라고 표현할 수 있음
    println!("Breaking news! {}", item.summary());
}

// fn some_function<T: A + B, U: B + C>(t: T, u: U) -> i32 {}
// 이렇게 여러개의 트레잇(A, B, C)을 사용할 수 있음
// 물론 트레잇 A, B, C가 모두 구현되어 있어야 함

// 이는 아래와 같이 where절로 표현할 수 있음
// fn some_function<T, U>(t: T, u: U) -> i32
//     where
//         T: A + B,
//         U: B + C
// {}



// 다시 돌아와서 비교 연산자를 사용하려면 PartialOrd 트레잇이 최소하한이 되어야 함
fn largest_1<T: std::cmp::PartialOrd>(list: &[T]) -> &T {
    let mut largest = &list[0];

    for item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

// 추가로 복사를 트레잇으로 두어 반환타입을 T로 할 수도 있음
// 이는 최소한 복사가 가능한 타입을 의미함 
fn largest_2<T: std::cmp::PartialOrd + Copy>(list: &[T]) -> T {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}