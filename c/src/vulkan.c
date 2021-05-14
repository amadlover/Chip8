#include "vulkan.h"
#include <vulkan/vulkan_win32.h>

#include <stdlib.h>

VkInstance instance;
VkSurfaceKHR surface;

void vulkan_init (HINSTANCE h_instance, HWND h_wnd)
{
    char* requested_instance_extensions[8] = {
        VK_KHR_SURFACE_EXTENSION_NAME, 
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        NULL,
        "Chip8",
        VK_MAKE_VERSION (1,0,0),
        "Chip8",
        VK_MAKE_VERSION (1,0,0),
        VK_MAKE_VERSION (1,0,0)
    };

    VkInstanceCreateInfo instance_create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        NULL,
        0,
        &application_info,
        0,
        NULL,
        2,
        requested_instance_extensions
    };

    vkCreateInstance (&instance_create_info, NULL, &instance);

    uint32_t num_physical_devices = 0;
    vkEnumeratePhysicalDevices (instance, &num_physical_devices, NULL);

    VkPhysicalDevice* physical_devices = (VkPhysicalDevice*) malloc (sizeof (VkPhysicalDevice) * num_physical_devices);
    vkEnumeratePhysicalDevices (instance, &num_physical_devices, physical_devices);

    VkPhysicalDevice physical_device = physical_devices[0];
    free (physical_devices);

	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures (physical_device, &device_features);

	uint32_t num_queue_families = 0;
	vkGetPhysicalDeviceQueueFamilyProperties (physical_device, &num_queue_families, NULL);
	VkQueueFamilyProperties* queue_family_properties = (VkQueueFamilyProperties*)malloc (sizeof (VkQueueFamilyProperties) * num_queue_families);
	vkGetPhysicalDeviceQueueFamilyProperties (physical_device, &num_queue_families, queue_family_properties);

    uint32_t graphics_queue_family_index = -1;
    uint32_t compute_queue_family_index = -1;
    uint32_t transfer_queue_family_index = -1;

	for (uint32_t i = 0; i < num_queue_families; ++i)
	{
		if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphics_queue_family_index = i;
			break;
		}
	}

	for (uint32_t i = 0; i < num_queue_families; ++i)
	{
		if (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && (i != graphics_queue_family_index))
		{
			compute_queue_family_index = i;
			break;
		}
	}

	if (compute_queue_family_index == -1)
	{
		for (uint32_t i = 0; i < num_queue_families; ++i)
		{
			if (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				compute_queue_family_index = i;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < num_queue_families; ++i)
	{
		if (queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && (i != graphics_queue_family_index) && (i != compute_queue_family_index))
		{
			transfer_queue_family_index = i;
			break;
		}
	}

	if (transfer_queue_family_index == -1)
	{
		for (uint32_t i = 0; i < num_queue_families; ++i)
		{
			if (queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				transfer_queue_family_index = i;
				break;
			}
		}
	}
    
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties (physical_device, &physical_device_memory_properties);

	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties (physical_device, &device_properties);
	VkPhysicalDeviceLimits physical_device_limits = device_properties.limits;

    free (queue_family_properties);

	VkWin32SurfaceCreateInfoKHR surface_create_info = {
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		NULL,
		0,
		h_instance,
		h_wnd
	};

	vkCreateWin32SurfaceKHR (instance, &surface_create_info, NULL, &surface);
}

void vulkan_destroy ()
{
	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR (instance, surface, NULL);
	}

    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance (instance, NULL);
    }
}