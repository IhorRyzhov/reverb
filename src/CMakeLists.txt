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
	${CMAKE_SOURCE_DIR}/liquid-dsp
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
    PREFIX              ${CMAKE_CURRENT_SOURCE_DIR}/libwave
	SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/libwave
	BINARY_DIR          ${CMAKE_BINARY_DIR}/../libwave
	TMP_DIR             ${CMAKE_BINARY_DIR}/../libwave
	STAMP_DIR           ${CMAKE_BINARY_DIR}/../libwave
	CMAKE_ARGS			-DBUILD_SHARED_LIBS=OFF	
    BUILD_IN_SOURCE     0
    INSTALL_COMMAND     ""
    UPDATE_COMMAND      ""
)

ExternalProject_Add(project-liquid-dsp
	SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/liquid-dsp
	TMP_DIR             ${CMAKE_BINARY_DIR}/../liquid-dsp
	STAMP_DIR           ${CMAKE_BINARY_DIR}/../liquid-dsp
	CONFIGURE_COMMAND   ""
	BUILD_COMMAND       "make"
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     ""
    UPDATE_COMMAND      ""
)

ExternalProject_Add_Step(project-liquid-dsp bootstrap
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/liquid-dsp
	COMMAND ./bootstrap.sh
)

ExternalProject_Add_Step(project-liquid-dsp call-configure
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/liquid-dsp
	COMMAND ./configure --host=x86_64-linux-gnu --target=aarch64-linux-gnu
)