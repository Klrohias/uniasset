
# C++ Options
set(CMAKE_CXX_STANDARD 17)

# Config

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
        audio/AudioPlayer.cpp
        audio/AudioPlayer.hpp
        audio/IAudioDecoder.cpp
        audio/IAudioDecoder.hpp
        thirdparty/stb_image.c
        thirdparty/miniaudio.c
        audio/SampleFormat.hpp
        thirdparty/drlibs.c
        audio/Mp3Decoder.cpp
        audio/Mp3Decoder.hpp
        audio/FlacDecoder.cpp
        audio/FlacDecoder.hpp
        audio/WavDecoder.cpp
        audio/WavDecoder.hpp
        audio/OggDecoder.cpp
        audio/OggDecoder.hpp
        thirdparty/miniaudio.m
)

target_compile_features(uniasset PRIVATE cxx_std_17)
target_compile_definitions(uniasset PRIVATE UNIASSET_BUILD)

if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    # enable bitcode
    target_compile_options(uniasset PRIVATE -fembed-bitcode)
endif()

# Link
target_include_directories(uniasset PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(uniasset PRIVATE ${MINIAUDIO_INCLUDE_DIRS})
target_include_directories(uniasset PRIVATE ${DRLIBS_INCLUDE_DIRS})
target_link_libraries(uniasset PRIVATE WebP::webpdecoder)
target_link_libraries(uniasset PRIVATE ${LINK_LIBJPEG_NAME})

# Post build
if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    add_custom_command(
            TARGET uniasset
            POST_BUILD
            COMMAND sh ${CMAKE_SOURCE_DIR}/scripts/ios-link-library.sh ${CMAKE_BINARY_DIR}
    )
endif()
