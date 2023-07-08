file(GLOB WHIST_LOGIC_SRC CONFIGURE_DEPENDS "*.cpp" "*.h")
add_library(WhistLogic ${WHIST_LOGIC_SRC})

target_link_libraries(WhistLogic PUBLIC whist_compiler_flags)

# System includes from the source directory are used to access header files.
target_include_directories(WhistLogic SYSTEM PUBLIC
                           "${CMAKE_SOURCE_DIR}/"
                           )
