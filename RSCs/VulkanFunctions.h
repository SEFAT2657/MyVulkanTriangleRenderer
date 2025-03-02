#ifndef VULKANFUNCTIONS_H
#define VULKANFUNCTIONS_H
#endif // VULKANFUNCTIONS_H

#include "vulkan/vulkan.h"


namespace VulkanPFN {

 #define EXPORTED_VULKAN_FUNCTION( name ) extern PFN_##name name;
 #define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name  name;
 #define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
 #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;
 #define DEVICE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
 #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;


#include "ListOfVulkanFunctions.inl"

 } // namespace VulkanCookbook

