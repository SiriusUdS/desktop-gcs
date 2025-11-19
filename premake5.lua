workspace "sirius-gcs"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "sirius-gcs"

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "out/bin/%{cfg.buildcfg}"
    objdir "out/obj"

    includedirs {
        "src",
        "src/com",
        "src/com/commands",
        "src/com/discovery",
        "src/com/monitor",
        "src/com/packet",
        "src/com/serial",
        "src/config",
        "src/data",
        "src/data/data_conversion",
        "src/logging",
        "src/map",
        "src/params",
        "src/plot",
        "src/utils",
        "src/ui/widgets",
        "src/ui/windows",
        "external/ceSerial",
        "external/mINI",
        "external/sirius-headers-common"
    }

    files {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "external/ceSerial/ceserial.h",
        "external/mINI/ini.h",
        "external/sirius-headers-common/**.h"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "system:windows"
        buildoptions {
            "/utf-8",
            "/wd4005",
            "/wd26498",
            "/wd26800"
        }

project "sirius-gcs"

project "sirius-gcs-tests"
    includedirs {
        "external/doctest"
    }

    files {
        "tests/**.cpp",
        "external/doctest/doctest.h"
    }

    removefiles {
        "src/EntryPoint.cpp"
    }
