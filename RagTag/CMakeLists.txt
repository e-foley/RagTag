set(wxBUILD_SHARED OFF)

# Group source files
set(SRC_FILES
    main_frame.h
    main_frame.cpp
    RagTag.h
    RagTag.cpp
    rag_tag_app.h
    rag_tag_app.cpp
    tag_map.h
    tag_map.cpp)

add_executable(RagTag WIN32 ${SRC_FILES})


target_include_directories(RagTag PRIVATE
                           "../json/include"
                           "../wxWidgets/include"
                           "../wxWidgets/include/msvc")

add_compile_definitions(_UNICODE)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RagTag PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
