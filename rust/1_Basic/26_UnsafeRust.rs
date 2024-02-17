fn main() {
    // Rust는 안전한 언어이지만, unsafe 키워드를 사용하여 안전하지 않은 코드를 작성할 수 있음
    // 권장하진 않지만, 특정 상황에서는 안정성을 포기하고 써야할 일이 있을 수 있음

    // rust에서는 이를 안전하지 않은 슈퍼파워라고 부르기도 함
    // 1. 원시 포인터 (raw pointer) 역참조
    // 2. 안전하지 않은 함수나 메서드 호출
    // 3. 가변 정적 변수에 접근하거나 수정
    // 4. 안전하지 않은 트레잇 구현
    // 5. union 필드에 접근하기

    // unsafe를 썻다고 해서 검사기가 모두 비활성화되는 것은 아님
    // 단지, 위의 5가지 규칙만을 검사에서 제외하고, 나머지는 여전히 검사함



    // 1. 원시 포인터 (raw pointer) 역참조
    let mut num = 5;

    let r1 = &num as *const i32; // num의 불변 참조를 받아 원시 포인터(*const i32)로 변환함
    // 즉, 불변 원시 포인터를 생성함
    let r2 = &mut num as *mut i32; // num의 가변 참조를 받아 원시 포인터(*mut i32)로 변환함
    // 즉, 가변 원시 포인터를 생성함

    unsafe { // unsafe 블록을 사용하여 안전하지 않은 코드를 작성함
        println!("r1 is: {}", *r1); // 원시 포인터의 역참조는 일반적으로 안전하지 않음
        println!("r2 is: {}", *r2); // 원시 포인터의 역참조는 일반적으로 안전하지 않음
    } // 하지만, unsafe 블록에선 실행됨



    // 2. 안전하지 않은 함수나 메서드 호출
    unsafe fn dangerous() {} // unsafe 키워드를 사용하여 안전하지 않은 함수를 정의함

    unsafe { // unsafe 블록을 사용하여 안전하지 않은 코드를 작성함
        dangerous(); // 안전하지 않은 함수를 호출함
    } // 하지만, unsafe 블록에선 실행됨



    // 예를들면 아래 코드는 safe rust에서는 컴파일 에러가 발생함
    // 배열을 mid에서 나눠 두 개의 가변 참조를 반환하는 함수
    fn split_at_mut(values: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) {
        let len = values.len();
        
        // // 이 코드의 에러는 이 부분으로 동일한 슬라이스를 두 번 가변 참조 빌림하기 때문
        // (&mut values[..mid], &mut values[mid..])
        
        // 이를 해결하기 위해 unsafe 키워드를 사용하여 안전하지 않은 코드를 작성함
        unsafe {
            (
                std::slice::from_raw_parts_mut(values.as_mut_ptr(), mid),
                std::slice::from_raw_parts_mut(values.as_mut_ptr().offset(mid as isize), len - mid),
            )
        }
    }

    let mut numbers = [1, 2, 3, 4, 5];
    let (left, right) = split_at_mut(&mut numbers, 3);
    println!("left: {:?}", left);
    println!("right: {:?}", right);



    // 3. 가변 정적 변수에 접근하거나 수정
    static mut COUNTER: u32 = 0; // 가변 정적 변수를 생성함
    // 이는 가변으로 정의했지만 정적 변수이기 때문에 접근하고 수정하는 것은 안전하지 않음

    fn add_to_count(inc: u32) {
        unsafe {
            COUNTER += inc; // 가변 정적 변수를 수정함
        }
    }

    add_to_count(3);

    unsafe {
        println!("COUNTER: {}", COUNTER); // 가변 정적 변수를 접근함
    }



    // 4. 안전하지 않은 트레잇 구현
    unsafe trait Foo {} // unsafe 키워드를 사용하여 안전하지 않은 트레잇을 정의함

    unsafe impl Foo for i32 {} // unsafe 키워드를 사용하여 안전하지 않은 트레잇 구현을 정의함
    
    // 구체적인 예시는 안들지만, 2번과 유사한 방식이라는 것을 알 수 있음
    // 보통 Sync와 Send 트레잇을 구현할 때 사용함



    // 5. union 필드에 접근하기
    union MyUnion { // union을 정의함
        f1: u32,
        f2: f32,
    }

    let mu = MyUnion { f1: 42 }; // union을 생성함

    // println!("f1: {}", mu.f1); // union의 필드에 접근함 error
    // rust는 union의 필드에 접근 행위 자체를 unsafe로 간주함

    unsafe { // unsafe 블록을 사용하여 안전하지 않은 코드를 작성함
        println!("f1: {}", mu.f1); // union의 필드에 접근함
    } // 하지만, unsafe 블록에선 실행됨
}