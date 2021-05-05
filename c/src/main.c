#include <stdio.h>

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _cpu
{
    unsigned char memory[4096];

    unsigned char vx[16];
    unsigned char vf;
    unsigned short i;

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short pc;
    unsigned char sp;

    unsigned short stack[16];
} cpu;

void cpu_create_font_set (cpu* cpu_obj)
{
    // 0
    cpu_obj->memory[0] = 0xF0;
    cpu_obj->memory[1] = 0x90;
    cpu_obj->memory[2] = 0x90;
    cpu_obj->memory[3] = 0x90;
    cpu_obj->memory[4] = 0xF0;

    // 1
    cpu_obj->memory[5] = 0x20;
    cpu_obj->memory[6] = 0x60;
    cpu_obj->memory[7] = 0x20;
    cpu_obj->memory[8] = 0x20;
    cpu_obj->memory[9] = 0x70;

    // 2
    cpu_obj->memory[10] = 0xF0;
    cpu_obj->memory[11] = 0x10;
    cpu_obj->memory[12] = 0xF0;
    cpu_obj->memory[13] = 0x80;
    cpu_obj->memory[14] = 0xF0;

    // 3
    cpu_obj->memory[15] = 0xF0;
    cpu_obj->memory[16] = 0x10;
    cpu_obj->memory[17] = 0xF0;
    cpu_obj->memory[18] = 0x10;
    cpu_obj->memory[19] = 0xF0;

    // 4
    cpu_obj->memory[20] = 0x90;
    cpu_obj->memory[21] = 0x90;
    cpu_obj->memory[22] = 0xF0;
    cpu_obj->memory[23] = 0x10;
    cpu_obj->memory[24] = 0x10;

    // 5
    cpu_obj->memory[25] = 0xF0;
    cpu_obj->memory[26] = 0x80;
    cpu_obj->memory[27] = 0xF0;
    cpu_obj->memory[28] = 0x10;
    cpu_obj->memory[29] = 0xF0;

    // 6
    cpu_obj->memory[30] = 0xF0;
    cpu_obj->memory[31] = 0x80;
    cpu_obj->memory[32] = 0xF0;
    cpu_obj->memory[33] = 0x90;
    cpu_obj->memory[34] = 0xF0;

    // 7
    cpu_obj->memory[35] = 0xF0;
    cpu_obj->memory[36] = 0x10;
    cpu_obj->memory[37] = 0x20;
    cpu_obj->memory[38] = 0x40;
    cpu_obj->memory[39] = 0x40;

    // 8
    cpu_obj->memory[40] = 0xF0;
    cpu_obj->memory[41] = 0x90;
    cpu_obj->memory[42] = 0xF0;
    cpu_obj->memory[43] = 0xF9;
    cpu_obj->memory[42] = 0xF0;

    // 9
    cpu_obj->memory[43] = 0xF0;
    cpu_obj->memory[44] = 0x90;
    cpu_obj->memory[43] = 0xF0;
    cpu_obj->memory[44] = 0x10;
    cpu_obj->memory[45] = 0xF0;

    // A
    cpu_obj->memory[46] = 0xF0;
    cpu_obj->memory[47] = 0x90;
    cpu_obj->memory[48] = 0xF0;
    cpu_obj->memory[49] = 0x90;
    cpu_obj->memory[50] = 0x90;

    // B
    cpu_obj->memory[51] = 0xE0;
    cpu_obj->memory[52] = 0x90;
    cpu_obj->memory[53] = 0xF0;
    cpu_obj->memory[54] = 0x10;
    cpu_obj->memory[55] = 0xE0;

    // C
    cpu_obj->memory[56] = 0xF0;
    cpu_obj->memory[57] = 0x80;
    cpu_obj->memory[58] = 0x80;
    cpu_obj->memory[59] = 0x80;
    cpu_obj->memory[60] = 0xF0;

    // D
    cpu_obj->memory[61] = 0xE0;
    cpu_obj->memory[62] = 0x90;
    cpu_obj->memory[63] = 0x90;
    cpu_obj->memory[64] = 0x90;
    cpu_obj->memory[65] = 0xE0;

    // E
    cpu_obj->memory[66] = 0xF0;
    cpu_obj->memory[67] = 0x80;
    cpu_obj->memory[68] = 0xF0;
    cpu_obj->memory[69] = 0x80;
    cpu_obj->memory[70] = 0xF0;

    // F
    cpu_obj->memory[71] = 0xF0;
    cpu_obj->memory[72] = 0x80;
    cpu_obj->memory[73] = 0xF0;
    cpu_obj->memory[74] = 0x80;
    cpu_obj->memory[75] = 0x80;
}

void* cpu_create ()
{
    return calloc (1, sizeof (cpu));
}

void cpu_read_rom_from_file (cpu* cpu_obj, char* path)
{
    FILE* file = fopen (path, "r");
    if (!file)
    {
        printf ("Failed to open %s\n", path);
        return;
    }

    fseek (file, 0, SEEK_END);
    int file_size = ftell (file);
    rewind (file);
    
    fread (cpu_obj->memory + 512, sizeof (unsigned char), file_size, file);

    fclose (file);

    for (int i = 512; i < 512 + file_size; i += 2)
    {
        printf ("%x %x\n", cpu_obj->memory[i], cpu_obj->memory[i + 1]);
    }
}

void cpu_destroy (cpu* cpu_obj)
{
    if (cpu_obj != NULL) 
    {
        free (cpu_obj);
    }
}


int WINAPI wWinMain (_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE previous_instance, _In_ PWSTR cmd_line, _In_ int cmd_show)
{
    printf ("Hello Chip8\n");

    char* path = "./build32/vscode/debug/IBM_Logo.ch8";

    cpu* cpu_obj = (cpu*)cpu_create ();
    cpu_create_font_set (cpu_obj);
    cpu_read_rom_from_file (cpu_obj, path);
    cpu_destroy (cpu_obj);

    return EXIT_SUCCESS;
}