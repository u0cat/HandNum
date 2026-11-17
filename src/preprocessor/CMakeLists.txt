add_executable(PreProcessor
    preprocessor.cpp
)

add_executable(deskewTest
    deskewTest.cpp
)

target_include_directories(PreProcessor
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIRS}
)

target_include_directories(deskewTest
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(PreProcessor ${OpenCV_LIBS})
target_link_libraries(deskewTest
    PRIVATE
    core
    ${OpenCV_LIBS}
)