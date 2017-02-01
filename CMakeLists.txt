# CMake 3.4.3 required to export all symbols on windows.
cmake_minimum_required(VERSION 3.4.3)

project(bag_oa)

# organize targets into folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# automatically export all DLL symbols on windows.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# set build configuration for make
set(CMAKE_BUILD_TYPE Release)

# set platform dependent compilation options
if (UNIX)
  message(status "** Setting GCC flags")
  # -fexceptions : enable exception handling
  # -g : include symbol information in executable for debugging.
  # -Wall : enable all warnings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -Wall -Wno-unused-variable")
else()
  message(status "** Setting MSVC flags")
  # s flag: enable C++ exceptions handling
  # c flag: functions declared as extern "C" will never throw C++ exception.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

message(status "** CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(SOURCES
  src/bag_oa.cpp
  src/bag_oa.h
  src/test.cpp
  )

link_directories(/tools/projects/erichang/BAG_2.0/oa_dist/lib/linux_rhel50_gcc44x_64/opt)

add_executable(testoa ${SOURCES})

# setup include directories
include_directories(
  /tools/projects/erichang/BAG_2.0/oa_dist/include/oa
  )

# test executable dependencies (transitive)
target_link_libraries(testoa
  oaCommon
  oaBase
  oaPlugIn
  oaDM
  oaTech
  oaDesign
  ${CMAKE_DL_LIBS}
  )
# set test executable folder
set_property(TARGET testoa PROPERTY FOLDER "executables")

# install targets to folders
install(TARGETS testoa
        RUNTIME DESTINATION ${psf_BINARY_DIR}/bin
        LIBRARY DESTINATION ${psf_BINARY_DIR}/bin
)
