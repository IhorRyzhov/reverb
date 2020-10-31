cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchains/arm-toolchain.cmake)

set(TARGET_NAME "Reverb")

project(${TARGET_NAME})

find_package(Threads REQUIRED)

add_executable(
	${TARGET_NAME} 
	main.c
	gpio/gpio.c
	spi/spi.c
	display/display.c
	display/st7735.c
	wav-reader/wav-reader.c
	input-output-device/input-output-device.c
	encoder/encoder.c
)
	
target_link_libraries(
	${TARGET_NAME} 
	pthread 
	${CMAKE_SOURCE_DIR}/third_party/portaudio/lib/.libs/libportaudio.so
)