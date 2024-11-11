# set minimum version of CMake
cmake_minimum_required(VERSION 3.14)

# set variable PROJECT_NAME as cmake_project_example and C++ version
set(PROJECT_NAME shaderPixel)
set(CMAKE_CXX_STANDARD 17)

# glfw로 띄울 윈도우의 정보 세팅
set(WINDOW_NAME "openGL Example")
set(WINDOW_WIDTH 1280)
set(WINDOW_HEIGHT 960)

# project define. project() can set the project name, language version.
project(${PROJECT_NAME})

# add execuatble file by ${PROJECT_NAME} using src/main.cpp
add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/common.cpp src/common.h 
	src/shader.cpp src/shader.h
	src/program.cpp src/program.h
	src/context.cpp src/context.h
	src/buffer.cpp src/buffer.h
	src/vertexLayout.cpp src/vertexLayout.h
	src/image.cpp src/image.h
	src/texture.cpp src/texture.h
	)


# 외부 라이브러리를 cmake한 것을 include 하기.
include(Dependency.cmake)


# 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# C에서 define에 #define WINDOW_NAME "HELLO OPENGL" 해버리는 것과 동일한 내용.
# 바깥에서 macro를 정의해준다고 보면 됨.
target_compile_definitions(${PROJECT_NAME} PUBLIC
	WINDOW_NAME="${WINDOW_NAME}"
	WINDOW_WIDTH=${WINDOW_WIDTH}
	WINDOW_HEIGHT=${WINDOW_HEIGHT}
	)

# Dependency 들(${DEP_LST})이 먼저 빌드될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})