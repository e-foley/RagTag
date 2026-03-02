# Group source files
# NOTE: Inclusion of header files here is for convenience in Visual Studio
set(SRC_FILES
    tagging/tag_map.h
    tagging/tag_map.cpp
    ui/about_dialog.h
    ui/about_dialog.cpp
    ui/main_frame.h
    ui/main_frame.cpp
    ui/rag_tag_app.h
    ui/rag_tag_app.cpp
    ui/rag_tag_util.h
    ui/rag_tag_util.cpp
    ui/summary_frame.h
    ui/summary_frame.cpp
    ui/tag_entry_dialog.h
    ui/tag_entry_dialog.cpp
    ui/tag_toggle_panel.h
    ui/tag_toggle_panel.cpp)

add_executable(RagTag WIN32 ${SRC_FILES} app.rc)

target_include_directories(RagTag PRIVATE
		           ${CMAKE_CURRENT_SOURCE_DIR}
                           "../libs/json/include"
                           "../libs/wxWidgets/include"
                           "../libs/wxWidgets/include/msvc")

# TODO: The following line doesn't seem to link the expected libraries. Learn why.
target_link_libraries(RagTag wx::net wx::core wx::base)

# Instead, link libraries one by one.
set(VC_X64_DIR ${CMAKE_BINARY_DIR}/libs/wxWidgets/lib/vc_x64_lib)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
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
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
target_link_libraries(RagTag
                      ${VC_X64_DIR}/wxbase33u_xml.lib
                      ${VC_X64_DIR}/wxexpat.lib
                      ${VC_X64_DIR}/wxscintilla.lib
                      ${VC_X64_DIR}/wxlexilla.lib
                      ${VC_X64_DIR}/wxmsw33u_html.lib
                      ${VC_X64_DIR}/wxmsw33u_qa.lib
                      ${VC_X64_DIR}/wxmsw33u_xrc.lib
                      ${VC_X64_DIR}/wxmsw33u_aui.lib
                      ${VC_X64_DIR}/wxmsw33u_propgrid.lib
                      ${VC_X64_DIR}/wxmsw33u_ribbon.lib
                      ${VC_X64_DIR}/wxmsw33u_richtext.lib
                      ${VC_X64_DIR}/wxmsw33u_media.lib
                      ${VC_X64_DIR}/wxmsw33u_stc.lib
                      ${VC_X64_DIR}/wxmsw33u_webview.lib
                      ${VC_X64_DIR}/wxmsw33u_gl.lib)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RagTag PROPERTY CXX_STANDARD 20)
endif()

add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

# TODO: Add tests and install targets if needed.
