add_executable(app "main.cpp")

target_link_directories(app PRIVATE "../../glcm/headers")
target_link_libraries(app PRIVATE glcm)
