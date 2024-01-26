set(OUTPUT_NAME JellyCram)

include_directories(include)
include_directories("${PROJECT_SOURCE_DIR}/common/include")

if (WINDOWS)
    set(LIB_HOP "${PROJECT_SOURCE_DIR}/common/windows/libHop-windows.a")
    set(LIB_JGL "${PROJECT_SOURCE_DIR}/common/windows/libjGL-windows.a")
    add_compile_definitions(WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
elseif(OSX)
    set(LIB_HOP "${PROJECT_SOURCE_DIR}/common/macos/libHop-macos.a")
    set(LIB_JGL "${PROJECT_SOURCE_DIR}/common/macos/libjGL-macos.a")   
else ()
    add_link_options(-no-pie)
    set(LIB_HOP "${PROJECT_SOURCE_DIR}/common/linux/libHop-linux-x86_64.a")
    set(LIB_JGL "${PROJECT_SOURCE_DIR}/common/linux/libjGL-linux-x86_64.a")
endif()

if (PROFILE)
    # apt-get install google-perftools libgoogle-perftools-dev google-pprof
    # CPUPROFILE=prof.out ./OUTPUT_NAME
    # google-pprof --pdf OUTPUT_NAME prof.out > sbt.pdf
    add_link_options("-Wl,--no-as-needed,-lprofiler,--as-needed")
endif()

add_executable(${OUTPUT_NAME} "src/main.cpp")

target_compile_definitions(${OUTPUT_NAME} PUBLIC GLSL_VERSION="330")

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(${OUTPUT_NAME} 
    ${LIB_JGL}
    ${X11_LIBRARIES}
    ${OPENGL_LIBRARIES} 
    ${Vulkan_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${PNG_LIBRARIES}
    ${LIB_HOP}
)

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")

file(GLOB LUA "${PROJECT_SOURCE_DIR}/common/*.lua")
file(GLOB MAP "${PROJECT_SOURCE_DIR}/common/*.hmap")

file(COPY ${LUA} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")
file(COPY ${MAP} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")
