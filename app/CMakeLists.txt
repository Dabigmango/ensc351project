cmake_minimum_required(VERSION 3.18)

project(project_app C)
include_directories(include)
file(GLOB MY_SOURCES "src/*.c")

# --- Executable --- 
add_executable(project ${MY_SOURCES})

find_package(ALSA REQUIRED)

# --- Include directories for app and HAL ---
target_include_directories(project PRIVATE
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/hal/include
 ${LIBGPIOD_INCLUDE_DIRS}
 /usr/lib/aarch64-linux-gnu
)

# --- Link against HAL library and libgpiod ---
target_link_libraries(project PRIVATE
 hal
 ${LIBGPIOD_LIBRARIES}
)

# --- Compiler warnings / sanitizers ---
target_compile_options(project PRIVATE
 -Wall -Werror -Wpedantic -Wextra -fdiagnostics-color -fsanitize=address -pthread
)
target_link_options(project PRIVATE 
-fsanitize=address -pthread
)

# --- Copy executable to NFS ---
add_custom_command(TARGET project POST_BUILD 
COMMAND ${CMAKE_COMMAND} -E copy
 $<TARGET_FILE:project>
 /home/mingchen/ensc351/public/
COMMENT "Copying executable to NFS directory"
)


