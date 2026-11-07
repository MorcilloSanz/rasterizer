add_executable(main main.c)

target_link_libraries(main PRIVATE rasterizer)

# Copy the test assets next to the executable so the program can load them at runtime
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/Roboto-Light.ttf
        $<TARGET_FILE_DIR:main>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/sprite.png
        $<TARGET_FILE_DIR:main>
    COMMENT "Copying test assets"
)

# Optional target: cmake --build build --target run
# Runs main from its own directory (so it finds the assets and writes output.png there)
add_custom_target(run
    COMMAND main
    WORKING_DIRECTORY $<TARGET_FILE_DIR:main>
    DEPENDS main
    COMMENT "Running main"
)