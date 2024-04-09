set(OUTPUT_NAME JellyCram)

include_directories(include)
include_directories("${PROJECT_SOURCE_DIR}/common/headers")
include_directories("${PROJECT_SOURCE_DIR}/common/")

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
    add_compile_definitions(APPLE)
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

add_executable(${OUTPUT_NAME} 
    "src/main.cpp" 
    "${PROJECT_SOURCE_DIR}/common/gameState.cpp"
    "${PROJECT_SOURCE_DIR}/common/common.cpp"
    "${PROJECT_SOURCE_DIR}/common/rand.cpp"
)

target_compile_definitions(${OUTPUT_NAME} PUBLIC GLSL_VERSION="330")

find_package(ZLIB REQUIRED)
if(NOT WINDOWS)
    # ubuntu has a libz-mingw-w64-dev but not a libpng-mingw-w64-dev...
    # there are no link errors...
    find_package(PNG REQUIRED)
endif()
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(${OUTPUT_NAME} 
    ${LIB_JGL}
    ${X11_LIBRARIES}
    ${OPENGL_LIBRARIES} 
    ${Vulkan_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${LIB_HOP}
    ${CMAKE_DL_LIBS}
)

if(NOT WINDOWS)
    target_link_libraries(${OUTPUT_NAME} 
        ${PNG_LIBRARIES}
    )
endif()

target_include_directories(
	${OUTPUT_NAME}
	PUBLIC
	${Vulkan_INCLUDE_DIR} 
)

if (OSX)
    # https://stackoverflow.com/questions/18391487/compiling-with-glfw3-linker-errors-undefined-reference
    target_link_libraries(${OUTPUT_NAME} "-framework Cocoa -framework IOKit -framework CoreVideo")
endif ()

if (WINDOWS)
    target_link_libraries(${OUTPUT_NAME} versioninfo)
endif ()

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")

file(GLOB RES "${PROJECT_SOURCE_DIR}/common/res/*")
file(COPY ${RES} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/res")

if (WINDOWS)
    file(GLOB DLL "${PROJECT_SOURCE_DIR}/common/windows/*.dll")
    file(COPY ${DLL} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")
endif()