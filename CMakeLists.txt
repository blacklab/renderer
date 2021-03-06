cmake_minimum_required(VERSION 3.2)

add_compile_options(-std=c++1y)
add_compile_options(
    -Wall -Wextra -Werror -Wno-missing-braces
    -Wno-deprecated-declarations
)

option(COVERAGE "Generate coverage data" OFF)
if(COVERAGE)
    add_compile_options(-O0 -g)
    add_compile_options(-fprofile-arcs -ftest-coverage)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif(COVERAGE)

# Add external projects

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/vendor)
add_subdirectory(${EXT_PROJECTS_DIR}/assimp)
add_subdirectory(${EXT_PROJECTS_DIR}/cereal)
add_subdirectory(${EXT_PROJECTS_DIR}/docopt)
add_subdirectory(${EXT_PROJECTS_DIR}/openmesh)
add_subdirectory(${EXT_PROJECTS_DIR}/threadpool)

include_directories(
	${assimp_INCLUDE_DIR}
	${cereal_INCLUDE_DIR}
	${docopt_INCLUDE_DIR}
	${threadpool_INCLUDE_DIR}
	${openmesh_INCLUDE_DIR}
)

# Turner sources and executables

file(GLOB TURNER_SRCS
	lib/*.cpp
	lib/*.h
)

add_library(turner OBJECT ${TURNER_SRCS})
add_dependencies(turner assimp cereal zlibstatic)

add_library(main OBJECT main.cpp)
add_dependencies(main assimp zlibstatic cereal docopt threadpool)

find_package(Threads REQUIRED)

add_executable(raycaster
	raycaster.cpp $<TARGET_OBJECTS:main> $<TARGET_OBJECTS:turner>
)
target_link_libraries(raycaster
	${assimp_LIBRARIES} ${docopt_LIBRARIES} Threads::Threads
)

add_executable(raytracer
	raytracer.cpp $<TARGET_OBJECTS:main> $<TARGET_OBJECTS:turner>
)
target_link_libraries(raytracer
	${assimp_LIBRARIES} ${docopt_LIBRARIES} Threads::Threads
)

add_executable(pathtracer
	pathtracer.cpp $<TARGET_OBJECTS:main> $<TARGET_OBJECTS:turner>
)
target_link_libraries(pathtracer
	${assimp_LIBRARIES} ${docopt_LIBRARIES} Threads::Threads
)

add_executable(radiosity radiosity.cpp $<TARGET_OBJECTS:turner>)
add_dependencies(radiosity cereal docopt openmesh threadpool)
target_link_libraries(radiosity
	Threads::Threads
	${assimp_LIBRARIES}
	${docopt_LIBRARIES}
	${openmesh_LIBRARIES}
)

# Add tests

enable_testing(true)
add_subdirectory(tests)
