include_directories(${CANBERRA_INCLUDE_DIR})

set(cpp_SRCS
    context.cpp
)

add_library(QCanberraPrivate SHARED ${cpp_SRCS})

target_link_libraries(QCanberraPrivate
    Qt5::Core
    ${CANBERRA_LIBRARIES}
)

install(TARGETS QCanberraPrivate ${INSTALL_TARGETS_DEFAULT_ARGS})
