add_executable(CXCAM main.cpp)
target_link_libraries(CXCAM PRIVATE cxcam_gui)

# Windows GUI 程序（不弹控制台）
set_target_properties(CXCAM PROPERTIES WIN32_EXECUTABLE TRUE)
