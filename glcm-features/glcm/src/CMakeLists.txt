include_directories(${PROJECT_SOURCE_DIR}/MainProject/headers)

add_library(glcm "glcm_features.cpp" "glcm.cpp" "image.cpp")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(glcm ${OpenCV_LIBS})
