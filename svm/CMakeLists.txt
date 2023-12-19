add_executable(svm)

target_compile_options(svm PRIVATE ${COMPILE_OPTIONS})
target_compile_features(svm PRIVATE ${COMPILE_FEATURES})

target_sources(svm PRIVATE
	src/main.c
)

target_include_directories(svm PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/core/src
    ${CMAKE_SOURCE_DIR}/print/include
)

target_link_libraries(svm PRIVATE
    ${SDL2_LIBRARIES}

    ${CMAKE_BUILD_DIR}/libcore.dll
    ${CMAKE_BUILD_DIR}/libprint.dll
)

set_target_properties(svm PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)