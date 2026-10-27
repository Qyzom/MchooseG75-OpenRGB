cmake_minimum_required(VERSION 3.14)
project(OpenRGBMchoseG75Plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

# Common sources and includes
set(COMMON_SOURCES
    plugin/LogManagerStub.cpp
    openrgb/MchoseG75Controller.h
    openrgb/MchoseG75Controller.cpp
    openrgb/RGBController_MchoseG75.h
    openrgb/RGBController_MchoseG75.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/RGBController/RGBController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/StringUtils.cpp
)

set(COMMON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/RGBController
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/Detectors
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/dependencies/hidapi/hidapi
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/dependencies/hidapi-win/include
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/dependencies/json
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/dependencies/hue-plusplus/include
    /usr/include/hidapi
)

# 1. Wired Plugin
add_library(OpenRGBMchoseG75WiredPlugin SHARED
    plugin/MchoseG75WiredPlugin.h
    plugin/MchoseG75WiredPlugin.cpp
    ${COMMON_SOURCES}
)
target_include_directories(OpenRGBMchoseG75WiredPlugin PRIVATE ${COMMON_INCLUDES})
target_link_libraries(OpenRGBMchoseG75WiredPlugin PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
set_target_properties(OpenRGBMchoseG75WiredPlugin PROPERTIES PREFIX "")

# 2. Wireless Plugin (2.4G)
add_library(OpenRGBMchoseG75WirelessPlugin SHARED
    plugin/MchoseG75WirelessPlugin.h
    plugin/MchoseG75WirelessPlugin.cpp
    ${COMMON_SOURCES}
)
target_include_directories(OpenRGBMchoseG75WirelessPlugin PRIVATE ${COMMON_INCLUDES})
target_link_libraries(OpenRGBMchoseG75WirelessPlugin PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
set_target_properties(OpenRGBMchoseG75WirelessPlugin PROPERTIES PREFIX "")

# 3. Combined Plugin (Wired + Wireless)
add_library(OpenRGBMchoseG75Plugin SHARED
    plugin/MchoseG75Plugin.h
    plugin/MchoseG75Plugin.cpp
    ${COMMON_SOURCES}
)
target_include_directories(OpenRGBMchoseG75Plugin PRIVATE ${COMMON_INCLUDES})
target_link_libraries(OpenRGBMchoseG75Plugin PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
set_target_properties(OpenRGBMchoseG75Plugin PROPERTIES PREFIX "")

# Link HIDAPI
set(ALL_TARGETS OpenRGBMchoseG75WiredPlugin OpenRGBMchoseG75WirelessPlugin OpenRGBMchoseG75Plugin)

foreach(tgt ${ALL_TARGETS})
    if(WIN32)
        target_link_libraries(${tgt} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/dependencies/hidapi-win/x64/hidapi.lib
        )
    elseif(UNIX AND NOT APPLE)
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(HIDAPI hidapi-hidraw)
        endif()
        if(HIDAPI_FOUND)
            target_link_libraries(${tgt} PRIVATE ${HIDAPI_LIBRARIES})
        else()
            target_link_libraries(${tgt} PRIVATE hidapi-hidraw)
        endif()
    endif()
endforeach()
