#include  "start.h"
#define   VK_NO_PROTOTYPES
#define   VK_USE_PLATFORM_WIN32_KHR
#include  <windows.h>
#include  <QApplication>
#include  <vulkan/vulkan.h>
#include  <iostream>
#include  "RSCs/VulkanFunctions.h"
#define   LoadFunction GetProcAddress
#define   MAKSIMUM_CHAR_Charecters 500
#include  <vector>
#include  <cstring>
#include  <fstream>
///GLM inclusion///
#include  <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>




using namespace VulkanPFN;
////////////////////////////////////////////////////////////////////////////////////////////////////
bool is_desired_extensions_supported(const std::vector<VkExtensionProperties>& available_extensions,
                                     const std::vector<const char*>& desired_extensions)
{
    // Iterate over all desired extensions
    for (const auto& desired_ext : desired_extensions)
    {
        // Check if the current desired extension is found in available extensions
        bool found = std::any_of(available_extensions.begin(), available_extensions.end(),
                                 [&desired_ext](const VkExtensionProperties& ext)
                                 {
                                     return std::strcmp(desired_ext, ext.extensionName) == 0;
                                 });

        // If not found, log the message and return false
        if (!found)
        {
            std::cout << "Extension not supported: " << desired_ext << std::endl;
            return false;  // Return false immediately if any desired extension is not supported
        }
        else
        {
            std::cout << "Instance Extension supported: " << desired_ext << std::endl;
        }
    }

    std::cout << "All desired extensions checked." << std::endl;
    return true;  // Return true only if all extensions are supported
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void print_bits_check_against(uint32_t& number, VkSurfaceCapabilitiesKHR& surface_capabilities,  VkImageUsageFlags& outflags){

    std::cout<<"The Desired usage flag Number given "<<number<<std::endl;
    std::cout<<"OUT image given "<<outflags<<std::endl;

    std::cout<<"desired outflags "<<number<<std::endl;
    std::cout<<"before image_usage "<<outflags<<std::endl;

    outflags = number & surface_capabilities.supportedUsageFlags;

    putchar('\n');
    std::cout<<"after discarding image_usage"<<outflags<<std::endl;

}


bool Vulkanloading()
{

    HMODULE vulkan_library= LoadLibraryW(L"vulkan-1.dll");
    if(vulkan_library == nullptr ){
        std::cout<<"There was an error when loading Vulkan"<<std::endl;
        return false;
    }   

    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr= (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkan_library, "vkGetInstanceProcAddr");
    if (vkGetInstanceProcAddr == nullptr ){
            std::cout<<"There was an error when loading the Instance Addr"<<std::endl;
    return false;
    }

///////////Global level function Loader////////////////////////////
    #define EXPORTED_VULKAN_FUNCTION( name )                       \
    name = (PFN_##name)LoadFunction( vulkan_library, #name );      \
    if( name == nullptr ) {                                        \
    std::cout << "Could not load exported Vulkan function named: " \
    #name << std::endl;                                            \
    return false;                                                  \
    }
///////////Global level function Loader////////////////////////////

    std::cout<< "All Vulkan functions loaded successfully." << std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;



    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties =reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceExtensionProperties"));
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties         =reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
    PFN_vkCreateInstance vkCreateInstance                                             =reinterpret_cast<PFN_vkCreateInstance>(vkGetInstanceProcAddr( nullptr, "vkCreateInstance" ));
    std::cout<<"Section 1 Normal"<<std::endl;

    if(vkEnumerateInstanceExtensionProperties == nullptr ||vkEnumerateInstanceLayerProperties == nullptr || vkCreateInstance == nullptr){
                std::cout<<"there was an issue with section 2"<<std::endl;
        return false;
    }
    std::cout<<"Section 2 Normal"<<std::endl;


/////////////////////Global Level FUNCTION//////////////////////
    #define GLOBAL_LEVEL_VULKAN_FUNCTION( name )                \
    name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name ); \
    if( name == nullptr ) {                                     \
    std::cout << "Could not load global-level function named: " \
    #name << std::endl;                                         \
    return false;                                               \
    }
    #include "ListOfVulkanFunctions.inl"
////////////////////Global Level FUNCTION///////////////////////

    std::vector<const char*> desired_extensions {
    };

    std::cout<<"-------------------------------------- " <<std::endl;

    for(int i=0; i<desired_extensions.size();i++){
        std::cout<<"desired extension: "<<i+1<<" "<<desired_extensions[i]<<std::endl;
    }

    std::cout<<"-------------------------------------- " <<std::endl;

    VkResult result=VK_SUCCESS;
    uint32_t instance_extensions_count = 0;
    std::vector<VkExtensionProperties> instance_available_extensions;

    result=vkEnumerateInstanceExtensionProperties(nullptr, &instance_extensions_count, nullptr);

    if (result != VK_SUCCESS || instance_extensions_count == 0) {
        std::cout << "Could't get the number extensions" << std::endl;
        return false;
    }
    std::cout<<"Count of available extension: "<<instance_extensions_count<<std::endl;
    instance_available_extensions.resize(instance_extensions_count);
    result=vkEnumerateInstanceExtensionProperties(nullptr, &instance_extensions_count, &instance_available_extensions[0]);
    //////Instance Second Call
    if (result != VK_SUCCESS || instance_extensions_count == 0) {
        std::cout << "Couldn't get the Extension infos" << std::endl;
        return false;
    }
    else{
        std::cout<<"-------------------------------------- " <<std::endl;
        for(int i=0; i< instance_extensions_count; ++i)
        {
            std::cout<<"Available Instance Extension Number "<<(i+1)<<": "<<instance_available_extensions[i].extensionName<<std::endl;
            desired_extensions.push_back(instance_available_extensions[i].extensionName);
        }
        std::cout<<"Desired extrensions loop exit count "<<desired_extensions.size()<<std::endl;
        is_desired_extensions_supported(instance_available_extensions,desired_extensions);

    }

    std::cout<<"-------------------------------------- " <<std::endl;
    const std::vector<const char*> validationLayer={"VK_LAYER_KHRONOS_validation"};

    VkApplicationInfo application_info;
    application_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext              = nullptr;
    application_info.pApplicationName   = "Vulkan13";
    application_info.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
    application_info.pEngineName        = "myvulkanengine";
    application_info.engineVersion      = VK_MAKE_API_VERSION(1, 0, 0, 0);
    application_info.apiVersion         = VK_MAKE_API_VERSION(1, 2, 0, 0);

    VkInstanceCreateInfo instance_create_info;
    instance_create_info.sType                  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext                  = nullptr;
    instance_create_info.flags                  = 0;
    instance_create_info.pApplicationInfo       = &application_info;
    instance_create_info.enabledLayerCount      = static_cast<uint32_t>(validationLayer.size());
    instance_create_info.ppEnabledLayerNames    = validationLayer.data();


    instance_create_info.enabledExtensionCount  = static_cast<uint32_t>(desired_extensions.size());
    instance_create_info.ppEnabledExtensionNames= desired_extensions.data();

    //istance section
    VkResult instanceResult = VK_SUCCESS;
    VkInstance instance;
    instanceResult = vkCreateInstance(&instance_create_info, nullptr, &instance);
    if (instanceResult != VK_SUCCESS || instance==VK_NULL_HANDLE){
    std::cout<< "Instance Creation Error"<<std::endl;
    }
    else{
        std::cout<< "Instance Creation Succesful"<<std::endl;
        std::cout<<"-------------------------------------- " <<std::endl;}



/////////////instance level function loader////////////////////////////
    #define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                      \
    name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );        \
    if( name == nullptr ) {                                             \
    std::cout << "Could not load instance-level Vulkan function named: "\
    #name << std::endl;                                                 \
    return false;                                                       \
    }
    #include "ListOfVulkanFunctions.inl"
/////////////instance level function loader////////////////////////////

    PFN_vkEnumeratePhysicalDevices               vkEnumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceProperties            vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceFeatures              vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkCreateDevice                           vkCreateDevice;
    PFN_vkGetDeviceProcAddr                      vkGetDeviceProcAddr;
    PFN_vkDestroyInstance                        vkDestroyInstance;
    PFN_vkEnumerateDeviceExtensionProperties     vkEnumerateDeviceExtensionProperties;
    PFN_vkCreateWin32SurfaceKHR                  vkCreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR     vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR      vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetSwapchainImagesKHR                   vkGetSwapchainImagesKHR;
    PFN_vkGetPhysicalDeviceMemoryProperties      vkGetPhysicalDeviceMemoryProperties;
    vkCreateWin32SurfaceKHR                 =(PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance,"vkCreateWin32SurfaceKHR");
    vkEnumeratePhysicalDevices              =(PFN_vkEnumeratePhysicalDevices)vkGetInstanceProcAddr(instance,"vkEnumeratePhysicalDevices");
    vkGetPhysicalDeviceProperties           =(PFN_vkGetPhysicalDeviceProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceFeatures             =(PFN_vkGetPhysicalDeviceFeatures)vkGetInstanceProcAddr(instance,"vkGetPhysicalDeviceFeatures");
    vkGetPhysicalDeviceQueueFamilyProperties=(PFN_vkGetPhysicalDeviceQueueFamilyProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkCreateDevice                          =(PFN_vkCreateDevice)vkGetInstanceProcAddr(instance,"vkCreateDevice");
    vkGetDeviceProcAddr                     =(PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(instance,"vkGetDeviceProcAddr");
    vkDestroyInstance                       =(PFN_vkDestroyInstance)vkGetInstanceProcAddr(instance,"vkDestroyInstance");
    vkEnumerateDeviceExtensionProperties    =(PFN_vkEnumerateDeviceExtensionProperties)vkGetInstanceProcAddr(instance,"vkEnumerateDeviceExtensionProperties");
    vkGetPhysicalDeviceSurfaceSupportKHR    =(PFN_vkGetPhysicalDeviceSurfaceSupportKHR) vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    vkGetPhysicalDeviceSurfacePresentModesKHR=(PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR=(PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(instance,"vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vkGetPhysicalDeviceSurfaceFormatsKHR     =(PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance,"vkGetPhysicalDeviceSurfaceFormatsKHR");
    vkGetSwapchainImagesKHR                  =(PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR");
    vkGetPhysicalDeviceMemoryProperties     =(PFN_vkGetPhysicalDeviceMemoryProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties");
    if(vkEnumeratePhysicalDevices==nullptr || vkGetPhysicalDeviceProperties == nullptr || vkGetPhysicalDeviceFeatures == nullptr
    || vkCreateDevice == nullptr ||  vkGetDeviceProcAddr == nullptr|| vkDestroyInstance == nullptr
    || vkEnumerateDeviceExtensionProperties == nullptr || vkGetPhysicalDeviceQueueFamilyProperties == nullptr
    || vkCreateWin32SurfaceKHR == nullptr || vkGetPhysicalDeviceSurfaceCapabilitiesKHR == nullptr || vkGetPhysicalDeviceMemoryProperties == nullptr)
    {std::cout<<"Section 3 Abnormal";
    return false;
    }


    std::cout<<"Section 3 Normal"<<std::endl;


    uint32_t devices_count=0;


    VkResult devResult1=VK_SUCCESS;
    devResult1= vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
    if(devResult1!=VK_SUCCESS)
        std::cout<<"There was an issue with Enumareting Devices First"<<std::endl;


    std::cout<<"Number of availabel device: "<<devices_count<<std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;

    std::vector<VkPhysicalDevice> available_devices;
    available_devices.resize(devices_count);


    devResult1=vkEnumeratePhysicalDevices(instance, &devices_count, &available_devices[0]);
    if(devResult1!=VK_SUCCESS)
        std::cout<<"There was an issue with Enumareting Devices Second Function";
    for(const auto ava_devices: available_devices)
    {
        std::cout<<"Available Device: " <<ava_devices<<std::endl;
    }



    VkPhysicalDevice physical_device= available_devices[0];
    std::vector<VkExtensionProperties> device_available_extensions;
    std::vector<const char*> device_desired_extensions;
    VkResult device_result2=VK_SUCCESS;
    uint32_t devices_extension_count=  0;

    device_result2=vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &devices_extension_count, nullptr);
    if(device_result2!=VK_SUCCESS){
        std::cout<<"there was an error with vkEnumerateDeviceExtensionProperties"<<std::endl;
    }

    device_available_extensions.resize(devices_extension_count);

    device_result2=vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &devices_extension_count, &device_available_extensions[0]);
    if(device_result2!=VK_SUCCESS)
        std::cout<<"there was an error with vkEnumerateDeviceExtensionProperties"<<std::endl;

    for(const auto ava_extensions: device_available_extensions)
    {
        char* a=new char[MAKSIMUM_CHAR_Charecters];
        strcpy(a,ava_extensions.extensionName);

        std::cout<<"Device available Extension: "<<ava_extensions.extensionName<<std::endl;
        device_desired_extensions.push_back(a);
    }
    std::cout<<"-------------------------------------- " <<std::endl;
//// device properties//////
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    uint32_t graphics_queue_family_index;
    uint32_t compute_queue_family_index;

    vkGetPhysicalDeviceFeatures(physical_device, &device_features);
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);

    std::cout<<device_properties.deviceName<<std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;

    if(device_features.geometryShader==VK_TRUE&& device_features.fillModeNonSolid==VK_TRUE)
    {
        device_features={};
        std::cout<<"Geo Shader is active"<<std::endl;
        device_features.geometryShader=VK_TRUE;
        device_features.fillModeNonSolid=VK_TRUE;
    }

/////////////////////////////////
    std::vector<VkQueueFamilyProperties> queue_families;
    uint32_t queue_families_count=0;

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
    if(queue_families_count<1)
    std::cout<<"Queue Familie error "<<std::endl;
    else
    std::cout<<"Queue Family Count: "<< queue_families_count<<std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;

    queue_families.resize(queue_families_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&queue_families_count, &queue_families[0]);




    struct QueueInfo {
        uint32_t FamilyIndex;
        std::vector<float> Priorities;
    };

    std::vector<QueueInfo> queue_infos;
    std::vector<uint32_t> qualified_indexies;
    uint32_t chosen_index;
    uint32_t chosen_index_queueCount;
    VkQueueFlags desired_capabilities=VK_QUEUE_GRAPHICS_BIT|VK_QUEUE_COMPUTE_BIT;

    for(uint32_t i;i<queue_families_count;i++){
        std::cout << "Queue Family " << i << ":" << std::endl;
        std::cout << "Queue Count: " << queue_families[i].queueCount << std::endl;

        VkQueueFlags queueFlags = queue_families[i].queueFlags;
        if (queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            std::cout << "  Supports Graphics Operations" << std::endl;
        }
        if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
            std::cout << "  Supports Compute Operations" << std::endl;
        }
        if (queueFlags & VK_QUEUE_TRANSFER_BIT) {
            std::cout << "  Supports Transfer Operations" << std::endl;
        }
        if (queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            std::cout << "  Supports Sparse Memory Operations" << std::endl;
        }
        if (queueFlags & VK_QUEUE_PROTECTED_BIT) {
            std::cout << "  Supports Protected Memory Operations" << std::endl;
        }
        std::cout << "--------------------------------------" << std::endl;
    }


    for(int i =0; i<queue_families_count ; i++ )
    {
            if(queue_families[i].queueCount>0 &&((queue_families[i].queueFlags & desired_capabilities)==desired_capabilities))
        {
            if( queue_families[i].queueFlags&VK_QUEUE_GRAPHICS_BIT && queue_families[i].queueFlags&VK_QUEUE_COMPUTE_BIT ) {
                    std::cout<<"Qeueu family: "<<i<<" does meet the requirement"<<std::endl;
                graphics_queue_family_index=i;
                compute_queue_family_index=i;
                qualified_indexies.emplace_back(i);

                QueueInfo queue_info;
                queue_info.FamilyIndex=i;
                queue_info.Priorities.resize(queue_families[i].queueCount,1.0f);
                chosen_index=queue_info.FamilyIndex;
                chosen_index_queueCount=queue_families[i].queueCount;

                std::cout<<"queue_families Qeueu Count: "<<queue_families[i].queueCount<<std::endl;
                queue_infos.push_back(queue_info);
                std::cout<<"queue_infos size:  " <<queue_infos.size()<<std::endl;
                std::cout<<"Inner IF WORKED " <<std::endl;

            }
            else{

            qualified_indexies.emplace_back(i);
            std::cout<<"Could be qualified Queue family Number: "<<i<<std::endl;
            std::cout<<"-------------------------------------- " <<std::endl;

            QueueInfo queue_info;
            queue_info.FamilyIndex=i;
            queue_info.Priorities.resize(queue_families[i].queueCount,1.0f);

            chosen_index_queueCount=queue_families[i].queueCount;
            queue_infos.push_back(queue_info);}

        }

        else{
            std::cout << "Queue family " << i << " does not meet the desired capabilities." << std::endl;
            }
    }





    std::cout<<"-------------------------------------- " <<std::endl;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    /////sized to how many queue familes that supports desired capabilities that we wanted which is that case only one supports all


    for(auto& a:queue_infos){
        VkDeviceQueueCreateInfo queueinfos_C_I;
        queueinfos_C_I.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueinfos_C_I.pNext=nullptr;
        queueinfos_C_I.flags=0;
        queueinfos_C_I.queueFamilyIndex=a.FamilyIndex;
        queueinfos_C_I.queueCount=static_cast<uint32_t>(a.Priorities.size());
        queueinfos_C_I.pQueuePriorities=a.Priorities.data();
        queue_create_infos.push_back(queueinfos_C_I);
    };

    /////////////////////////////////*there could be an eroror with this*\\\


    VkDeviceCreateInfo device_create_info{
        .sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext=nullptr,
        .flags=0,
        .queueCreateInfoCount=static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos=queue_create_infos.data(),
        .enabledLayerCount=0,
        .ppEnabledLayerNames=nullptr,
        .enabledExtensionCount=static_cast<uint32_t>(device_desired_extensions.size()),            //
        .ppEnabledExtensionNames=device_desired_extensions.data(),
        .pEnabledFeatures=&device_features                   ////
    };


    ///////////////// Logical Device Creation and enumerate and call all extension and base pointers/////////////////////

    VkDevice logical_device;
    if(vkCreateDevice(physical_device,&device_create_info, nullptr, &logical_device)!= VK_SUCCESS)
    std::cout<<"Logical Device Creation Error"<<std::endl;


    std::cout << "logical_device: "<<logical_device<<std::endl;


///// Dvice Level Function loader///////////////
    #define DEVICE_LEVEL_VULKAN_FUNCTION( name ) \
    name = (PFN_##name)vkGetDeviceProcAddr( logical_device, #name ); \
    if( name == nullptr ) { \
    std::cout << "Could not load device-level Vulkan function named: "\
    #name<< std::endl; \
    return false; \
    }
    #include "ListOfVulkanFunctions.inl"
///// Dvice Level Function loader///////////////
    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )  \
    for( auto & enabled_extension : desired_extensions ) {                  \
    if( std::string( enabled_extension ) == std::string( extension )) {     \
    name = (PFN_##name)vkGetDeviceProcAddr( logical_device, #name );        \
    if( name == nullptr ) {                                                 \
    std::cout << "Could not load device-level Vulkan function named: "      \
    #name << std::endl;                                                     \
    return false;                                                           \
    }                                                                       \
    }                                                                       \
    }
    #include "ListOfVulkanFunctions.inl"


//Dvice level PFNs
    PFN_vkGetDeviceQueue vkGetDeviceQueue                           =(PFN_vkGetDeviceQueue)vkGetDeviceProcAddr(logical_device,"vkGetDeviceQueue");
    PFN_vkDeviceWaitIdle vkDeviceWaitIdle                           =(PFN_vkDeviceWaitIdle)vkGetDeviceProcAddr(logical_device,"vkDeviceWaitIdle");
    PFN_vkDestroyDevice vkDestroyDevice                             =(PFN_vkDestroyDevice)vkGetDeviceProcAddr(logical_device,"vkDestroyDevice");
    PFN_vkCreateBuffer vkCreateBuffer                               =(PFN_vkCreateBuffer)vkGetDeviceProcAddr(logical_device,"vkCreateBuffer");
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements =(PFN_vkGetBufferMemoryRequirements)vkGetDeviceProcAddr(logical_device,"vkGetBufferMemoryRequirements");
    PFN_vkAllocateMemory vkAllocateMemory                           =(PFN_vkAllocateMemory)vkGetDeviceProcAddr(logical_device,"vkAllocateMemory");
    PFN_vkBindBufferMemory vkBindBufferMemory                       =(PFN_vkBindBufferMemory)vkGetDeviceProcAddr(logical_device,"vkBindBufferMemory");
    PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier                   =(PFN_vkCmdPipelineBarrier)vkGetDeviceProcAddr(logical_device,"vkCmdPipelineBarrier");
    PFN_vkCreateImage vkCreateImage                                 =(PFN_vkCreateImage)vkGetDeviceProcAddr(logical_device,"vkCreateImage");
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements   =(PFN_vkGetImageMemoryRequirements)vkGetDeviceProcAddr(logical_device,"vkGetImageMemoryRequirements");
    PFN_vkBindImageMemory vkBindImageMemory                         =(PFN_vkBindImageMemory)vkGetDeviceProcAddr(logical_device,"vkBindImageMemory");
    PFN_vkCreateImageView vkCreateImageView                         =(PFN_vkCreateImageView)vkGetDeviceProcAddr(logical_device,"vkCreateImageView");
    PFN_vkMapMemory vkMapMemory                                     =(PFN_vkMapMemory)vkGetDeviceProcAddr(logical_device,"vkMapMemory");
    PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges         =(PFN_vkFlushMappedMemoryRanges)vkGetDeviceProcAddr(logical_device,"vkFlushMappedMemoryRanges");
    PFN_vkUnmapMemory vkUnmapMemory                                 =(PFN_vkUnmapMemory)vkGetDeviceProcAddr(logical_device,"vkUnmapMemory");
    PFN_vkCmdCopyBuffer CmdCopyBuffer                               =(PFN_vkCmdCopyBuffer)vkGetDeviceProcAddr(logical_device,"CmdCopyBuffer");
    PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage               =(PFN_vkCmdCopyBufferToImage)vkGetDeviceProcAddr(logical_device,"vkCmdCopyBufferToImage");
    PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer               =(PFN_vkCmdCopyImageToBuffer)vkGetDeviceProcAddr(logical_device, "vkCmdCopyImageToBuffer");
    PFN_vkBeginCommandBuffer vkBeginCommandBuffer                   =(PFN_vkBeginCommandBuffer)vkGetDeviceProcAddr(logical_device, "vkBeginCommandBuffer");
    PFN_vkEndCommandBuffer vkEndCommandBuffer                       =(PFN_vkEndCommandBuffer)vkGetDeviceProcAddr(logical_device, "vkEndCommandBuffer");
    PFN_vkQueueSubmit vkQueueSubmit                                 =(PFN_vkQueueSubmit)vkGetDeviceProcAddr(logical_device, "vkQueueSubmit");
    PFN_vkDestroyImageView vkDestroyImageView                       =(PFN_vkDestroyImageView)vkGetDeviceProcAddr(logical_device, "vkDestroyImageView");
    PFN_vkDestroyImage vkDestroyImage                               =(PFN_vkDestroyImage)vkGetDeviceProcAddr(logical_device, "vkDestroyImage");
    PFN_vkDestroyBuffer vkDestroyBuffer                             =(PFN_vkDestroyBuffer)vkGetDeviceProcAddr(logical_device, "vkDestroyBuffer");
    PFN_vkFreeMemory vkFreeMemory                                   =(PFN_vkFreeMemory)vkGetDeviceProcAddr(logical_device, "vkFreeMemory");
    PFN_vkCreateCommandPool vkCreateCommandPool                     =(PFN_vkCreateCommandPool)vkGetDeviceProcAddr(logical_device, "vkCreateCommandPool");
    PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers           =(PFN_vkAllocateCommandBuffers)vkGetDeviceProcAddr(logical_device, "vkAllocateCommandBuffers");
    PFN_vkCreateSemaphore vkCreateSemaphore                         =(PFN_vkCreateSemaphore)vkGetDeviceProcAddr(logical_device, "vkCreateSemaphore");
    PFN_vkCreateFence vkCreateFence                                 =(PFN_vkCreateFence)vkGetDeviceProcAddr(logical_device, "vkCreateFence");
    PFN_vkWaitForFences vkWaitForFences                             =(PFN_vkWaitForFences)vkGetDeviceProcAddr(logical_device, "vkWaitForFences");
    PFN_vkResetFences vkResetFences                                 =(PFN_vkResetFences)vkGetDeviceProcAddr(logical_device, "vkResetFences");
    PFN_vkDestroyFence vkDestroyFence                               =(PFN_vkDestroyFence)vkGetDeviceProcAddr(logical_device, "vkDestroyFence");
    PFN_vkDestroySemaphore vkDestroySemaphore                       =(PFN_vkDestroySemaphore)vkGetDeviceProcAddr(logical_device, "vkDestroySemaphore");
    PFN_vkResetCommandBuffer vkResetCommandBuffer                   =(PFN_vkResetCommandBuffer)vkGetDeviceProcAddr(logical_device, "vkResetCommandBuffer");
    PFN_vkFreeCommandBuffers vkFreeCommandBuffers                   =(PFN_vkFreeCommandBuffers)vkGetDeviceProcAddr(logical_device, "vkFreeCommandBuffers");
    PFN_vkResetCommandPool vkResetCommandPool                       =(PFN_vkResetCommandPool)vkGetDeviceProcAddr(logical_device, "vkResetCommandPool");
    PFN_vkDestroyCommandPool vkDestroyCommandPool                   =(PFN_vkDestroyCommandPool)vkGetDeviceProcAddr(logical_device, "vkDestroyCommandPool");
    PFN_vkCreateBufferView vkCreateBufferView                       =(PFN_vkCreateBufferView)vkGetDeviceProcAddr(logical_device, "vkCreateBufferView");
    PFN_vkDestroyBufferView vkDestroyBufferView                     =(PFN_vkDestroyBufferView)vkGetDeviceProcAddr(logical_device, "vkDestroyBufferView");
    PFN_vkQueueWaitIdle vkQueueWaitIdle                             =(PFN_vkQueueWaitIdle)vkGetDeviceProcAddr(logical_device, "vkQueueWaitIdle");
    PFN_vkCreateSampler vkCreateSampler                             =(PFN_vkCreateSampler)vkGetDeviceProcAddr(logical_device, "vkCreateSampler");
    PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout     =(PFN_vkCreateDescriptorSetLayout)vkGetDeviceProcAddr(logical_device, "vkCreateDescriptorSetLayout");
    PFN_vkCreateDescriptorPool vkCreateDescriptorPool               =(PFN_vkCreateDescriptorPool)vkGetDeviceProcAddr(logical_device, "vkCreateDescriptorPool");
    PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets           =(PFN_vkAllocateDescriptorSets)vkGetDeviceProcAddr(logical_device, "vkAllocateDescriptorSets");
    PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets               =(PFN_vkUpdateDescriptorSets)vkGetDeviceProcAddr(logical_device, "vkUpdateDescriptorSets");
    PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets             =(PFN_vkCmdBindDescriptorSets)vkGetDeviceProcAddr(logical_device, "vkCmdBindDescriptorSets");
    PFN_vkFreeDescriptorSets vkFreeDescriptorSets                   =(PFN_vkFreeDescriptorSets)vkGetDeviceProcAddr(logical_device, "vkFreeDescriptorSets");
    PFN_vkResetDescriptorPool vkResetDescriptorPool                 =(PFN_vkResetDescriptorPool)vkGetDeviceProcAddr(logical_device, "vkResetDescriptorPool");
    PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool             =(PFN_vkDestroyDescriptorPool)vkGetDeviceProcAddr(logical_device, "vkDestroyDescriptorPool");
    PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout   =(PFN_vkDestroyDescriptorSetLayout)vkGetDeviceProcAddr(logical_device, "vkDestroyDescriptorSetLayout");
    PFN_vkDestroySampler vkDestroySampler                           =(PFN_vkDestroySampler)vkGetDeviceProcAddr(logical_device, "vkDestroySampler");
    PFN_vkCreateRenderPass vkCreateRenderPass                       =(PFN_vkCreateRenderPass)vkGetDeviceProcAddr(logical_device, "vkCreateRenderPass");
    PFN_vkCreateFramebuffer vkCreateFramebuffer                     =(PFN_vkCreateFramebuffer)vkGetDeviceProcAddr(logical_device, "vkCreateFramebuffer");
    PFN_vkDestroyFramebuffer vkDestroyFramebuffer                   =(PFN_vkDestroyFramebuffer)vkGetDeviceProcAddr(logical_device, "vkDestroyFramebuffer");
    PFN_vkDestroyRenderPass vkDestroyRenderPass                     =(PFN_vkDestroyRenderPass)vkGetDeviceProcAddr(logical_device, "vkDestroyRenderPass");
    PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass                   =(PFN_vkCmdBeginRenderPass)vkGetDeviceProcAddr(logical_device, "vkCmdBeginRenderPass");
    PFN_vkCmdNextSubpass vkCmdNextSubpass                           =(PFN_vkCmdNextSubpass)vkGetDeviceProcAddr(logical_device, "vkCmdNextSubpass");
    PFN_vkCmdEndRenderPass vkCmdEndRenderPass                       =(PFN_vkCmdEndRenderPass)vkGetDeviceProcAddr(logical_device, "vkCmdEndRenderPass");
    PFN_vkCreatePipelineCache vkCreatePipelineCache                 =(PFN_vkCreatePipelineCache)vkGetDeviceProcAddr(logical_device, "vkCreatePipelineCache");
    PFN_vkGetPipelineCacheData vkGetPipelineCacheData               =(PFN_vkGetPipelineCacheData)vkGetDeviceProcAddr(logical_device, "vkGetPipelineCacheData");
    PFN_vkMergePipelineCaches vkMergePipelineCaches                 =(PFN_vkMergePipelineCaches)vkGetDeviceProcAddr(logical_device, "vkMergePipelineCaches");
    PFN_vkDestroyPipelineCache vkDestroyPipelineCache               =(PFN_vkDestroyPipelineCache)vkGetDeviceProcAddr(logical_device, "vkDestroyPipelineCache");
    PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines         =(PFN_vkCreateGraphicsPipelines)vkGetDeviceProcAddr(logical_device, "vkCreateGraphicsPipelines");
    PFN_vkCreateComputePipelines vkCreateComputePipelines           =(PFN_vkCreateComputePipelines)vkGetDeviceProcAddr(logical_device, "vkCreateComputePipelines");
    PFN_vkDestroyPipeline vkDestroyPipeline                         =(PFN_vkDestroyPipeline)vkGetDeviceProcAddr(logical_device, "vkDestroyPipeline");
    PFN_vkDestroyEvent vkDestroyEvent                               =(PFN_vkDestroyEvent)vkGetDeviceProcAddr(logical_device, "vkDestroyEvent");
    PFN_vkDestroyQueryPool vkDestroyQueryPool                       =(PFN_vkDestroyQueryPool)vkGetDeviceProcAddr(logical_device, "vkDestroyQueryPool");
    PFN_vkCreateShaderModule vkCreateShaderModule                   =(PFN_vkCreateShaderModule)vkGetDeviceProcAddr(logical_device, "vkCreateShaderModule");
    PFN_vkDestroyShaderModule vkDestroyShaderModule                 =(PFN_vkDestroyShaderModule)vkGetDeviceProcAddr(logical_device, "vkDestroyShaderModule");
    PFN_vkCreatePipelineLayout vkCreatePipelineLayout               =(PFN_vkCreatePipelineLayout)vkGetDeviceProcAddr(logical_device, "vkCreatePipelineLayout");
    PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout             =(PFN_vkDestroyPipelineLayout)vkGetDeviceProcAddr(logical_device, "vkDestroyPipelineLayout");
    PFN_vkCmdBindPipeline vkCmdBindPipeline                         =(PFN_vkCmdBindPipeline)vkGetDeviceProcAddr(logical_device, "vkCmdBindPipeline");
    PFN_vkCmdSetViewport vkCmdSetViewport                           =(PFN_vkCmdSetViewport)vkGetDeviceProcAddr(logical_device, "vkCmdSetViewport");
    PFN_vkCmdSetScissor vkCmdSetScissor                             =(PFN_vkCmdSetScissor)vkGetDeviceProcAddr(logical_device, "vkCmdSetScissor");
    PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers               =(PFN_vkCmdBindVertexBuffers)vkGetDeviceProcAddr(logical_device, "vkCmdBindVertexBuffers");
    PFN_vkCmdDraw vkCmdDraw                                         =(PFN_vkCmdDraw)vkGetDeviceProcAddr(logical_device, "vkCmdDraw");
    PFN_vkCmdDrawIndexed vkCmdDrawIndexed                           =(PFN_vkCmdDrawIndexed)vkGetDeviceProcAddr(logical_device, "vkCmdDrawIndexed");
    PFN_vkCmdDispatch vkCmdDispatch                                 =(PFN_vkCmdDispatch)vkGetDeviceProcAddr(logical_device, "vkCmdDispatch");
    PFN_vkCmdCopyImage vkCmdCopyImage                               =(PFN_vkCmdCopyImage)vkGetDeviceProcAddr(logical_device, "vkCmdCopyImage");
    PFN_vkCmdPushConstants vkCmdPushConstants                       =(PFN_vkCmdPushConstants)vkGetDeviceProcAddr(logical_device, "vkCmdPushConstants");
    PFN_vkCmdClearColorImage vkCmdClearColorImage                   =(PFN_vkCmdClearColorImage)vkGetDeviceProcAddr(logical_device, "vkCmdClearColorImage");
    PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage     =(PFN_vkCmdClearDepthStencilImage)vkGetDeviceProcAddr(logical_device, "vkCmdClearDepthStencilImage");
    PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer                   =(PFN_vkCmdBindIndexBuffer)vkGetDeviceProcAddr(logical_device, "vkCmdBindIndexBuffer");
    PFN_vkCmdSetLineWidth vkCmdSetLineWidth                         =(PFN_vkCmdSetLineWidth)vkGetDeviceProcAddr(logical_device, "vkCmdSetLineWidth");
    PFN_vkCmdSetDepthBias vkCmdSetDepthBias                         =(PFN_vkCmdSetDepthBias)vkGetDeviceProcAddr(logical_device, "vkCmdSetDepthBias");
    PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants               =(PFN_vkCmdSetBlendConstants)vkGetDeviceProcAddr(logical_device, "vkCmdSetBlendConstants");
    PFN_vkCmdExecuteCommands vkCmdExecuteCommands                   =(PFN_vkCmdExecuteCommands)vkGetDeviceProcAddr(logical_device, "vkCmdExecuteCommands");
    PFN_vkCmdClearAttachments vkCmdClearAttachments                 =(PFN_vkCmdClearAttachments)vkGetDeviceProcAddr(logical_device, "vkCmdClearAttachments");
    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR                   =(PFN_vkCreateSwapchainKHR)vkGetDeviceProcAddr(logical_device, "vkCreateSwapchainKHR");
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR                 =(PFN_vkAcquireNextImageKHR)vkGetDeviceProcAddr(logical_device,"vkAcquireNextImageKHR");
    PFN_vkQueuePresentKHR vkQueuePresentKHR                         =(PFN_vkQueuePresentKHR)vkGetDeviceProcAddr(logical_device,"vkQueuePresentKHR");


    ////////////////////////////////////////////////////////////////////////
    
    if(vkAcquireNextImageKHR==nullptr||vkQueuePresentKHR==nullptr)
    {
        std::cout<<"vkQueuePresentKHR Error"<<std::endl;
        return false;
    }


    VkQueue graphics_queue;
    VkQueue compute_queue;

    vkGetDeviceQueue(logical_device,chosen_index,0,&graphics_queue);
    vkGetDeviceQueue(logical_device,chosen_index,0,&compute_queue);

    struct WindowParameters{
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        HINSTANCE HInstance;
        HWND HWnd;
    #endif
    };

    WindowParameters window_parameters;
    window_parameters.HInstance=GetModuleHandle(nullptr);
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = DefWindowProc;  // Default window procedure
    wc.hInstance = window_parameters.HInstance;
    wc.lpszClassName = CLASS_NAME;  // Class name
    RegisterClassEx(&wc);


    window_parameters.HWnd=CreateWindowExW(0,CLASS_NAME,        // name of window class
                                             L"Sample",            // title-bar string
                                             WS_OVERLAPPEDWINDOW, // top-level window
                                             CW_USEDEFAULT,       // default horizontal position
                                             CW_USEDEFAULT,       // default vertical position
                                             1000,       // default width
                                             1000,       // default height
                                             (HWND) NULL,         // no owner window
                                             (HMENU) NULL,        // use class menu
                                             window_parameters.HInstance,           // handle to application instance
                                             (LPVOID) NULL);

    ////////Windwos32 HWND used to create a window and names and other properties filled

    VkSurfaceKHR presentation_surface=VK_NULL_HANDLE;

    VkWin32SurfaceCreateInfoKHR surface_create_info ={
        .sType=VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext=nullptr,
        .flags=0,
        .hinstance=window_parameters.HInstance,
        .hwnd=window_parameters.HWnd
    };

    ShowWindow(window_parameters.HWnd,SW_SHOW);

    if(vkCreateWin32SurfaceKHR(instance,&surface_create_info,nullptr,&presentation_surface)!=VK_SUCCESS){
        std::cout<<"vkCreateWin32SurfaceKHR Creation Error"<<std::endl;
    return false;}


    std::cout << "Physical Device: " << physical_device << std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;
    ////
    /// \brief presentation_supported
    ///
    VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
    debugInfo.sType          = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity=VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType    =VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;


    VkBool32 presentation_supported={};
    VkPresentModeKHR desired_present_mode=VK_PRESENT_MODE_MAILBOX_KHR;
    uint32_t present_modes_count;
    std::vector<VkPresentModeKHR> present_modes;
    VkPresentModeKHR present_mode={};
    uint32_t presentQueueFamilyIndex;
    for(int index=0;index<queue_families_count;index++){

        VkResult vkCreateWin32SurfaceKHR_result = vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device,
            index,
            presentation_surface,
            &presentation_supported);

        if (vkCreateWin32SurfaceKHR_result != VK_SUCCESS || presentation_supported == VK_FALSE) {
            std::cout << "Error occurred while checking surface support: " << result << std::endl;
        }

        else {
            std::cout << "Presentation is supported at index presentation_supported: "<<index << std::endl;
        }
        if(presentation_supported==VK_TRUE && queue_families[index].queueFlags &VK_QUEUE_GRAPHICS_BIT){
            std::cout<<"index :"<<index<< "Supports both Graphics Bit and Presentation Support"<<std::endl;
            presentQueueFamilyIndex=index;
        }

    }
    result={};

    if(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,presentation_surface,&present_modes_count, nullptr) !=VK_SUCCESS)
        std::cout << "Presentation ERROR " << std::endl;
    ///////////First call vkGetPhysicalDeviceSurfacePresentModesKHR call

    present_modes.resize(present_modes_count);

    std::cout<<"-------------------------------------- " <<std::endl;
    std::cout << "Presentation modes count "<<present_modes_count << std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;

    if(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,presentation_surface,&present_modes_count, &present_modes[0]) !=VK_SUCCESS)
        std::cout << "Presentation ERROR " << std::endl;
    ///////////Second vkGetPhysicalDeviceSurfacePresentModesKHR call
    int index_over_present_modes=0;
    for( auto &in_iteration: present_modes)
    {
        if(in_iteration==desired_present_mode){
        std::cout<< "Presentation mode is supported:"<<index_over_present_modes<< std::endl;
            present_mode=desired_present_mode;            
        }
        else{
            std::cout << "Presentation modes is not supported at index: "<<index_over_present_modes<<std::endl;
        }


        index_over_present_modes++;
    }
    if(desired_present_mode==VK_PRESENT_MODE_MAILBOX_KHR)
    {
        std::cout<<"presentation mode VK_PRESENT_MODE_MAILBOX_KHR"<<present_mode<<std::endl;
    }
    else{
        present_mode=VK_PRESENT_MODE_FIFO_KHR;
        std::cout<<"presentation mode VK_PRESENT_MODE_FIFO_KHR"<<present_mode<<std::endl;
    }

    std::cout<<"-------------------------------------- " <<std::endl;

////////////iterate voer presenVkVtation modes and select current mode with desired mode used to qualify
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkImageUsageFlags desired_usages;
    VkExtent2D size_of_images;
    VkImageUsageFlags image_usage=0;
    desired_usages=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT |VK_IMAGE_USAGE_SAMPLED_BIT ;


    result = {};
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device,presentation_surface,&surface_capabilities);
    if(result != VK_SUCCESS || surface_capabilities.maxImageCount==0) {
        std::cout << "surface_capabilities error"<<std::endl;
        return false;
    }

    uint32_t number_of_images = surface_capabilities.minImageCount+1;
    std::cout<<"maximum swapchain image count "<<surface_capabilities.maxImageCount<<std::endl;
    if(number_of_images>surface_capabilities.maxImageCount) {
        std::clamp(number_of_images,surface_capabilities.minImageCount,surface_capabilities.maxImageCount);
        std::cout << number_of_images<<"  "<<surface_capabilities.maxImageCount<<std::endl;
    }
    else{
        std::cout << "number_of_images: "<<number_of_images<<","<<" surface_capabilities.maxImageCount: "<<surface_capabilities.maxImageCount<<std::endl;
    }

    ///// clamped the swapchain size if  it exceed size clamp i to the highest

    if (surface_capabilities.currentExtent.width != UINT32_MAX) {
        // Vulkan allows us to set the extent, so we clamp it within valid range
        size_of_images = surface_capabilities.currentExtent;
        std::cout << "Using required swapchain extent: " << size_of_images.width << "x" << size_of_images.height << std::endl;


    }
    else {
        // Vulkan specifies an exact extent, so we must use it
        size_of_images.width = std::clamp(size_of_images.width,
                                          surface_capabilities.minImageExtent.width,
                                          surface_capabilities.maxImageExtent.width);
        size_of_images.height = std::clamp(size_of_images.height,
                                           surface_capabilities.minImageExtent.height,
                                           surface_capabilities.maxImageExtent.height);
        std::cout << "Using custom image extent: " << size_of_images.width << "x" << size_of_images.height << std::endl;

    }

    std::cout << "desired_usages: "<<desired_usages<<std::endl;
    std::cout << "surface_capabilies: "<<surface_capabilities.supportedUsageFlags<<std::endl;
    print_bits_check_against(desired_usages,surface_capabilities, image_usage);
    std::cout<<"-------------------------------------- " <<std::endl;

   ///// Surface Transform
    VkSurfaceTransformFlagBitsKHR desired_transform=VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkSurfaceTransformFlagBitsKHR surface_transform=surface_capabilities.currentTransform;

    std::cout <<"Surface_capabilites: "<< surface_capabilities.currentTransform<<std::endl;
//////////////Surface Color Space and Format
    VkSurfaceFormatKHR desired_surface_format{
        .format=VK_FORMAT_B8G8R8A8_UNORM,
        .colorSpace=VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    };
    ///
    /// \MY desired formats surface_formats
    ///
    std::vector<VkSurfaceFormatKHR> surface_formats;
    VkSurfaceFormatKHR surface_format;
    uint32_t           formats_count;
    ///
    ///
    ///
    VkFormat           image_format;
    VkColorSpaceKHR    image_color_space;
    ///
    /// vkGetPhysicalDeviceSurfaceFormatsKHR
    ///


    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,presentation_surface,&formats_count,nullptr);
    std::cout<<"Presentation surface Format Count: "<<formats_count<<std::endl;
    surface_formats.resize(formats_count);
    std::cout<<"-------------------------------------- " <<std::endl;

    if(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,presentation_surface,&formats_count,&surface_formats[0])!=VK_SUCCESS|| surface_formats.size()==1)
    std::cout<<"vkGetPhysicalDeviceSurfaceFormatsKHR error"<<std::endl;

    std::cout<<"surface format size is "<<surface_formats.size()<<std::endl;
    std::cout<<"-------------------------------------- " <<std::endl;

    if(surface_formats.size()==1&&surface_formats[0].format==VK_FORMAT_UNDEFINED)
    {
        desired_surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
        desired_surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        image_format = desired_surface_format.format;
        image_color_space = desired_surface_format.colorSpace;

        std::cout << "Chosen format: " << image_format << " and color space: " << image_color_space << std::endl;


    }
    else{ std::cout << "Surfaceformats size is not 1" << std::endl; }
    ////in situation where surface format has only 1 member and its called VK_FORMAT_UNDEFINED

    bool match;
    int surfaceformatindex=0;
