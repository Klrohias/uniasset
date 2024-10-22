
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

set(OBJC_SUPPORTED 0)
if (APPLE)
    set(OBJC_SUPPORTED 1)
endif ()

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
        audio/AudioAsset.cpp
        audio/AudioAsset.hpp
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
        $<IF:${OBJC_SUPPORTED},thirdparty/miniaudio.m,>
        common/Buffer.hpp
        common/MagicNumbers.hpp
        thirdparty/unique.hpp
        thirdparty/c_file.hpp
        common/Templates.hpp
        thirdparty/turbojpeg.hpp
        thirdparty/miniaudio.hpp
        audio/ExternalAudioDecoder.cpp
        audio/ExternalAudioDecoder.hpp
        thirdparty/stb_vorbis.hpp
        thirdparty/stb_vorbis.cpp
        common/Errors.hpp
        common/Errors.cpp
        audio/BufferedAudioDecoder.cpp
        audio/BufferedAudioDecoder.hpp
        audio/AudioDevice.cpp
        audio/AudioDevice.hpp
        common/Result.hpp
        audio/player/AudioPlayer.cpp
        audio/player/AudioPlayer.hpp
        audio/player/IWaveProvider.cpp
        audio/player/IWaveProvider.hpp
        audio/player/AudioMixer.cpp
        audio/player/AudioMixer.hpp
        audio/player/PlayingHandler.cpp
        audio/player/PlayingHandler.hpp
        common/AudioUtils.hpp
        audio/player/waves/AdaptWaveProvider.hpp
        audio/player/waves/AdaptWaveProvider.cpp
        audio/AudioConfig.hpp
        audio/player/waves/BufferedWaveProvider.hpp
        audio/player/waves/BufferedWaveProvider.cpp
        common/RingQueueBuffer.hpp
        common/RingQueueBuffer.cpp
        common/IJob.hpp)

target_compile_features(uniasset PUBLIC cxx_std_20)
target_compile_definitions(uniasset PRIVATE UNIASSET_BUILD)

set(UNIASSET_COMPILE_FLAGS "")
set(UNIASSET_CXX_FLAGS "")
set(UNIASSET_C_FLAGS "")
set(UNIASSET_LINK_FLAGS "")

if (BUILD_SHARED_LIBS)
    target_compile_definitions(uniasset PRIVATE UNIASSET_SHARED)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    # enable bitcode
    list(APPEND UNIASSET_COMPILE_FLAGS "-fembed-bitcode")
endif()

# Options
target_include_directories(uniasset PUBLIC ${Stb_INCLUDE_DIR})
target_include_directories(uniasset PUBLIC ${MINIAUDIO_INCLUDE_DIRS})
target_include_directories(uniasset PUBLIC ${DRLIBS_INCLUDE_DIRS})
target_link_libraries(uniasset PRIVATE WebP::webpdecoder)
target_link_libraries(uniasset PRIVATE ${LINK_LIBJPEG_NAME})

if ((BUILD_SHARED_LIBS) AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    list(APPEND UNIASSET_CXX_FLAGS "-fvisibility=hidden" "-fvisibility-inlines-hidden" "-ffunction-sections" "-fdata-sections")
    list(APPEND UNIASSET_C_FLAGS "-fvisibility=hidden" "-ffunction-sections")
    list(APPEND UNIASSET_LINK_FLAGS  "-Wl,--version-script=${CMAKE_SOURCE_DIR}/scripts/ld-version.script" "-Wl,--gc-sections" "-s")

endif()

if ((BUILD_SHARED_LIBS) AND (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    list(APPEND UNIASSET_CXX_FLAGS "-fvisibility=hidden" "-fvisibility-inlines-hidden")
endif()

target_compile_options(uniasset PRIVATE
    ${UNIASSET_COMPILE_FLAGS}
    $<$<COMPILE_LANGUAGE:CXX>:${UNIASSET_CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:C>:${UNIASSET_C_FLAGS}>)

target_link_options(uniasset PRIVATE ${UNIASSET_LINK_FLAGS})

# Post build
if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    add_custom_command(
            TARGET uniasset
            POST_BUILD
            COMMAND sh ${CMAKE_SOURCE_DIR}/scripts/ios-link-library.sh ${CMAKE_BINARY_DIR}
    )
endif()
