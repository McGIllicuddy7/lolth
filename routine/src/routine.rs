#[repr(C)]

pub struct JmpBuf {
    p: [i32; 48],
}
impl JmpBuf {
    pub fn new() -> Self {
        Self { p: [0; 48] }
    }
}

unsafe extern "C" {

    pub fn setjmp(buf: *mut JmpBuf) -> i32;
    pub fn longjmp(buf: *mut JmpBuf, v: i32);
}
pub struct Context {
    pub buf: JmpBuf,
}
pub fn context_switch(c1: &mut Context, c2: &mut Context) {}
pub fn spawn_context(c1: &mut Context, old: &mut Context, to_call: Box<dyn Fn()>) {}
