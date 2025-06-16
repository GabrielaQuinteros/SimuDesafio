cmake_minimum_required(VERSION 3.27.8)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(main 
    src/main.cpp

    # Utils
    src/utils/MapLoader.cpp
    src/utils/Utils.cpp

    # Model
    src/model/HexGrid.cpp
    src/model/Player.cpp

    # Core (lógica del juego)
    src/core/GameLogic.cpp

    # Render
    src/render/Renderer.cpp
)

target_link_libraries(main PRIVATE sfml-graphics)
target_compile_features(main PRIVATE cxx_std_17)
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/src")

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "${CMAKE_BINARY_DIR}/resources"
)

set_target_properties(main PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
