# ExternalProject 관련 명령어 셋 추가 
# include -> cmake 에 새 기능을 추가하겠다는 의미. 여기서는 ExternalProject_Add 를 사용하기 위함.
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)


# if (APPLE)
# macOS 환경에서는 필요한 프레임워크를 명시적으로 링크합니다.
if (APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
		${OPENGL_FRAMEWORK}
    )
endif()


# spdlog: fast logger library
# 해당 함수는 깃 주소에 있는 레포를 cmake 해주겠다는 의미.
ExternalProject_Add(
	dep_spdlog # 해당 프로젝트의 대표명
	GIT_REPOSITORY "https://github.com/gabime/spdlog.git" 
	GIT_TAG "v1.x" # 버전 세팅. 깃 저장소에 브랜치/태그 정보로 적혀있다.
	GIT_SHALLOW 1 # on/off boolean 값. shallow 를 활성화한다. 이것은 커밋 정보를 가장 최신의 것만 다운받겠다는 뜻.
	UPDATE_COMMAND "" # 최신의 깃 버젼으로 업데이트해준다
	PATCH_COMMAND ""  # 코드를 고치고 싶다면 패치 커맨드
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR} # cmake configure. 인자의 세팅
	TEST_COMMAND ""   # 자동테스트 세팅 시 어떻게 테스트할지
)

# dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep_spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog)

# glad
ExternalProject_Add(
	dep_glad
	GIT_REPOSITORY "https://github.com/Dav1dde/glad"
	GIT_TAG "v0.1.34"
	GIT_SHALLOW 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
		-DGLAD_INSTALL=ON
	TEST_COMMAND ""
	)
	
set(DEP_LIST ${DEP_LIST} dep_glad)
set(DEP_LIBS ${DEP_LIBS} glad)

# glfw include
ExternalProject_Add(
	dep_glfw
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.3"
	GIT_SHALLOW 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	TEST_COMMAND ""
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
		-DGLFW_BIULD_EXAMPLES=OFF 
		-DGLFW_BUILD_TESTS=OFF
		-DGLFW_BUILD_DOCS=OFF # 해당 옵션들은 glfw 깃에 들어가서 CMakeLists.txt에 적혀있다. -D 플래그를 통해 해당 옵션들을 수정해주는것.
	)
set(DEP_LIST ${DEP_LIST} dep_glfw)
set(DEP_LIBS ${DEP_LIBS} glfw3)


# stb
ExternalProject_Add(
	dep_stb # target name
	GIT_REPOSITORY "https://github.com/nothings/stb"
	GIT_TAG "master"
	GIT_SHALLOW 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	TEST_COMMAND ""
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
		${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
		${DEP_INSTALL_DIR}/include/stb/stb_image.h

	)
set(DEP_LIST ${DEP_LIST} dep_stb)

# glm
ExternalProject_Add(
	dep_glm
	GIT_REPOSITORY "https://github.com/g-truc/glm"
	GIT_TAG "0.9.9.8"
	GIT_SHALLOW 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	TEST_COMMAND ""
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
		${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
		${DEP_INSTALL_DIR}/include/glm
	)
	
set(DEP_LIST ${DEP_LIST} dep_glm)

add_library(imgui
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    )
target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
add_dependencies(imgui ${DEP_LIST})
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)