#include <stdio.h>

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdint.h>

#include <vulkan.h>

typedef struct _display
{
    unsigned char pixels[64 * 32];
} display;

void *display_create()
{
    return calloc(1, sizeof(display));
}

void display_init (display* display_obj)
{
    memset (display_obj->pixels, 0, sizeof (unsigned char) * 64 * 32);
}

void display_generate_diagonal(display *display_obj)
{
    for (char y = 0; y < 32; ++y)
    {
        for (char x = 0; x < 64; ++x)
        {
            if (x == (y * 2))
            {
                display_obj->pixels[y * 64 + x] = 1;
            }
            else
            {
                display_obj->pixels[y * 64 + x] = 0;
            }
        }
    }
}

void display_destroy(display *display_obj)
{
    if (display_obj != NULL)
    {
        free(display_obj);
    }
}

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

cpu* cpu_obj = NULL;
display* display_obj = NULL;

void cpu_create_font_set(cpu *cpu_obj)
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

void *cpu_create()
{
    return calloc(1, sizeof(cpu));
}

void cpu_init(cpu *cpu_obj)
{
    cpu_obj->pc = 0x200;
}

void cpu_read_rom_from_file(cpu *cpu_obj, char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        wchar_t buff[256];
        swprintf(buff, 256, L"Failed to open %hs\n", path);
        OutputDebugString(buff);
        return;
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    fread(cpu_obj->memory + 512, sizeof(unsigned char), file_size, file);

    fclose(file);

    for (int i = 512; i < 512 + file_size; i += 2)
    {
        wchar_t buff[256];
        swprintf(buff, 256, L"%02x %02x\n", cpu_obj->memory[i], cpu_obj->memory[i + 1]);
        OutputDebugString(buff);
    }
}

void cpu_execute(cpu *cpu_obj)
{
    unsigned short opcode = cpu_obj->memory[cpu_obj->pc] << 8 | cpu_obj->memory[cpu_obj->pc + 1];

    wchar_t buff[256];
    swprintf(buff, 256, L"Executing %02x %02x\n", cpu_obj->memory[cpu_obj->pc], cpu_obj->memory[cpu_obj->pc + 1]);
    OutputDebugString(buff);

    unsigned char opcode_higher_byte = cpu_obj->memory[cpu_obj->pc];
    unsigned char opcode_lower_byte = cpu_obj->memory[cpu_obj->pc + 1];

    if (opcode_higher_byte == 0x00)
    {
        if (opcode_lower_byte == 0xE0)
        {
            OutputDebugString(L"Clearing the screen\n");

            memset(display_obj->pixels, 0, sizeof(unsigned char) * 64 * 32);
            cpu_obj->pc += 2;
        }
        else if (opcode_lower_byte == 0xEE)
        {
            OutputDebugString(L"Returning from subroutine\n");
        }
    }
    else if ((opcode_higher_byte & 0xF0) == 0xA0)
    {
        unsigned short address = (opcode_higher_byte & 0x0F) << 8 | opcode_lower_byte;

        wchar_t buff[256];
        swprintf(buff, 256, L"Pushing %04x to I\n", address);
        OutputDebugString(buff);

        cpu_obj->i = address;

        cpu_obj->pc += 2;
    }
    else if ((opcode_higher_byte & 0xF0) == 0x60)
    {
        cpu_obj->vx[opcode_higher_byte & 0x0F] = opcode_lower_byte;

        wchar_t buff[256];
        swprintf(buff, 256, L"Putting %02x into V[%02x]\n", opcode_lower_byte, opcode_higher_byte & 0x0F);
        OutputDebugString(buff);

        cpu_obj->pc += 2;
    }
    else if ((opcode_higher_byte & 0xF0) == 0xD0)
    {
        wchar_t buff[256];
        swprintf(buff, 256, L"Drawing %02x bytes starting at %02x at %02x %02x\n", opcode_lower_byte & 0x0F, cpu_obj->i, cpu_obj->vx[opcode_higher_byte & 0x0F], cpu_obj->vx[opcode_lower_byte & 0xF0]);
        OutputDebugString(buff);

        unsigned char *bytes_to_display = (unsigned char *)malloc(opcode_lower_byte & 0x0F);
        memcpy(bytes_to_display, cpu_obj->memory + cpu_obj->i, opcode_lower_byte & 0x0F);

        display_obj->pixels[cpu_obj->vx[opcode_lower_byte & 0xF0] * 64 + cpu_obj->vx[opcode_higher_byte & 0x0F]] = 1;

        free(bytes_to_display);
        cpu_obj->pc += 2;
    }
    else if ((opcode_higher_byte & 0xF0) == 0x70)
    {
        wchar_t buff[256];
        swprintf(buff, 256, L"Setting vx[%02x] = vx[%02x] + %02x\n", opcode_higher_byte & 0x0F, opcode_higher_byte & 0x0F, opcode_lower_byte);
        OutputDebugString(buff);

        cpu_obj->vx[opcode_higher_byte & 0x0F] += opcode_lower_byte;

        cpu_obj->pc += 2;
    }
    else if ((opcode_higher_byte & 0xF0) == 0x10)
    {
        wchar_t buff[256];
        swprintf(buff, 256, L"Setting pc to %02x\n", (opcode_higher_byte & 0x0F) << 8 | opcode_lower_byte);
        OutputDebugString(buff);

        cpu_obj->pc = (opcode_higher_byte & 0x0F) << 8 | opcode_lower_byte;
    }

    memset(buff, 0, 256);
    swprintf(buff, 256, L"PC: %04x\n", cpu_obj->pc);
    OutputDebugString(buff);
}

