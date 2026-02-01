require "premake-ecc/ecc"

VCPKG_ROOT = "vcpkg"
VCPKG_TRIPLET = "x64-windows"
VKPKG_INCLUDE = path.join(VCPKG_ROOT, "installed", VCPKG_TRIPLET, "include")
VCPKG_DEBUG_BIN   = path.join(VCPKG_ROOT, "installed", VCPKG_TRIPLET, "debug", "bin")
VCPKG_DEBUG_LIB = path.join(VCPKG_ROOT, "installed", VCPKG_TRIPLET, "debug", "lib")
VCPKG_RELEASE_BIN = path.join(VCPKG_ROOT, "installed", VCPKG_TRIPLET, "bin")
VCPKG_RELEASE_LIB = path.join(VCPKG_ROOT, "installed", VCPKG_TRIPLET, "lib")

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
        VKPKG_INCLUDE,
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
        "src/param",
        "src/plot",
        "src/plot/processors",
        "src/plot/views",
        "src/state",
        "src/utils",
        "src/ui/layouts",
        "src/ui/theme",
        "src/ui/widgets",
        "src/ui/windows",
        "src/ui/windows/experimental",
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

        libdirs {
            VCPKG_DEBUG_LIB
        }

        links {
            "CoolPropd",
            "fmtd",
            "glad",
            "glfw3dll",
            "hello_imgui",
            "imguid",
            "implotd",
            "libcurl-d",
            "spdlogd",
            "stb_hello_imgui",
            "zlibd"
        }

        postbuildcommands {
            '{COPY} "' .. VCPKG_DEBUG_BIN .. '/*.dll" "%{cfg.targetdir}"'
        }

    filter "configurations:Release"
        optimize "On"

        libdirs {
            VCPKG_RELEASE_LIB
        }

        links {
            "CoolProp",
            "fmt",
            "glad",
            "glfw3dll",
            "hello_imgui",
            "imgui",
            "implot",
            "libcurl",
            "spdlog",
            "stb_hello_imgui",
            "zlib"
        }

        postbuildcommands {
            '{COPY} "' .. VCPKG_RELEASE_BIN .. '/*.dll" "%{cfg.targetdir}"'
        }

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
