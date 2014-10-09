include_directories(${PULSEAUDIO_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})

set(SRC_LIST
    client.cpp
    context.cpp
    pulseaudio.cpp
    pulseobject.cpp
    sink.cpp
    sinkinput.cpp
    source.cpp
    sourceoutput.cpp
)

add_library(plasma-volume-declarative SHARED plugin.cpp ${SRC_LIST})
target_link_libraries(plasma-volume-declarative
    Qt5::Quick
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
)
install(TARGETS plasma-volume-declarative DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/volume)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/volume)

add_subdirectory(kcm)
