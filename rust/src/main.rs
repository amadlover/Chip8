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

impl CPU {
    fn new() -> CPU {
        println!("Hello CPU");

        CPU {
            memory: [0; 4096],
            vx: [0; 16],
            i: 0,

            delay_timer: 0,
            sound_timer: 0,
            pc: 0x200,
            sp: 0,

            stack: [0; 16],
        }
    }

    fn execute() {}
}

fn main() {
    
}
