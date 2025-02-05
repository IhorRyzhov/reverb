cmake_minimum_required(VERSION 3.15)

include(ExternalProject)

project(reverb)

add_executable(${CMAKE_PROJECT_NAME}
	main.c
	io-audio/io-audio.c
)

add_dependencies(${CMAKE_PROJECT_NAME}
	project-liquid-dsp
	project-libwave
)
	
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
	${CMAKE_BINARY_DIR}/../liquid-dsp
	${CMAKE_BINARY_DIR}/../libwave
)

target_link_libraries(${CMAKE_PROJECT_NAME}
	m
	dl
	pthread
	libliquid.a
	libwave.a
)

ExternalProject_Add(project-libwave
    PREFIX              ${CMAKE_SOURCE_DIR}/libwave
	SOURCE_DIR          ${CMAKE_SOURCE_DIR}/libwave
	BINARY_DIR          ${CMAKE_BINARY_DIR}/../libwave
	TMP_DIR             ${CMAKE_BINARY_DIR}/../libwave
	STAMP_DIR           ${CMAKE_BINARY_DIR}/../libwave
	CMAKE_ARGS			-DBUILD_SHARED_LIBS=OFF	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/toolchains/arm-toolchain.cmake
	PATCH_COMMAND       patch -t -p1 --forward < ${CMAKE_SOURCE_DIR}/libwave.patch || true
    BUILD_IN_SOURCE     0
    INSTALL_COMMAND     ""
    UPDATE_COMMAND      ""
)

ExternalProject_Add(project-liquid-dsp
	PREFIX              ${CMAKE_SOURCE_DIR}/liquid-dsp
	SOURCE_DIR          ${CMAKE_SOURCE_DIR}/liquid-dsp
	BINARY_DIR          ${CMAKE_BINARY_DIR}/../liquid-dsp
	TMP_DIR             ${CMAKE_BINARY_DIR}/../liquid-dsp
	STAMP_DIR           ${CMAKE_BINARY_DIR}/../liquid-dsp
	CMAKE_ARGS          -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/toolchains/arm-toolchain.cmake
	                    -DCOVERAGE=OFF -DBUILD_EXAMPLES=OFF -DBUILD_AUTOTESTS=OFF -DBUILD_BENCHMARKS=OFF 
	PATCH_COMMAND       patch -t -p1 --forward < ${CMAKE_SOURCE_DIR}/libliquid.patch || true
    BUILD_IN_SOURCE     0
    INSTALL_COMMAND     ""
    UPDATE_COMMAND      ""
)
