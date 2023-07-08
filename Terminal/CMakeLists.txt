file(GLOB WHIST_TERMINAL_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(WhistTerminal ${WHIST_TERMINAL_SRC})
target_link_libraries(WhistTerminal PUBLIC WhistLogic)

# Include generated config file.
target_include_directories(WhistTerminal PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )