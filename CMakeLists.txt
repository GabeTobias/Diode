cmake_minimum_required (VERSION 3.1)

project (Diode)

add_executable(Diode ${PROJECT_SOURCE_DIR}/main.cpp)

set_property(TARGET Diode PROPERTY CXX_STANDARD 11)
target_compile_options(Diode PRIVATE -Wall)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(Diode PRIVATE glfw)

add_definitions(-DGLEW_STATIC)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(Diode PRIVATE libglew_static)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(Diode PRIVATE glm)

IF(WIN32)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Diode)
ENDIF()

