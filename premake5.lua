workspace "RandomNumberGenerators" 
    configurations { "Debug", "Release" }
    platforms "Win64"



project "RandomNumberGenerators"
    location "RandomNumberGenerators"
    kind "StaticLib"
    language "C++"

    targetdir "bin/output/%{prj.name}/%{cfg.buildcfg}"
    objdir "bin/int/%{prj.name}/%{cfg.buildcfg}"

    includedirs {
        "Dependencies/spdlog/include"
    }

    files { "%{prj.name}/**.h", "%{prj.name}/**.c", "%{prj.name}/**.hpp", "%{prj.name}/**.cpp" }

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"RELEASE"}
        optimize "On"


    filter { "platforms:Win64" }
        system "Windows"
        cppdialect "C++17"
        systemversion "latest"
        architecture "x86_64"    


project "Application"
    location "Application"
    kind "ConsoleApp"
    language "C++"

    targetdir "bin/output/%{prj.name}/%{cfg.buildcfg}"
    objdir "bin/int/%{prj.name}/%{cfg.buildcfg}"

    includedirs {
        "Dependencies/GLFW/include",
        "Dependencies/GLAD/include",
        "Dependencies/spdlog/include",
        "Dependencies/stb",
        "RandomNumberGenerators",
        "RandomNumberGenerators/src",
        "Application/vendor/ImGui",
        "Application/src"
    }

    libdirs {
        "Dependencies/GLFW/lib-vc2019"
    }

    links {
        "RandomNumberGenerators",
        "glfw3.lib"
    }
    
    files { "%{prj.name}/**.h", "%{prj.name}/**.c", "%{prj.name}/**.hpp", "%{prj.name}/**.cpp"}

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"RELEASE"}
        optimize "On"

    filter { "platforms:Win64" }
        system "Windows"
        cppdialect "C++17"
        systemversion "latest"
        architecture "x86_64"
  