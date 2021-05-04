#include <stdio.h>

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char path[256];
    char *opcodes;
} rom;

void* create_rom ()
{
    return calloc (1, sizeof (rom));
}

void read_rom_from_file (rom* rom_obj, char* path)
{
    FILE* file = fopen (path, "r");
    if (!file)
    {
        printf ("Failed to open %s\n", path);
        return;
    }
    int file_size = fseek (file, 0, SEEK_END);
    rewind (file);
    
    rom_obj->opcodes = (char*) malloc (sizeof (char) * file_size);
    fread (rom_obj->opcodes, sizeof (char), file_size, file);

    fclose (file);
}

void destroy_rom (rom* rom_obj)
{
    if (rom_obj != NULL)
    {
        if (rom_obj->opcodes != NULL)
        {
            free (rom_obj->opcodes);
        }
        
        free (rom_obj);
    }
}


int WINAPI wWinMain (_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE previous_instance, _In_ PWSTR cmd_line, _In_ int cmd_show)
{
    printf ("Hello Chip8\n");

    rom* rom_obj = (rom*)create_rom ();

    char* path = "./build32/vscode/debug/IBM_Logo.ch8";

    read_rom_from_file (rom_obj, path);

    destroy_rom (rom_obj);

    return EXIT_SUCCESS;
}