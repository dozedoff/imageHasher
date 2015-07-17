cmake_minimum_required(VERSION 2.8)
project(imageHasher)

# Add additional cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Options
option (BUILD_TESTS "Build and run unit tests" ON)

# Use code coverage
include(CodeCoverage)

# Add depdency project
add_subdirectory(commoncpp)


# Collect source files
file(GLOB_RECURSE tests src/test/*.cpp)
file(GLOB odbtablesrc src/main/table/*.c*)
file(GLOB ihcommonsrc src/main/*.cpp)

# Include additional header files
include_directories(src/include)
include_directories(/usr/include/GraphicsMagick/)
include_directories(cppzmq)
include_directories(commoncpp/src/include)

# Find libraries
find_library(gtest gtest)
find_library(log4cplus log4cplus)
find_library(odb odb)
find_library(odbsqlite odb-sqlite)
find_library(zmq zmq)

## Find packages

# Add required boost libraries
find_package(Boost 1.36.0 COMPONENTS log filesystem system thread chrono program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Add GTest library
find_package(GTest)

# Libraries to build
add_library(odbtable SHARED ${odbtablesrc})
add_library(ihcommon SHARED ${ihcommonsrc})

# Executables to build
add_executable(imageHasher src/ImageHasher.cpp)
add_executable(hashUtil src/HashUtil.cpp)
add_executable(computeClient src/pHashComputeClient.cpp)

# Linking
target_link_libraries(odbtable ${odb} ${odbsqlite})
target_link_libraries(ihcommon commoncpp odbtable zmq ${log4cplus} ${Boost_LIBRARIES})

target_link_libraries(imageHasher ihcommon commoncpp)
target_link_libraries(hashUtil ihcommon commoncpp ${Boost_LIBRARIES})
target_link_libraries(computeClient ihcommon commoncpp ${Boost_LIBRARIES})

# Copy resources
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/logs.properties ./logs.properties COPYONLY)

# Only build tests if GTest is installed and option is selected
if(GTEST_FOUND AND BUILD_TESTS)
	add_executable (imagehasher_test ${tests})
	target_link_libraries(imagehasher_test ihcommon ${GTEST_LIBRARIES})

	# copy test files to build directory
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/commoncpp DESTINATION ${CMAKE_BINARY_DIR})

	enable_testing()
	add_test(NAME imagehasher_test COMMAND imagehasher_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	add_custom_command(TARGET imagehasher_test POST_BUILD COMMAND ctest -V -R imagehasher_test)
elseif(GTEST_FOUND)
	message("Skipping tests...")
else()
	message(WARNING "GTest library not found, skipping tests.")
endif()

# Ignore in coverage report
set(LCOV_REMOVE_EXTRA 'test/*' 'commoncpp/*' 'cppzmq/*' '*-odb.*')

# Coverage target
setup_target_for_coverage(
        cov             	# Name for custom target.
        imagehasher_test        # Name of the test driver executable that runs the tests.
        coverage        	# Name of output directory.
)
