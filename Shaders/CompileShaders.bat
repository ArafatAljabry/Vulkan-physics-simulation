
@echo off
%VULKAN_SDK%\Bin\glslangValidator.exe -V Shader.vert -o Shader.vert.spv
%VULKAN_SDK%\Bin\glslangValidator.exe -V Shader.frag -o Shader.frag.spv
%VULKAN_SDK%\Bin\glslangValidator.exe -V PhongShader.vert -o PhongShader.vert.spv
%VULKAN_SDK%\Bin\glslangValidator.exe -V PhongShader.frag -o PhongShader.frag.spv

pause
