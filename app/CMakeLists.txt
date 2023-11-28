add_executable(app)

target_compile_options(app PRIVATE ${COMPILE_OPTIONS})
target_compile_features(app PRIVATE ${COMPILE_FEATURES})

target_sources(app PRIVATE
	src/main.c
)

target_include_directories(app PRIVATE
    # ${SDL2_INCLUDE_DIRS}
    core
    print
)

target_link_libraries(app PRIVATE
    # ${SDL2_LIBRARIES}
    
    core
    print
)

set_target_properties(app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)