#include "vulkan.h"
#include <vulkan/vulkan_win32.h>

#include <stdlib.h>

VkInstance instance;
VkSurfaceKHR surface;
VkDevice graphics_device;
VkSwapchainKHR swapchain;
VkExtent2D current_extent;
VkSurfaceFormatKHR chosen_surface_format;
VkPresentModeKHR chosen_present_mode;

uint32_t swapchain_image_count = 0;
VkImage* swapchain_images = NULL;
VkImageView* swapchain_image_views = NULL;

void vulkan_init (HINSTANCE h_instance, HWND h_wnd)
{
    char* requested_instance_extensions[2] = {
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

    free (queue_family_properties);

	VkWin32SurfaceCreateInfoKHR surface_create_info = {
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		NULL,
		0,
		h_instance,
		h_wnd
	};

	vkCreateWin32SurfaceKHR (instance, &surface_create_info, NULL, &surface);

	VkBool32 is_surface_supported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR (physical_device, graphics_queue_family_index, surface, &is_surface_supported);

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR (physical_device, surface, &surface_capabilities);

	current_extent = surface_capabilities.currentExtent;
	uint32_t surface_format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR (physical_device, surface, &surface_format_count, NULL);

	VkSurfaceFormatKHR* surface_formats = (VkSurfaceFormatKHR*)malloc (sizeof (VkSurfaceFormatKHR) * surface_format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR (physical_device, surface, &surface_format_count, surface_formats);

	chosen_surface_format = surface_formats[0];

	uint32_t present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR (physical_device, surface, &present_mode_count, NULL);

	VkPresentModeKHR* present_modes = (VkPresentModeKHR*)malloc (sizeof (VkPresentModeKHR) * present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR (physical_device, surface, &present_mode_count, present_modes);

	for (uint32_t p = 0; p < present_mode_count; p++)
	{
		if (present_modes[p] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			chosen_present_mode = present_modes[p];
			break;
		}
	}

	char* device_extensions[1];
	device_extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	float priorities[3] = { 1.f, 1.f, 1.f };

	VkDeviceQueueCreateInfo queue_create_infos[3];
	uint32_t unique_queue_family_indices[3] = { 0,0,0 };
	uint32_t unique_queue_count[3] = { 1,1,1 };
	uint32_t unique_queue_family_index_count = 0;

	if (graphics_queue_family_index == compute_queue_family_index)
	{
		unique_queue_family_indices[0] = graphics_queue_family_index;
		++unique_queue_family_index_count;
		++unique_queue_count[0];
	}
	else
	{
		unique_queue_family_indices[0] = graphics_queue_family_index;
		unique_queue_family_indices[1] = compute_queue_family_index;
		unique_queue_family_index_count += 2;
	}

	if (graphics_queue_family_index != transfer_queue_family_index)
	{
		unique_queue_family_indices[unique_queue_family_index_count] = transfer_queue_family_index;
		++unique_queue_family_index_count;
	}

	for (uint32_t ui = 0; ui < unique_queue_family_index_count; ++ui)
	{
		queue_create_infos[ui].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_infos[ui].pNext = NULL;
		queue_create_infos[ui].pQueuePriorities = priorities;
		queue_create_infos[ui].queueCount = unique_queue_count[ui];
		queue_create_infos[ui].queueFamilyIndex = unique_queue_family_indices[ui];
		queue_create_infos[ui].flags = 0;
	}

	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures (physical_device, &device_features);

	VkDeviceCreateInfo device_create_info = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		NULL,
		0,
		unique_queue_family_index_count,
		queue_create_infos,
		0,
		NULL,
		1,
		device_extensions,
		&device_features
	};
	vkCreateDevice (physical_device, &device_create_info, NULL, &graphics_device);

	VkSwapchainCreateInfoKHR swapchain_create_info = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		NULL,
		0,
		surface,
		surface_capabilities.minImageCount + 1,
		chosen_surface_format.format,
		chosen_surface_format.colorSpace,
		surface_capabilities.currentExtent,
		1,
		surface_capabilities.supportedUsageFlags,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		NULL,
		surface_capabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		chosen_present_mode,
		1,
		VK_NULL_HANDLE
	};
	vkCreateSwapchainKHR (graphics_device, &swapchain_create_info, NULL, &swapchain);

	vkGetSwapchainImagesKHR (graphics_device, swapchain, &swapchain_image_count, NULL);
	swapchain_images = (VkImage*) malloc (sizeof (VkImage) * swapchain_image_count);
	vkGetSwapchainImagesKHR (graphics_device, swapchain, &swapchain_image_count, swapchain_images);

	swapchain_image_views = (VkImageView*) malloc (sizeof (VkImageView) * swapchain_image_count);

	VkImageSubresourceRange subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkComponentMapping component_mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageViewCreateInfo image_view_create_info = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		NULL,
		0,
		VK_NULL_HANDLE,
		VK_IMAGE_VIEW_TYPE_2D,
		chosen_surface_format.format,
		component_mapping,
		subresource_range
	};

	for (uint32_t i = 0; i < swapchain_image_count; ++i)
	{
		image_view_create_info.image = swapchain_images[i];
		vkCreateImageView (graphics_device, &image_view_create_info, NULL, swapchain_image_views + i);
	}
}

void vulkan_destroy ()
{
	free (swapchain_images);

	for (uint32_t i = 0; i < swapchain_image_count; ++i)
	{
		if (swapchain_image_views[i] != VK_NULL_HANDLE)
		{
			vkDestroyImageView (graphics_device, swapchain_image_views[i], NULL);
		}
	}

	free (swapchain_image_views);

	if (swapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR (graphics_device, swapchain, NULL);
	}
	
	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR (instance, surface, NULL);
	}

	if (graphics_device != VK_NULL_HANDLE)
	{
		vkDestroyDevice (graphics_device, NULL);
	}

    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance (instance, NULL);
    }
}