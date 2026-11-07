add_library(rasterizer rasterizer.c)
add_library(rasterizer::rasterizer ALIAS rasterizer)

target_include_directories(rasterizer
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)

# stb_truetype and friends need libm on Linux/macOS
if(NOT WIN32)
    target_link_libraries(rasterizer PUBLIC m)
endif()

if(MSVC)
    target_compile_options(rasterizer PRIVATE /W3)
else()
    target_compile_options(rasterizer PRIVATE -Wall -Wno-sign-compare)
endif()