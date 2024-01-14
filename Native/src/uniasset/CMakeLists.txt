
# C++ Options
set(CMAKE_CXX_STANDARD 14)

# Config
if (BUILD_SHARED_LIBS AND WIN32)
    add_compile_definitions(UNIASSET_BUILD)
endif()

set(LINK_LIBJPEG_NAME libjpeg-turbo::turbojpeg-static)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(LINK_LIBJPEG_NAME libjpeg-turbo::turbojpeg)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(BUILD_SHARED_LIBS no)
endif()

# Packages
find_package(Stb REQUIRED)
find_package(libjpeg-turbo CONFIG REQUIRED)
find_package(WebP CONFIG REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")
find_path(DRLIBS_INCLUDE_DIRS "dr_flac.h")

# Targets
add_library(uniasset
        Foundation.hpp
        common/IAudioDecoder.cpp
        common/IAudioDecoder.hpp
        common/IAudioPlayer.cpp
        common/IAudioPlayer.hpp
        CBinding.h
        CBinding.cpp
        image/ImageAsset.cpp
        image/ImageAsset.hpp
        common/Utils.cpp
        common/Utils.hpp
        common/ErrorHandler.cpp
        common/ErrorHandler.hpp
        audio/AudioAsset.cpp
        audio/AudioAsset.hpp
        audio/MiniAudioPlayer.cpp
        audio/MiniAudioPlayer.hpp
        audio/OboeAudioPlayer.cpp
        audio/OboeAudioPlayer.hpp)

# Link
target_include_directories(uniasset PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(uniasset PRIVATE ${MINIAUDIO_INCLUDE_DIRS})
target_include_directories(uniasset PRIVATE ${DRLIBS_INCLUDE_DIRS})
target_link_libraries(uniasset PRIVATE WebP::webp WebP::webpdecoder WebP::webpdemux)
target_link_libraries(uniasset PRIVATE ${LINK_LIBJPEG_NAME})

# Post build
if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    add_custom_command(
            TARGET uniasset
            POST_BUILD
            COMMAND sh ${CMAKE_SOURCE_DIR}/scripts/ios-link-library.sh ${VCPKG_INSTALLED_DIR}/../
    )
endif()
