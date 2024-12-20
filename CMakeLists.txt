cmake_minimum_required(VERSION 3.13)
if (MSVC)
	add_compile_options(/utf-8)
endif()
set(PROJECT_NAME ShaderPixel)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "ShaderPixel")
set(WINDOW_WIDTH 320)
set(WINDOW_HEIGHT 240)

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

include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

# Dependency들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})

# if (MSVC)
#     target_compile_options(${PROJECT_NAME} PUBLIC /wd4819)
# endif()
