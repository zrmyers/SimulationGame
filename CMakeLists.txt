cmake_minimum_required(VERSION 3.28.1)

project(SimulationGame
    VERSION 0.1
    DESCRIPTION "SimulationGame"
    LANGUAGES CXX)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_EXPORT_COMPILE_COMMANDS On)

set(CMAKE_ALL_ERRORS_FATAL TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include (cmake/CPM.cmake)

set(CPM_SOURCE_CACHE "./cpm-cache")

CPMAddPackage(
    URI "gh:libsdl-org/SDL#release-3.2.26"
)
CPMAddPackage(
    URI "gh:libsdl-org/SDL_ttf#release-3.2.2"
)
set(SDLIMAGE_AVIF OFF) # requires ASM_NASM compiler, which I don't want to install.
CPMAddPackage(
    URI "gh:libsdl-org/SDL_image#release-3.2.4"
)
CPMAddPackage(
    URI "gh:KhronosGroup/SPIRV-Headers#vulkan-sdk-1.4.328.1"
)
set(SPIRV_WERROR OFF)
CPMAddPackage(
    URI "gh:KhronosGroup/SPIRV-Tools#vulkan-sdk-1.4.328.1"
)
CPMAddPackage(
    URI "gh:KhronosGroup/glslang#vulkan-sdk-1.4.328.1"
)
CPMAddPackage(
    URI "gh:KhronosGroup/SPIRV-Cross#vulkan-sdk-1.4.328.1"
)
set(GLM_BUILD_LIBRARY OFF)
CPMAddPackage(
    URI "gh:g-truc/glm#1.0.2"
)
set(JSON_Diagnostics ON)
CPMAddPackage(
    URI "gh:nlohmann/json#v3.12.0"
)
CPMAddPackage(
    URI "gh:spnda/fastgltf#d3d6ee651f878347e29352000a1f0fa1324236b4"
)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/SimulationGame.cpp
    ./src/characters/PlayerCharacter.cpp
    ./src/creature/Compendium.cpp
    ./src/core/AssetLoader.cpp
    ./src/core/Engine.cpp
    ./src/core/Environment.cpp
    ./src/core/Filesystem.cpp
    ./src/core/Logger.cpp
    ./src/core/Settings.cpp
    ./src/core/String.cpp
    ./src/gltf/GLTF.cpp
    ./src/graphics/Font.cpp
    ./src/graphics/Mesh.cpp
    ./src/graphics/ShaderCross.cpp
    ./src/graphics/Texture2D.cpp
    ./src/items/ItemCatalog.cpp
    ./src/json/Json.cpp
    ./src/menu/ChooseCharacterMenu.cpp
    ./src/menu/CreateCharacterMenu.cpp
    ./src/menu/MainMenu.cpp
    ./src/menu/MenuManager.cpp
    ./src/menu/MenuUtilities.cpp
    ./src/menu/SettingsMenu.cpp
    ./src/sdl/SDL.cpp
    ./src/sdl/TTF.cpp
    ./src/systems/CreatureSystem.cpp
    ./src/systems/GuiSystem.cpp
    ./src/systems/InventorySystem.cpp
    ./src/systems/RenderSystem.cpp
    ./src/systems/SpriteSystem.cpp
    ./src/systems/TextSystem.cpp
    ./src/ui/Button.cpp
    ./src/ui/ButtonStyle.cpp
    ./src/ui/CheckBox.cpp
    ./src/ui/CheckBoxStyle.cpp
    ./src/ui/DropDown.cpp
    ./src/ui/DropDownStyle.cpp
    ./src/ui/Element.cpp
    ./src/ui/HorizontalLayout.cpp
    ./src/ui/ImageElement.cpp
    ./src/ui/NineSlice.cpp
    ./src/ui/NineSliceStyle.cpp
    ./src/ui/Radio.cpp
    ./src/ui/RadioStyle.cpp
    ./src/ui/Slider.cpp
    ./src/ui/SliderStyle.cpp
    ./src/ui/Spacer.cpp
    ./src/ui/Style.cpp
    ./src/ui/Switch.cpp
    ./src/ui/TextElement.cpp
    ./src/ui/TextInputBox.cpp
    ./src/ui/TextInputBoxStyle.cpp
    ./src/ui/VerticalLayout.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ./src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image
    SPIRV
    glslang
    glslang-default-resource-limits
    glm
    nlohmann_json
    fastgltf)
