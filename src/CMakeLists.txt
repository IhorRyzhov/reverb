cmake_minimum_required(VERSION 3.15)

include(ExternalProject)

project(reverb)

add_executable(${CMAKE_PROJECT_NAME}
	main.c
)

add_dependencies(${CMAKE_PROJECT_NAME} 
	project-portaudio 
	project-liquid-dsp
)
	
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/asoundlib	
	${CMAKE_SOURCE_DIR}/liquid-dsp
	${CMAKE_BINARY_DIR}/../portaudio
)

target_link_libraries(${CMAKE_PROJECT_NAME}
	m
	pthread
	libasound.so
	libliquid.a
	libportaudio.a
)

ExternalProject_Add(project-portaudio
    PREFIX              ${CMAKE_CURRENT_SOURCE_DIR}/portaudio
	SOURCE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/portaudio
	BINARY_DIR          ${CMAKE_BINARY_DIR}/../portaudio
	TMP_DIR             ${CMAKE_BINARY_DIR}/../portaudio
	STAMP_DIR           ${CMAKE_BINARY_DIR}/../portaudio
	CMAKE_ARGS
		-DALSA_INCLUDE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/asoundlib
		-DALSA_LIBRARY=${CMAKE_CURRENT_SOURCE_DIR}/asoundlib/libasound.so
		-DCMAKE_C_STANDARD_INCLUDE_DIRECTORIES=${CMAKE_CURRENT_SOURCE_DIR}/asoundlib
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