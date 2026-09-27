cmake_minimum_required(VERSION 3.14)
project(OpenRGBMchoseG75Plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

add_library(OpenRGBMchoseG75Plugin SHARED
    plugin/MchoseG75Plugin.h
    plugin/MchoseG75Plugin.cpp
    plugin/LogManagerStub.cpp
    openrgb/MchoseG75Controller.h
    openrgb/MchoseG75Controller.cpp
    openrgb/RGBController_MchoseG75.h
    openrgb/RGBController_MchoseG75.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/RGBController/RGBController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/StringUtils.cpp
)

target_include_directories(OpenRGBMchoseG75Plugin PRIVATE
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

target_link_libraries(OpenRGBMchoseG75Plugin PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

if(WIN32)
    target_link_libraries(OpenRGBMchoseG75Plugin PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/openrgb_src/dependencies/hidapi-win/x64/hidapi.lib
    )
elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(HIDAPI hidapi-hidraw)
    endif()
    if(HIDAPI_FOUND)
        target_link_libraries(OpenRGBMchoseG75Plugin PRIVATE ${HIDAPI_LIBRARIES})
    else()
        target_link_libraries(OpenRGBMchoseG75Plugin PRIVATE hidapi-hidraw)
    endif()
endif()
