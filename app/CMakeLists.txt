add_executable(app)

target_compile_options(app PRIVATE ${COMPILE_OPTIONS})
target_compile_features(app PRIVATE ${COMPILE_FEATURES})

target_sources(app PRIVATE
	src/main.c
)

target_include_directories(app PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/core/src
    ${CMAKE_SOURCE_DIR}/print/include
)

target_link_libraries(app PRIVATE
    ${SDL2_LIBRARIES}

    ${CMAKE_BINARY_DIR}/libcore.dll
    # ${CMAKE_BINARY_DIR}/libprint.dll
)

set_target_properties(app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)