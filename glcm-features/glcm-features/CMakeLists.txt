add_executable (glcm-features "glcm-features.cpp" "glcm-features.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET glcm-features PROPERTY CXX_STANDARD 20)
endif()