void cpu_destroy(cpu *cpu_obj)
{
    if (cpu_obj != NULL)
    {
        free(cpu_obj);
    }
}

#define ID_GAME_TICK 1237

LRESULT CALLBACK WindowProc(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    RECT client_rect;
    GetClientRect(h_wnd, &client_rect);

    switch (msg)
    {
    case WM_QUIT:
        PostQuitMessage(0);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        OutputDebugString(L"key down\n");
        break;

    case WM_KEYUP:
        OutputDebugString(L"key up\n");
        break;

    case WM_TIMER:
        OutputDebugString(L"timer\n");
        cpu_execute(cpu_obj);
        InvalidateRect(h_wnd, &client_rect, TRUE);
        break;

    case WM_PAINT:
        OutputDebugString(L"Paint\n");
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h_wnd, &ps);

        int chip_8_to_win32_scale = 16;

        for (unsigned char y = 0; y < 32; ++y)
        {
            for (unsigned char x = 0; x < 64; ++x)
            {
                if (display_obj->pixels[y * 64 + x] == 1)
                {
                    wchar_t buff[256];
                    swprintf (buff, 256, L"1 %d %d\n", x, y);
                    OutputDebugString (buff);
                    RECT draw_rect;
                    draw_rect.left = chip_8_to_win32_scale * x;
                    draw_rect.top = chip_8_to_win32_scale * y;
                    draw_rect.bottom = chip_8_to_win32_scale * (y + 1);
                    draw_rect.right = chip_8_to_win32_scale * (x + 1);

                    FillRect(hdc, &draw_rect, CreateSolidBrush(RGB(255, 255, 255)));
                }
                else if (display_obj->pixels[y * 64 + x] == 0)
                {
                    /*wchar_t buff[256];
                    swprintf (buff, 256, L"0 %d %d\n", x, y);
                    OutputDebugString (buff);*/
                    RECT draw_rect;
                    draw_rect.left = chip_8_to_win32_scale * x;
                    draw_rect.top = chip_8_to_win32_scale * y;
                    draw_rect.bottom = chip_8_to_win32_scale * (y + 1);
                    draw_rect.right = chip_8_to_win32_scale * (x + 1);

                    FillRect(hdc, &draw_rect, CreateSolidBrush(RGB(0, 0, 0)));
                }
            }
        }

        EndPaint(h_wnd, &ps);
        break;

    default:
        break;
    }

    return DefWindowProc(h_wnd, msg, w_param, l_param);
}

int WINAPI wWinMain(_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE previous_instance, _In_ PWSTR cmd_line, _In_ int cmd_show)
{
    OutputDebugString(L"Hello Chip8\n");

    char *path = "./build32/vscode/debug/IBM_Logo.ch8";

    cpu_obj = (cpu*) cpu_create ();

    cpu_create_font_set (cpu_obj);
    cpu_read_rom_from_file (cpu_obj, path);
    cpu_init (cpu_obj);

    display_obj = (display*) display_create ();
    display_init (display_obj);
    display_generate_diagonal (display_obj);

    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = h_instance;
    wc.lpszClassName = L"Chip8";
    wc.hCursor = LoadCursor(h_instance, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        return EXIT_FAILURE;
    }

    HWND h_wnd = CreateWindow(
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
        NULL);

    if (!h_wnd)
    {
        return EXIT_FAILURE;
    }

    ShowWindow(h_wnd, cmd_show);
    UpdateWindow(h_wnd);

    SetTimer(h_wnd, ID_GAME_TICK, 1000, NULL);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    vulkan_init();

    while (
        msg.message != WM_QUIT &&
        msg.message != WM_CLOSE &&
        msg.message != WM_DESTROY)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    KillTimer(h_wnd, ID_GAME_TICK);
    DestroyWindow(h_wnd);

    vulkan_destroy();
    cpu_destroy (cpu_obj);
    display_destroy (display_obj);

    return EXIT_SUCCESS;
}