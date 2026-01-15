cmake_minimum_required(VERSION 3.20)
project("vrpltw")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(program_executable_name ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS src/*.cpp)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
set(compile_options
  "$<${gcc_like_cxx}:-pedantic-errors;-Wall;-Weffc++;-Wextra;-Wsign-conversion;-Wconversion>"
  "$<${msvc_cxx}:/permissive-;/W4;/WX>"
)

add_executable(${program_executable_name} ${src_files})
target_include_directories(${program_executable_name} PRIVATE include)
target_compile_options(${program_executable_name} PRIVATE ${compile_options})