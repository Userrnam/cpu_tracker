cmake_minimum_required(VERSION 3.14)
project(cpu_tracker LANGUAGES C)

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
	# using Clang
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	# using GCC
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
else()
	# it's some other compiler
	message(WARNING "only gcc and clang are tested")
endif()

# build cpu_tracker
add_executable(${PROJECT_NAME}
	src/reader.c
	src/printer.c
	src/main.c
	src/ring_buffer.c
	src/analyzer.c
	src/logger.c
)

# build tests
add_executable(tests
	test/main.c
	src/reader.c
	src/printer.c
	src/ring_buffer.c
	src/analyzer.c
	src/logger.c
)

target_include_directories(tests PUBLIC src)

