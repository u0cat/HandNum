include(FetchContent)

find_package(SDL3 REQUIRED)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG "v1.93.0"
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

set(appScreens
    screen/ScreenManager.cpp
    
    screen/PredictScreen/PredictScreen.cpp
)

add_executable(HandNum
    app.cpp
    components/canvas/canvas.cpp
    ${appScreens}
)

target_include_directories(HandNum PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(HandNum imgui)
