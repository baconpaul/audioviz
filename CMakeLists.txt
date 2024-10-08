cmake_minimum_required(VERSION 3.16)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")

project(audioviz LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

find_package(Git)

execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_CXX_STANDARD 17)
include(FetchContent)

set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


if (APPLE)
    set(RTAUDIO_API_JACK FALSE CACHE BOOL "Not on pauls mac at least")
endif()
FetchContent_Declare(RTAudio
        GIT_REPOSITORY https://github.com/thestk/rtaudio.git
        GIT_TAG 6.0.1)
FetchContent_MakeAvailable(RTAudio)

include("cmake/CmakeRC.cmake")

add_subdirectory(res)
add_subdirectory(src/shaders)

add_executable(${PROJECT_NAME}
        src/infra/main.cpp
        src/infra/resource_access.cpp
        src/audio/audio.cpp
        src/MainScreenProvider.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics ${PROJECT_NAME}-resources ${PROJECT_NAME}-shaders rtaudio)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE GIT_HASH="${GIT_COMMIT_HASH}")

if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/Info.plist.in
    )
    target_link_options(${PROJECT_NAME} PUBLIC  "-Wl,-rpath,@executable_path/../Frameworks")
    set(FWD "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND mkdir -p ${FWD}/${PROJECT_NAME}.app/Contents/Frameworks
            COMMAND  cp -a ${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/libs-osx/Frameworks/freetype.framework ${FWD}/${PROJECT_NAME}.app/Contents/Frameworks
            USES_TERMINAL
    )
endif()