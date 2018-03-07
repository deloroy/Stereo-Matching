cmake_minimum_required(VERSION 3.6)

file(TO_CMAKE_PATH "$ENV{Imagine_DIR}" d)

if (NOT EXISTS "${d}")

    message(FATAL_ERROR "Error: environment variable Imagine_DIR=" "${d}")

endif (NOT EXISTS "${d}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

find_package(Imagine)
find_package(OpenMP)


add_executable(Mopsi main.cpp imageProcessingFunc.cpp imageProcessingFunc.h
        shorterPath.cpp shorterPath.h groundControlPoint.cpp
        groundControlPoint.h)
ImagineUseModules(Mopsi Images)