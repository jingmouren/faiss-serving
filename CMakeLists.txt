cmake_minimum_required(VERSION 3.17)

project(faiss-serving VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TEST "Build the tests" OFF)
option(USE_GPU "Use GPU in Faiss" OFF)

set(SOURCE_CODE)
set(TEST_CODE src/server-test.cc)

include(FetchContent)

# load faiss
FetchContent_Declare(faiss URL "https://github.com/facebookresearch/faiss/archive/v1.6.4.tar.gz")
if (${USE_GPU})
  set(FAISS_ENABLE_GPU CACHE BOOL ON)
else()
  set(FAISS_ENABLE_GPU CACHE BOOL OFF)
endif()
set(FAISS_ENABLE_PYTHON CACHE BOOL OFF)
FetchContent_MakeAvailable(faiss)

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/simdjson)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/third_party/spdlog/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/simdjson/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/cpp-httplib)
include_directories(${PROJECT_SOURCE_DIR}/third_party/cxxopts/include)

add_executable(faiss-serving src/main.cc ${SOURCE_CODE})
target_link_libraries(faiss-serving faiss simdjson)


if(BUILD_TEST)
  enable_testing()
  add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/googletest)

  include_directories(${gtest_SOURCE_DIR}/include)
  include_directories(${gmock_SOURCE_DIR}/include)

  add_executable(run-test ${TEST_CODE} ${SOURCE_CODE})
  target_link_libraries(run-test gtest gmock gtest_main faiss simdjson)

  add_test(NAME run-test COMMAND ./run-test)
endif()
