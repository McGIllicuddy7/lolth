pub mod routine;
use crate::routine::{JmpBuf, longjmp, setjmp};

pub fn test(buf: *mut JmpBuf) {
    println!("lmao\n");
    unsafe {
        longjmp(buf, 1);
    }
}
fn main() {
    let mut j = JmpBuf::new();
    println!("Hello, world!");
    if unsafe { setjmp(&mut j) == 0 } {
        println!("bruh");
        test(&mut j);
        println!("unreachable");
    }
    println!("done");
}
