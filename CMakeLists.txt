cmake_minimum_required(VERSION 2.6)
project(CGProject)
set(ROOT "${CMAKE_CURRENT_LIST_DIR}")

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
# Add your project files
include_directories("include/")
set(SRC_DIR "src/")
file(GLOB SRCFILES "${SRC_DIR}/*.cpp")
set(HW2FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/Plane.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/Sphere.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/Triangle.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/TriangleMesh.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/WaveFrontParser.cpp" include/WaveFrontParser.h include/write_ppm.h src/write_ppm.cpp include/Intersection.h src/Intersection.cpp src/lambertian_reflection.cpp include/lambertian_reflection.h src/Camera.cpp include/AABB.h src/AABB.cpp include/BVH.h src/BVH.cpp include/AreaLight.h src/AreaLight.cpp include/sampling.h src/sampling.cpp include/Scene.h)
list(REMOVE_ITEM SRCFILES ${HW2FILES})
list(APPEND SRCFILES main.cpp)
# create executable
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

if(HW2LIB_DIR)
    link_directories(${HW2LIB_DIR})
endif()

add_executable(${PROJECT_NAME} ${SRCFILES} ${LIBIGL_EXTRA_SOURCES})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ROOT}/Eigen/)

if(HW2LIB_DIR)
else()
    add_library(hw2 ${HW2FILES})
     target_include_directories(hw2 SYSTEM PUBLIC ${ROOT}/Eigen)
endif()
target_link_libraries(${PROJECT_NAME} hw2)
