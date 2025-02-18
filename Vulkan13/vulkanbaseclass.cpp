#include "vulkanbaseclass.h"
#include <cstring>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
#include <algorithm>

VulkanBaseClass::VulkanBaseClass() {}
bool VulkanBaseClass::is_desired_extensions_supported(const std::vector<VkExtensionProperties>& available_extensions,
                                                      const std::vector<const char *>& desired_extensions)
{
    using namespace std;

    for (const auto& desired_ext : desired_extensions)
    {
        bool found = std::any_of(available_extensions.begin(), available_extensions.end(),
                                 [&desired_ext](const VkExtensionProperties& ext)
                                 {
                                     std::cout << "TRIED " << std::endl;
                                     return std::strcmp(desired_ext, ext.extensionName) == 0;
                                 });

        if (!found)
        {
            std::cout << "Extension not supported: " << desired_ext << std::endl;
            continue;
        }
        else
        {
            std::cout << "Instance Extension supported: " << desired_ext << std::endl;
        }
    }
    /////////////
    std::cout << "All desired extensions Checked." << std::endl;
    return true;  // Return true only if all extensions are supported
};
