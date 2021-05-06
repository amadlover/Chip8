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
    cpu_obj->memory[44] = 0xF0;

    // 9
    cpu_obj->memory[45] = 0xF0;
    cpu_obj->memory[46] = 0x90;
    cpu_obj->memory[47] = 0xF0;
    cpu_obj->memory[48] = 0x10;
    cpu_obj->memory[49] = 0xF0;

    // A
    cpu_obj->memory[50] = 0xF0;
    cpu_obj->memory[51] = 0x90;
    cpu_obj->memory[52] = 0xF0;
    cpu_obj->memory[53] = 0x90;
    cpu_obj->memory[54] = 0x90;

    // B
    cpu_obj->memory[55] = 0xE0;
    cpu_obj->memory[56] = 0x90;
    cpu_obj->memory[57] = 0xF0;
    cpu_obj->memory[58] = 0x10;
    cpu_obj->memory[59] = 0xE0;

    // C
    cpu_obj->memory[60] = 0xF0;
    cpu_obj->memory[61] = 0x80;
    cpu_obj->memory[62] = 0x80;
    cpu_obj->memory[63] = 0x80;
    cpu_obj->memory[64] = 0xF0;

    // D
    cpu_obj->memory[65] = 0xE0;
    cpu_obj->memory[66] = 0x90;
    cpu_obj->memory[67] = 0x90;
    cpu_obj->memory[68] = 0x90;
    cpu_obj->memory[69] = 0xE0;

    // E
    cpu_obj->memory[70] = 0xF0;
    cpu_obj->memory[71] = 0x80;
    cpu_obj->memory[72] = 0xF0;
    cpu_obj->memory[73] = 0x80;
    cpu_obj->memory[74] = 0xF0;

    // F
    cpu_obj->memory[75] = 0xF0;
    cpu_obj->memory[76] = 0x80;
    cpu_obj->memory[77] = 0xF0;
    cpu_obj->memory[78] = 0x80;
    cpu_obj->memory[79] = 0x80;
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
        OutputDebugString (L"Failed to open %s\n", path);  
        return;
    }

    fseek (file, 0, SEEK_END);
    int file_size = ftell (file);
    rewind (file);
    
    fread (cpu_obj->memory + 512, sizeof (unsigned char), file_size, file);

    fclose (file);

    for (int i = 512; i < 512 + file_size; i += 2)
    {
        OutputDebugString (L"%x %x\n", cpu_obj->memory[i], cpu_obj->memory[i + 1]);
    }
}

void cpu_destroy (cpu* cpu_obj)
{
    if (cpu_obj != NULL) 
    {
        free (cpu_obj);
    }
}

#define ID_GAME_TICK 1237

LRESULT CALLBACK WindowProc (HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_QUIT:
            PostQuitMessage (0);
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        case WM_CLOSE:
            PostQuitMessage (0);
            break;

        case WM_KEYDOWN:
            OutputDebugString (L"key down\n");
            break;

        case WM_KEYUP:
            OutputDebugString (L"key up\n");
            break;

        case WM_TIMER:
            OutputDebugString (L"timer\n");
            break;

        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint (h_wnd, &ps);
            RECT rect;
            GetClientRect (h_wnd, &rect);
            
            FillRect (hdc, &rect, CreateSolidBrush (RGB (0,0,0)));
            
            EndPaint (h_wnd, &ps);
           break;

        default:
            break;
    }
    
    return DefWindowProc (h_wnd, msg, w_param, l_param);
}

int WINAPI wWinMain (_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE previous_instance, _In_ PWSTR cmd_line, _In_ int cmd_show)
{
    OutputDebugString (L"Hello Chip8\n");

    char* path = "./build32/vscode/debug/IBM_Logo.ch8";

    cpu* cpu_obj = (cpu*)cpu_create ();
    cpu_create_font_set (cpu_obj);
    cpu_read_rom_from_file (cpu_obj, path);

    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = h_instance;
    wc.lpszClassName = L"Chip8";
    wc.hCursor = LoadCursor (h_instance, IDC_ARROW);

    if (!RegisterClass (&wc))
    {
        return EXIT_FAILURE;
    }

    HWND h_wnd = CreateWindow (
        L"Chip8", 
        L"Chip8",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, 
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1024,
        512, 
        NULL,
        NULL,
        h_instance,
        NULL
    );

    if (!h_wnd)
    {
        return EXIT_FAILURE;
    }

    ShowWindow (h_wnd, cmd_show);
    UpdateWindow (h_wnd);

    SetTimer (h_wnd, ID_GAME_TICK, 1000, NULL);

    MSG msg;
    ZeroMemory (&msg, sizeof (msg));

    while (
        msg.message != WM_QUIT &&
        msg.message != WM_CLOSE &&
        msg.message != WM_DESTROY)
    {
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
        else
        {
            // draw
        }
    }

    KillTimer (h_wnd, ID_GAME_TICK);
    DestroyWindow (h_wnd);

    cpu_destroy (cpu_obj);

    return EXIT_SUCCESS;
}