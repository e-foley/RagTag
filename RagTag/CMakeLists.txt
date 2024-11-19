# CMakeList.txt : CMake project for RagTag, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(RagTag
               "RagTag.h"
               "RagTag.cpp"
               "tag_map.h"
               "tag_map.cpp")

target_include_directories(RagTag PRIVATE
                           "../json/include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RagTag PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
