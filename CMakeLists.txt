cmake_minimum_required(VERSION 4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)

project(HandNum VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(SVM_MODEL_PATH="${PROJECT_BINARY_DIR}/svmModel.yml")

# CHECK digits.png
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/data/raw/digits.png")
    message(SEND_ERROR "digits.png Missing...")
endif()

# DOWNLOAD MNIST DATASETS BEGIN
set(MNIST_BASE_URL "https://ossci-datasets.s3.amazonaws.com/mnist")
set(MNIST_TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/data/mnist")
set(MNIST_DEST_DIR "${PROJECT_SOURCE_DIR}/data/raw/mnist")

file(MAKE_DIRECTORY "${MNIST_TMP_DIR}")
file(MAKE_DIRECTORY "${MNIST_DEST_DIR}")

set(MNIST_FILES
    "train-images-idx3-ubyte.gz"
    "train-labels-idx1-ubyte.gz"
    "t10k-images-idx3-ubyte.gz"
    "t10k-labels-idx1-ubyte.gz"
)

foreach(filename IN LISTS MNIST_FILES)
    if(NOT EXISTS "${MNIST_TMP_DIR}/${filename}")
        message(STATUS "Downloading: ${filename}...")
        file(DOWNLOAD
            "${MNIST_BASE_URL}/${filename}"
            "${MNIST_TMP_DIR}/${filename}"
        )
    endif()

    string(REGEX REPLACE "\\.gz$" "" final_filename "${filename}")
    if(EXISTS "${MNIST_DEST_DIR}/${final_filename}")
        message(STATUS "mnist dataset ok: ${final_filename}...")
        continue()
    endif()

    message(STATUS "Deploying: ${filename}...")
    execute_process(
        COMMAND gzip -dc "${MNIST_TMP_DIR}/${filename}"
        OUTPUT_FILE "${MNIST_DEST_DIR}/${final_filename}"
        RESULT_VARIABLE result
        ERROR_VARIABLE error_msg
    )
    if(NOT result EQUAL 0)
        message(SEND_ERROR "FAILED TO DOWNLOAD MNIST ${filename}: ${error_msg}")
    endif()
endforeach()
# DOWNLOAD MNIST DATASETS END

find_package(OpenCV REQUIRED)
find_package(spdlog REQUIRED)

# add_subdirectory(src/preprocessor)
add_subdirectory(src/app)
add_subdirectory(src/core)

target_include_directories(HandNum PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(HandNum PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(HandNum
    core
)
target_link_libraries(HandNum
    ${OpenCV_LIBS}
    spdlog::spdlog
)


add_executable(HandNumTest
    src/main.cpp
)

target_include_directories(HandNumTest PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(HandNumTest PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(HandNumTest
    core
)
target_link_libraries(HandNumTest
    ${OpenCV_LIBS}
    spdlog::spdlog
)
