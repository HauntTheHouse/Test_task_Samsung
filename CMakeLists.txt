cmake_minimum_required(VERSION 3.14)
project(Test_task_Samsung)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")

file(GLOB SRC_LIST CONFIGURE_DEPENDS
    src/*.h
    src/*.cpp
)

#message("SRC_LIST" )
add_executable(${PROJECT_NAME}
	${SRC_LIST}
	lib/glad/src/glad.c)

# add GLFW
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE lib/glfw/include)

# # add GLAD
target_include_directories(${PROJECT_NAME} PRIVATE lib/glad/include)

# add GLM
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_include_directories(${PROJECT_NAME} PRIVATE lib/glm)

# add bullet3
add_subdirectory(lib/bullet3)
target_link_libraries(${PROJECT_NAME} PRIVATE BulletDynamics BulletCollision LinearMath)
target_include_directories(${PROJECT_NAME} PRIVATE lib/bullet3/src)

# add stb
target_include_directories(${PROJECT_NAME} PRIVATE lib/stb)

file(COPY shaders DESTINATION .)
file(COPY images DESTINATION .)
