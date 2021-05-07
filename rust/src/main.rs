struct CPU {
    memory: [u8; 4096],
    vx: [u8; 16],
    i: u16,

    delay_timer: u8,
    sound_timer: u8,

    pc: u16,
    sp: u16,

    stack: [u16; 16],
}

fn main() {
    println!("Hello, world!");
}
