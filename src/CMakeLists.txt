# list sources explicitly, so if we add/remove sources cmake 
# knows to update makefiles.
set(SOURCES 
  bag_oa.cpp
  ${CMAKE_SOURCE_DIR}/include/bag_oa.hpp
  )

# setup link library path.  Must call before defining target.
link_directories( $ENV{OA_LINK_DIR} )

# build shared library
add_library( bag_oa SHARED ${SOURCES} )

# setup include libraries
include_directories( ${CMAKE_SOURCE_DIR}/include
  $ENV{OA_INCLUDE_DIR}
  )

message(status "** OA_LINK_DIR: $ENV{OA_LINK_DIR}")
message(status "** OA_INCLUDE_DIR: $ENV{OA_INCLUDE_DIR}")

# shared library dependencies
target_link_libraries( bag_oa oaCommon oaBase oaPlugIn oaDM oaTech oaDesign ${CMAKE_DL_LIBS} )

# set shared library file folder
set_property( TARGET bag_oa PROPERTY FOLDER "libraries" )

# install target to folders
install( TARGETS bag_oa
  RUNTIME DESTINATION ${bag_oa_BINARY_DIR}/bin
  LIBRARY DESTINATION ${bag_oa_BINARY_DIR}/lib
  )
