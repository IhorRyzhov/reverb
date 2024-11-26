cmake_minimum_required(VERSION 3.15)

set(TARGET_NAME "reverb")

project(${TARGET_NAME})

add_executable(
	${TARGET_NAME} 
	main.c
#	gpio/gpio.c
#	spi/spi.c
#	display/display.c
#	display/st7735.c
#	wav-reader/wav-reader.c
#	input-output-device/input-output-device.c
#	encoder/encoder.c
)
	
#target_link_libraries(
#	${TARGET_NAME} 
#	pthread 
#	${CMAKE_SOURCE_DIR}/third_party/portaudio/lib/.libs/libportaudio.so
#)
