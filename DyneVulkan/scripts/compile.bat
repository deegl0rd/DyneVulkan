@echo off

C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\shader.vert -o ..\..\x64\MTDebug\shaders\shader.vert.spv
C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\shader.frag -o ..\..\x64\MTDebug\shaders\shader.frag.spv

C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\shader.vert -o ..\shaders\shader.vert.spv
C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\shader.frag -o ..\shaders\shader.frag.spv

C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\pointlight.vert -o ..\..\x64\MTDebug\shaders\pointlight.vert.spv
C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\pointlight.frag -o ..\..\x64\MTDebug\shaders\pointlight.frag.spv

C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\pointlight.vert -o ..\shaders\pointlight.vert.spv
C:\VulkanSDK\1.2.198.1\Bin\glslc.exe ..\shaders\pointlight.frag -o ..\shaders\pointlight.frag.spv
