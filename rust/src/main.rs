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
    fn cpu() -> CPU {
        CPU {}
    }

    fn read_rom_from_file (&mut self, path: String) {
    
    }
}

fn main() {
    println!("Hello, world!");
}
