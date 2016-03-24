cmake_minimum_required(VERSION 2.8)
project(imageHasher CXX)

### User options ###

option(BUILD_SHARED_LIBS "Build as shared or static library" ON)
option(BUILD_TESTS "Build and run unit tests" ON)

#################################

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

# Add additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Options
option (BUILD_TESTS "Build and run unit tests" ON)

# Boost needs additional flags
if(BUILD_SHARED_LIBS)
        ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
else()
        SET(Boost_USE_STATIC_LIBS ON)
endif()

# Use code coverage
include(CodeCoverage)

# Add depdency project
add_subdirectory(commoncpp)

#Generate ODB files
EXECUTE_PROCESS( COMMAND sh ${CMAKE_SOURCE_DIR}/generate-tables.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Collect source files
file(GLOB_RECURSE tests src/test/*.cpp)
file(GLOB odbtablesrc src/main/table/*.c*)
file(GLOB ihcommonsrc src/main/*.cpp)

# Include additional header files
include_directories(src/main)
include_directories(/usr/include/GraphicsMagick/)
include_directories(cppzmq)
include_directories(commoncpp/src/include)

# Find libraries
find_library(odb odb)
find_library(odbsqlite odb-sqlite)
find_library(zmq zmq)
find_library(sqlite sqlite3)
find_library(avahi-client avahi-client)
find_library(avahi-common avahi-common)

## Find packages

# Add required boost libraries
find_package(Boost 1.36.0 COMPONENTS log filesystem system thread chrono program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Add catch testing framework
include_directories(catch/include)

# Libraries to build
add_library(odbtable ${odbtablesrc})
add_library(ihcommon ${ihcommonsrc})

# Executables to build
add_executable(imageHasher src/ImageHasher.cpp)
add_executable(hashUtil src/HashUtil.cpp)
add_executable(computeClient src/pHashComputeClient.cpp)
add_executable (test_bin ${tests})
add_executable (exam src/exam.cpp)

# Linking
target_link_libraries(odbtable ${odb} ${odbsqlite} ${sqlite})
target_link_libraries(ihcommon commoncpp odbtable ${avahi-client} ${zmq} ${Boost_LIBRARIES})

target_link_libraries(imageHasher ihcommon commoncpp ${Boost_LIBRARIES})
target_link_libraries(hashUtil ihcommon commoncpp ${Boost_LIBRARIES})
target_link_libraries(computeClient ihcommon commoncpp ${Boost_LIBRARIES})
target_link_libraries(test_bin ihcommon)
target_link_libraries(exam ${avahi-client} ${avahi-common})

# Only build tests if the option is selected
if(BUILD_TESTS)
	SET(TEST_BINARY test_${PROJECT_NAME})
	add_executable (${TEST_BINARY} ${tests})
	target_link_libraries(${TEST_BINARY} ihcommon)

	# copy test files to build directory
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/commoncpp DESTINATION ${CMAKE_BINARY_DIR})

	enable_testing()
	add_test(NAME ${TEST_BINARY} COMMAND ${TEST_BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	add_custom_command(TARGET ${TEST_BINARY} POST_BUILD COMMAND ctest -V -R ${TEST_BINARY})
else()
	message("Skipping tests...")
endif()

# Ignore in coverage report
set(LCOV_REMOVE_EXTRA 'test/*' 'commoncpp/*' 'cppzmq/*' '*-odb.*' 'catch/*')

# Coverage target
setup_target_for_coverage(
        cov_${PROJECT_NAME}     # Name for custom target.
        ${TEST_BINARY}        	# Name of the test driver executable that runs the tests.
        coverage        	# Name of output directory.
)