///////////index that is gonig to be used in loop
    for( auto surface_loop: surface_formats)
    {
        if((surface_loop.format==desired_surface_format.format)&&(surface_loop.colorSpace==desired_surface_format.colorSpace)){
        image_format=surface_loop.format;
        image_color_space=surface_loop.colorSpace;
        std::cout << "Desired Surface matched with Number "<<surfaceformatindex << std::endl;
        match=true;
        surface_format.format=image_format;
        surface_format.colorSpace=surface_loop.colorSpace;
        }
        else
            std::cout << "Desired Surface wasnt a match with "<<surfaceformatindex << std::endl;

        surfaceformatindex++;

    }


    if(match==false){
        std::cout << "There wasnt a match in Surface format and color defaulted to Surface format[0] "<< std::endl;
    image_format=surface_formats[0].format;
    image_color_space=surface_formats[0].colorSpace;
    }
    ///if match wanst found it will direct system to default format and color space
    std::cout<<"-------------------------------------- " <<std::endl;



/////After here swapchain creation
    VkSwapchainKHR swapchain;
    VkSwapchainKHR old_swapchain=VK_NULL_HANDLE;

    uint32_t imagecount=4;
    uint32_t swapchain_images_count=0;
    VkExtent2D image_size;


    std::vector<VkImage> swapchain_images;
    std::cout << "Swapchain create info"<<
        "\nflags "<<0<<
        "\nsurface "<<presentation_surface<<
        "\nminimum image count "<<imagecount<<
        "\nimageformat "<<surface_format.format<<
        "\nimageColorSpace: "<<surface_format.colorSpace<<
        "\nimage extent "<<",image extent "<<
        "\nsurface transform "<<surface_transform << std::endl;


    VkSwapchainCreateInfoKHR swapchain_create_info{
        .sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext=nullptr,
        .flags=0,
        .surface=presentation_surface,
        .minImageCount=imagecount,
        .imageFormat=surface_format.format,
        .imageColorSpace=surface_format.colorSpace,
        .imageExtent=surface_capabilities.currentExtent,
        .imageArrayLayers=1,
        .imageUsage=desired_usages,
        .imageSharingMode=VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount=0,
        .pQueueFamilyIndices=nullptr,
        .preTransform=surface_transform,
        .compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode=present_mode,
        .clipped=VK_TRUE,
        .oldSwapchain=old_swapchain
    };
    result={};

    result=vkCreateSwapchainKHR(logical_device,&swapchain_create_info, nullptr,&swapchain);
    if(result!=VK_SUCCESS){
        std::cout<<"Swapchain Creation error"<<std::endl;
        return false;
    }

    std::cout<<"-------------------------------------- " <<std::endl;
