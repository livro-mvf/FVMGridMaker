# ------------------------------------------------------------
# Tests configuration
# ------------------------------------------------------------

if(BUILD_TESTS)
  message(STATUS "Building Tests...")
  include(CTest)
  enable_testing()
  include(FetchContent)
  
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  add_subdirectory(tests)
endif()