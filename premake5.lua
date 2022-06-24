workspace "erosion"
    architecture "x64"
    toolset "gcc"

    configurations {
        "debug",
        "release",
        "dist"
    }

outputdir = "%{cfg.buildcfg}/%{cfg.system}/%{cfg.architectrure}"

project "erosion"
    kind "ConsoleApp"
    language "C++"
    targetname "erosion"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files {
        "src/**.hpp",
        "src/**.cpp",
        "include/**.hpp",
        "include/**.cpp",
        --"deps/src/**.cpp",
        "deps/include/**.h",
        "deps/include/**.hpp"
    }

    includedirs {
        "include",
        "deps/include"
    }

    links {
        "SDL2",
        "SDL2_image"
    }

    libdirs {
        "deps/libs",
    }

    postbuildcommands {
        "cp -r res bin/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architectrure}"
    }

    filter "configurations:debug"
        symbols "On"
    filter "configurations:release"
        optimize "On"
    filter "configurations:dist"
        optimize "On"
