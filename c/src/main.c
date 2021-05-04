#include <stdio.h>

#include <Windows.h>
#include <stdlib.h>


int WINAPI wWinMain (_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE previous_instance, _In_ PWSTR cmd_line, _In_ int cmd_show)
{
    printf ("Hello Chip8\n");
    
    return EXIT_SUCCESS;
}