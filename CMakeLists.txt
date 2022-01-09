cmake_minimum_required(VERSION 3.14)
project(test_task_samsung_source_code)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")

add_executable(test_task_samsung_source_code src/main.cpp src/Shader.h src/Object.cpp src/Object.h src/DirLighting.h src/ShadowMap.cpp src/ShadowMap.h lib/glad/src/glad.c lib/stb_image.h)

# # add GLFW
# add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(test_task_samsung_source_code PRIVATE glfw3)
# target_include_directories(test_task_samsung_source_code PRIVATE lib/glfw/include)

# # add GLAD
target_include_directories(test_task_samsung_source_code PRIVATE lib/glad/include)

# # add GLM
# add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
# target_link_libraries(test_task_samsung_source_code PRIVATE glm)
# target_include_directories(test_task_samsung_source_code PRIVATE lib/glm)

# add bullet3
add_subdirectory(lib/bullet3)
target_link_libraries(test_task_samsung_source_code PRIVATE BulletDynamics BulletCollision LinearMath)
target_include_directories(test_task_samsung_source_code PRIVATE lib/bullet3/src)
