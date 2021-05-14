#pragma once

#include <vulkan/vulkan.h>
#include <Windows.h>

void vulkan_init (HINSTANCE h_instance, HWND h_wnd);
void vulkan_destroy ();