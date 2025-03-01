#ifndef VULKANBASECLASS_H
#define VULKANBASECLASS_H
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanBaseClass
{


public:



    bool is_desired_extensions_supported(const std::vector<VkExtensionProperties>& available_extensions,
                                         const std::vector<const char *>& desired_extensions);






    VulkanBaseClass();
    ~VulkanBaseClass();
};

#endif // VULKANBASECLASS_H
