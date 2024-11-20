# CMakeList.txt : CMake project for RagTag, include source and define
# project specific logic here.
#

find_package(Catch2 3 REQUIRED)

# Add source to this project's executable.
add_executable (Tests
                "Tests.cpp"
                "../RagTag/tag_map.cpp")

target_include_directories(Tests PRIVATE
                           "../RagTag"
                           "../json/include")

target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Tests PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
