# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Vulkan13_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Vulkan13_autogen.dir\\ParseCache.txt"
  "Vulkan13_autogen"
  )
endif()
