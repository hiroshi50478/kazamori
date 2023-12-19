# -mwindows - убирает консоль при запуске *.exe

cmake_minimum_required(VERSION 3.26.3 FATAL_ERROR)
project(
	project_nn
	VERSION 0.3.2
	DESCRIPTION "neural network..."
	LANGUAGES C
)

option(core_compile "" ON)
option(print_compile "" OFF)
option(svm_compile "" OFF)
option(app_compile "" ON)

set(COMPILE_OPTIONS -Wall -Wextra -Wpedantic)
set(COMPILE_FEATURES c_std_23)

if(svm_compile OR app_compile)
	set(SDL2_DIR ${CMAKE_SOURCE_DIR}/external/SDL2-2.28.5/cmake)
	# if(${CMAKE_BUILD_TYPE} EQUAL Debug)
		set(SDL2_NO_MWINDOWS ON)
	# endif()
	find_package(SDL2 REQUIRED)
	configure_file(
		${CMAKE_SOURCE_DIR}/external/SDL2-2.28.5/x86_64-w64-mingw32/bin/SDL2.dll
		${CMAKE_BINARY_DIR}/SDL2.dll COPYONLY
	)
endif()

if(core_compile)
	message("   CORE COMPILATION")
	add_subdirectory(core)
endif()

if(print_compile)
	message("   PRINT COMPILATION")
	add_subdirectory(print)
endif()

if(svm_compile)
	message("   SVM COMPILATION")
	add_subdirectory(svm)
endif()

if(app_compile)
	message("   APP COMPILATION")
	add_subdirectory(app)
endif()
