# Group source files
set(SRC_FILES
    main_frame.h
    main_frame.cpp
    rag_tag_app.h
    rag_tag_app.cpp
    rag_tag_util.h
    rag_tag_util.cpp
    summary_frame.h
    summary_frame.cpp
    tag_entry_dialog.h
    tag_entry_dialog.cpp
    tag_map.h
    tag_map.cpp
    tag_toggle_panel.h
    tag_toggle_panel.cpp)

add_executable(RagTag WIN32 ${SRC_FILES})

target_include_directories(RagTag PRIVATE
                           "../libs/json/include"
                           "../libs/wxWidgets/include"
                           "../libs/wxWidgets/include/msvc")

# TODO: The following line doesn't seem to link the expected libraries. Learn why.
target_link_libraries(RagTag wx::net wx::core wx::base)

# Instead, link libraries one by one.
set(VC_X64_DIR ${CMAKE_BINARY_DIR}/libs/wxWidgets/lib/vc_x64_lib)
target_link_libraries(RagTag
                      ${VC_X64_DIR}/wxbase33ud_xml.lib
                      ${VC_X64_DIR}/wxexpatd.lib
                      ${VC_X64_DIR}/wxscintillad.lib
                      ${VC_X64_DIR}/wxlexillad.lib
                      ${VC_X64_DIR}/wxmsw33ud_html.lib
                      ${VC_X64_DIR}/wxmsw33ud_qa.lib
                      ${VC_X64_DIR}/wxmsw33ud_xrc.lib
                      ${VC_X64_DIR}/wxmsw33ud_aui.lib
                      ${VC_X64_DIR}/wxmsw33ud_propgrid.lib
                      ${VC_X64_DIR}/wxmsw33ud_ribbon.lib
                      ${VC_X64_DIR}/wxmsw33ud_richtext.lib
                      ${VC_X64_DIR}/wxmsw33ud_media.lib
                      ${VC_X64_DIR}/wxmsw33ud_stc.lib
                      ${VC_X64_DIR}/wxmsw33ud_webview.lib
                      ${VC_X64_DIR}/wxmsw33ud_gl.lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RagTag PROPERTY CXX_STANDARD 20)
endif()

add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

# TODO: Add tests and install targets if needed.