/////Swapchain creation succcccesss

    VkResult rvkGetSwapchainImagesKHR=vkGetSwapchainImagesKHR(logical_device,swapchain,&swapchain_images_count,nullptr);
    if (rvkGetSwapchainImagesKHR != VK_SUCCESS && rvkGetSwapchainImagesKHR != VK_INCOMPLETE) {
        std::cout<< "Error getting swapchain image count\n";
        return false;
    }

    swapchain_images.resize(swapchain_images_count);



    if( vkGetSwapchainImagesKHR(logical_device, swapchain, &swapchain_images_count, swapchain_images.data())!=VK_SUCCESS){
        std::cout<<"Error when getting Images count\n";
        return false;

    }


    std::cout<<"Swapchain images count: "<<swapchain_images_count<<", swapchain_images size: "<<swapchain_images.size()<<std::endl;

    std::vector<VkImageView> swapChainImageViews(swapchain_images_count);
    for(size_t i=0;i<swapchain_images_count;i++){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchain_images[i]; // The swapchain image
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // 2D texture
        createInfo.format = surface_format.format; // Format of the swapchain images
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // No swizzling
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0; // No mipmapping
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0; // No array layers
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(logical_device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            std::cout<<"vkCreateImageView error"<<std::endl;
        }

        }



        VkAttachmentDescription colorAttachment{};
        colorAttachment.format=surface_format.format;
        colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkAttachmentReference                 colorAttachmentRef{};
    colorAttachmentRef.attachment       = 0;
    colorAttachmentRef.layout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription                  subpass{};
    subpass.pipelineBindPoint           = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics pipeline
    subpass.colorAttachmentCount        = 1; // One color attachment
    subpass.pColorAttachments           = &colorAttachmentRef; // Reference to the color attachment

    VkSubpassDependency                   dependency{};
    dependency.srcSubpass               = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass               = 0;
    dependency.srcStageMask             = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask            = 0;
    dependency.dstStageMask             = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask            = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkRenderPassCreateInfo              renderPassInfo{};
    renderPassInfo.sType                = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount      = 1;
    renderPassInfo.pAttachments         = &colorAttachment;
    renderPassInfo.subpassCount         = 1;
    renderPassInfo.pSubpasses           = &subpass;
    renderPassInfo.dependencyCount      = 1;
    renderPassInfo.pDependencies        = &dependency;
    VkRenderPass renderPass;

    if(vkCreateRenderPass(logical_device,&renderPassInfo,nullptr, &renderPass)!=VK_SUCCESS)
    {
        std::cout<<"Render pass creation error "<<std::endl;
        return 0;
    }

    std::vector<VkFramebuffer> frameBuffers(swapChainImageViews.size());


        for(size_t i=0; i<swapChainImageViews.size();i++){

            VkImageView attachment[]={swapChainImageViews[i]};
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachment;
            framebufferInfo.width = size_of_images.width;
            framebufferInfo.height = size_of_images.height;
            framebufferInfo.layers = 1;

            if(vkCreateFramebuffer(logical_device,&framebufferInfo,nullptr, &frameBuffers[i])!=VK_SUCCESS)
            {
                std::cout<<"VKFRAMBUFFER error"<<std::endl;
            }
        }


        /////////////////
        struct Vertex{
            glm::vec2 position;
            glm::vec3 color;

        };
        std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, -0.5f}, {0.0f, 1.00f, 0.0f}},
            {{ 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        VkBufferCreateInfo bufferinfo{};
        bufferinfo.sType            =VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferinfo.size             =sizeof( vertices[0] ) * vertices.size();
        bufferinfo.usage            =VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferinfo.sharingMode      =VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer vertexBuffer;

        if(vkCreateBuffer(logical_device,&bufferinfo,nullptr,&vertexBuffer)!=VK_SUCCESS){
            std::cout<<"There was anwith buffer creation";
        }




        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logical_device,vertexBuffer,&memRequirements);

        ///query get physicaldevicememoyproerties

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physical_device,&memProperties);
        uint32_t retrivedMemoryIndex;
        uint32_t typeFilter = 1;
        VkMemoryPropertyFlags requiredProperties= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT| VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        for(uint32_t i=0; i < memProperties.memoryTypeCount ; i++){
            if((memRequirements.memoryTypeBits &(1<<i))&&(memProperties.memoryTypes[i].propertyFlags & requiredProperties)==requiredProperties){
                std::cout<<"expected VK_MEMORY_PROPERTY_HOST_COHERENT_BIT| VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT mem type is matched and number is "<<i<<std::endl;
                retrivedMemoryIndex=i;
            }
            else{
                std::cout<<"no match in Current index of Memory Property questioning "<<i<<std::endl;
            }
        }
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize= memRequirements.size;
        allocInfo.memoryTypeIndex=retrivedMemoryIndex;


        VkDeviceMemory vertexBufferMemory;
        if(vkAllocateMemory(logical_device,&allocInfo,nullptr,&vertexBufferMemory)!=VK_SUCCESS)
            std::cout<<"vkallocatemomory error"<<std::endl;

        vkBindBufferMemory(logical_device,vertexBuffer,vertexBufferMemory,0);

        void* data;
        vkMapMemory(logical_device,vertexBufferMemory,0, sizeof(vertices[0])*vertices.size(),0,&data);
        std::memcpy(data, vertices.data(), sizeof(vertices[0])*vertices.size());
        vkUnmapMemory(logical_device,vertexBufferMemory);


        std::vector<VkVertexInputAttributeDescription> attributeDescription(2);
        attributeDescription[0].binding=0;
        attributeDescription[0].location=0;
        attributeDescription[0].format=VK_FORMAT_R32G32_SFLOAT;
        attributeDescription[0].offset=offsetof(Vertex,position);

        attributeDescription[1].binding=0;
        attributeDescription[1].location=1;
        attributeDescription[1].format=VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription[1].offset=offsetof(Vertex,color);

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding=0;
        bindingDescription.stride=sizeof(Vertex);
        bindingDescription.inputRate=VK_VERTEX_INPUT_RATE_VERTEX;

        std::cout<<"Attribute-------------------------------------- " << std::endl;



        std::string vertexShaderPath = "C:\\Users\\sefa_\\Documents\\Vulkan13\\RSCs\\vertex\\vertex.spv";
        std::string fragmentShaderPath = "C:\\Users\\sefa_\\Documents\\Vulkan13\\RSCs\\fragment\\fragment.spv";


        std::cout<<"File Paths-------------------------------------- " <<std::endl;

        //vertex
        std::ifstream vertexfile(vertexShaderPath, std::ios::ate | std::ios::binary);
        if(!vertexfile.is_open()){
            throw std::runtime_error("Failed to open file: " + vertexShaderPath);
        }
        std::cout<<"vertexFile-------------------------------------- " <<std::endl;

        size_t fileSize =(size_t)vertexfile.tellg();
        std::vector<char> vertexshaderfilebuffer(fileSize);
        vertexfile.seekg(0);
        vertexfile.read(vertexshaderfilebuffer.data(),fileSize);
        vertexfile.close();
        std::cout<<"vertexFile-------------------------------------- " <<std::endl;


        //fragment
        std::ifstream fragmentfile(fragmentShaderPath, std::ios::ate | std::ios::binary);
        if(!fragmentfile.is_open()){
            throw std::runtime_error("Failed to open file: " + fragmentShaderPath);
        }
        size_t frgfileSize =(size_t)fragmentfile.tellg();
        std::vector<char> fragmentshaderfilebuffer(frgfileSize);
        fragmentfile.seekg(0);
        fragmentfile.read(fragmentshaderfilebuffer.data(),frgfileSize);
        fragmentfile.close();
        /////
        std::cout<<"-------------------------------------- " <<std::endl;

        VkShaderModuleCreateInfo vertexcreateInfo{};
        vertexcreateInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertexcreateInfo.codeSize= vertexshaderfilebuffer.size();
        vertexcreateInfo.pCode= reinterpret_cast<const uint32_t *>(vertexshaderfilebuffer.data());

        VkShaderModule vertexModule;
        if(vkCreateShaderModule(logical_device,&vertexcreateInfo, nullptr, &vertexModule)!=VK_SUCCESS)
            throw std::runtime_error("Failed to create shader module!");

        std::cout<<"Create Vertex Shader Module-------------------------------------- " <<std::endl;


        VkShaderModuleCreateInfo fragcreateInfo{};
        fragcreateInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragcreateInfo.codeSize= fragmentshaderfilebuffer.size();
        fragcreateInfo.pCode= reinterpret_cast<const uint32_t *>(fragmentshaderfilebuffer.data());

        VkShaderModule fragmentModule;
        if(vkCreateShaderModule(logical_device,&fragcreateInfo, nullptr, &fragmentModule)!=VK_SUCCESS)
            throw std::runtime_error("Failed to create shader module!");

        std::cout<<"Create Fragment Shader Module-------------------------------------- " <<std::endl;


        VkPipelineShaderStageCreateInfo vertStageInfo{};
        vertStageInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage             = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module            = vertexModule;
        vertStageInfo.pName             = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType           =VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage           = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module          = fragmentModule;
        fragShaderStageInfo.pName           = "main";

        VkPipelineShaderStageCreateInfo shaderStages[]= { vertStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount= 1;
        vertexInputInfo.pVertexBindingDescriptions= &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount= static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.pVertexAttributeDescriptions=attributeDescription.data();

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;  // No blending, just overwrite framebuffer

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;  // No logical operations
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        /////
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Define how vertices are connected
        inputAssembly.primitiveRestartEnable = VK_FALSE; // Restarting primitives is disabled
        //multisamplestate
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable=VK_FALSE;
        multisampling.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading=1.0f;
        multisampling.pSampleMask=nullptr;
        multisampling.alphaToCoverageEnable=VK_FALSE;
        multisampling.alphaToOneEnable=VK_FALSE;

        /////Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; // Ignore depth beyond near/far planes
        rasterizer.rasterizerDiscardEnable = VK_FALSE; // Ensure fragments are generated
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // Fill the triangle (not wireframe)
        rasterizer.lineWidth = 1.0f; // Default line thickness
        rasterizer.cullMode = VK_CULL_MODE_NONE; // Cull back faces (disable for two-sided rendering)
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Define vertex order
        rasterizer.depthBiasEnable = VK_FALSE; // No depth bias
        rasterizer.rasterizerDiscardEnable=VK_FALSE;
        //// viewport and scissor
        VkViewport viewport{};
        viewport.x=0.0f;
        viewport.y=0.0f;
        viewport.width=static_cast<float>(size_of_images.width);
        viewport.height=static_cast<float>(size_of_images.height);
        viewport.minDepth=0.0f;
        viewport.maxDepth=1.0f;

    //scisssor
        VkRect2D scissor{};
        scissor.offset={0,0};
        scissor.extent=size_of_images;


        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_FALSE; // Disable depth testing
        depthStencil.depthWriteEnable = VK_FALSE; // Disable depth writing
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; // Default depth comparison
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE; // Disable stencil testing

        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount=0;
        pipelineLayoutInfo.pSetLayouts=nullptr;
        pipelineLayoutInfo.pushConstantRangeCount=0;
        pipelineLayoutInfo.pPushConstantRanges=nullptr;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType=VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount=static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates= dynamicStates.data();

        if(vkCreatePipelineLayout(logical_device,&pipelineLayoutInfo, nullptr, &pipelineLayout)!=VK_SUCCESS)
            throw std::runtime_error("Failed to create pipeline layout!");

        std::cout<<"vkCreatePipelineLayout-------------------------------------- " <<std::endl;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType =VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.renderPass=renderPass;
        pipelineInfo.layout=pipelineLayout;
        pipelineInfo.subpass=0;
        pipelineInfo.pStages=shaderStages;
        pipelineInfo.stageCount=2;
        pipelineInfo.pDepthStencilState=&depthStencil;
        pipelineInfo.pDynamicState=&dynamicState;
        pipelineInfo.pColorBlendState=&colorBlending;
        pipelineInfo.pVertexInputState=&vertexInputInfo;
        pipelineInfo.pInputAssemblyState=&inputAssembly;
        pipelineInfo.pMultisampleState=&multisampling;
        pipelineInfo.pRasterizationState=&rasterizer;
        pipelineInfo.pViewportState=&viewportState;
        pipelineInfo.pTessellationState=nullptr;
        pipelineInfo.basePipelineHandle=VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex=-1;

        if(vkCreateGraphicsPipelines(logical_device,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&graphicsPipeline)!=VK_SUCCESS)
            throw std::runtime_error("Failed to create graphics pipeline!");

        std::cout<<"vkCreateGraphicsPipelines-------------------------------------- " <<std::endl;


        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        VkSemaphoreCreateInfo smphrCreateInfo{};
        smphrCreateInfo.sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags=VK_FENCE_CREATE_SIGNALED_BIT;
        VkFence   inFlightFence;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;

        if(vkCreateFence(logical_device,&fenceCreateInfo, nullptr, &inFlightFence)||vkCreateSemaphore(logical_device,&smphrCreateInfo,nullptr,&imageAvailableSemaphore)||vkCreateSemaphore(logical_device,&smphrCreateInfo,nullptr,&renderFinishedSemaphore)!=VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");

        }




        VkCommandPoolCreateInfo cmdPoolCreateInfo{};
        cmdPoolCreateInfo.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolCreateInfo.flags= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolCreateInfo.queueFamilyIndex=chosen_index;
        VkCommandPool commandPool;
        if(vkCreateCommandPool(logical_device,&cmdPoolCreateInfo,nullptr, &commandPool)!=VK_SUCCESS){
            throw std::runtime_error("Failed to create command pool!");

        }

        VkCommandBufferAllocateInfo cmdBufferallocInfo{};
        cmdBufferallocInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufferallocInfo.commandPool= commandPool;
        cmdBufferallocInfo.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufferallocInfo.commandBufferCount=1;

        VkCommandBuffer commandBuffer;
        if (vkAllocateCommandBuffers(logical_device, &cmdBufferallocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }
        std::cout<<"vkAllocateCommandBuffers-------------------------------------- " <<std::endl;
        vkWaitForFences(logical_device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logical_device, 1, &inFlightFence);

        uint32_t image_index;
        if(vkAcquireNextImageKHR(logical_device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &image_index)!=VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer!");


        std::cout<<"Availabel Swapchain index taken from vkAcquireNextImageKHR "<<image_index<<std::endl;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        std::cout<<"renderBeginPassInfo-------------------------------------- " <<std::endl;

        VkRenderPassBeginInfo renderBeginPassInfo{};
        renderBeginPassInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderBeginPassInfo.renderPass=renderPass;
        renderBeginPassInfo.framebuffer=frameBuffers[image_index];
        renderBeginPassInfo.renderArea.offset={0,0};
        renderBeginPassInfo.renderArea.extent=surface_capabilities.currentExtent;

        VkClearValue clearColor={{{0.0f, 0.0f, 0.0f, 1.0f}}};
        vkCmdSetViewport(commandBuffer,0,1,&viewport);
        vkCmdSetScissor(commandBuffer,0,1,&scissor);

        renderBeginPassInfo.clearValueCount=1;
        renderBeginPassInfo.pClearValues=&clearColor;

        vkCmdBeginRenderPass(commandBuffer,&renderBeginPassInfo,VK_SUBPASS_CONTENTS_INLINE);
        std::cout<<"vkCmdBeginRenderPass-------------------------------------- " <<std::endl;


        VkBuffer vertexBuffers[]={vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer,0,1,vertexBuffers,offsets);
        vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline);

        vkCmdDraw(commandBuffer,3,1,0,0);

        vkCmdEndRenderPass(commandBuffer);
        std::cout<<"vkCmdEndRenderPass-------------------------------------- " <<std::endl;

        if(vkEndCommandBuffer(commandBuffer)!=VK_SUCCESS){
            throw std::runtime_error("Failed to record command buffer!");
        }


        VkSemaphore waitSemaphores[]={imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[]={VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{};
        submitInfo.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount=1;
        submitInfo.pCommandBuffers=&commandBuffer;
        submitInfo.waitSemaphoreCount=1;
        submitInfo.pWaitSemaphores=waitSemaphores;
        submitInfo.pWaitDstStageMask=waitStages;
        submitInfo.commandBufferCount=1;


        VkSemaphore signalSemaphores[]={renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount=1;
        submitInfo.pSignalSemaphores=signalSemaphores;

        if(vkQueueSubmit(graphics_queue,1,&submitInfo,inFlightFence)!=VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType=VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount=1;
        presentInfo.pWaitSemaphores=signalSemaphores;
        if(swapchain==VK_NULL_HANDLE){
            throw std::runtime_error("swapchain==VK_NULL_HANDLE");

        }

        presentInfo.swapchainCount=1;
        presentInfo.pImageIndices=&image_index;
        presentInfo.pImageIndices=&image_index;
        VkSemaphore waitPresentSemaphore []={renderFinishedSemaphore};
        presentInfo.waitSemaphoreCount=1;
        presentInfo.pWaitSemaphores=waitPresentSemaphore;
        presentInfo.pSwapchains=&swapchain;
        presentInfo.pImageIndices=&image_index;
        presentInfo.pResults=nullptr;
        ShowWindow(window_parameters.HWnd,SW_SHOW);

        if(vkQueuePresentKHR(graphics_queue,&presentInfo)!=VK_SUCCESS)
            throw std::runtime_error("failed to vkQueuePresentKHR!");

        vkQueueWaitIdle(graphics_queue);

        UpdateWindow(window_parameters.HWnd);


        std::cout<<"-------------------------------------- " <<std::endl;




        return true;
}




int main(int argc, char *argv[])
{

    Vulkanloading();

    QApplication a(argc, argv);
    Start w;


    return a.exec();
}


