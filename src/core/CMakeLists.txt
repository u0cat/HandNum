set(CoreUtils
    utils/DatasetLoader.cpp
    utils/DigitNormalizer.cpp
)

add_library(core
    KNNCore.cpp
    SVMCore.cpp
    ${CoreUtils}
)

target_include_directories(core
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(core ${OpenCV_LIBS})
